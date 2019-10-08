// SPDX-License-Identifier: MIT
#include "texture.h"
#include "texture_imp.hpp"

#include "engine_imp.hpp"
#include "error.h"
#include "vk_command_pool.hpp"
#include "vk_image.hpp"
#include "vk_queue_family.hpp"
#include "vk_source_buffer.hpp"

#include <iostream>

namespace
{
void move_image_state(
    VkCommandBuffer vk_command_buffer, VkImage vk_image, VkFormat format,
    VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask,
    VkImageLayout old_layout, VkImageLayout new_layout,
    VkPipelineStageFlags source_stage, VkPipelineStageFlags destination_stage)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.pNext = nullptr;
  barrier.srcAccessMask = src_access_mask;
  barrier.dstAccessMask = dst_access_mask;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = vk_image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  vkCmdPipelineBarrier(
      vk_command_buffer, source_stage, destination_stage, 0, 0, nullptr,
      0, nullptr, 1, &barrier);
}

}

VALUE bk_cTexture;

/*
  Basic functions all Ruby classes need.
 */

void
bk_free_texture(void* obj)
{
  struct bk_texture_data *ptr;
  ptr = static_cast<bk_texture_data*>(obj);

  delete ptr;
}

size_t
bk_memsize_texture(const void* obj)
{
  // TODO
  return 0;
}

static const rb_data_type_t
bk_texture_type = {
  "blue_kitty_texture",
  {0, bk_free_texture, bk_memsize_texture,},
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE
bk_alloc_texture(VALUE klass)
{
  VALUE obj;
  struct bk_texture_data *ptr;

  ptr = new bk_texture_data{};
  ptr->texture = std::make_shared<bk_sTexture>();
  ptr->texture->loader = new Loader::Stack<bk_sTexture>{ptr->texture.get()};
  obj = TypedData_Wrap_Struct(klass, &bk_texture_type, ptr);

  return obj;
}

/*
  External interface.
*/

VALUE
bk_cTexture_initialize(VALUE self, VALUE file_path)
{
  SafeStringValue(file_path);

  bk_texture_data *ptr_d;
  TypedData_Get_Struct(self, struct bk_texture_data, &bk_texture_type, ptr_d);
  std::shared_ptr<bk_sTexture> ptr{ptr_d->texture};

  std::shared_ptr<BKVK::Device> device{BKGE::engine->get_devices()[0]};
  ptr->device = device;

  ptr->loader->add(&bk_sTexture::load_image, &bk_sTexture::unload_image);
  ptr->loader->add(&bk_sTexture::load_sampler, &bk_sTexture::unload_sampler);
  ptr->loader->add(&bk_sTexture::load_view, &bk_sTexture::unload_view);

  ptr->texture_path = StringValueCStr(file_path);

  try
  {
    ptr->loader->load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{"Failed to load texture → "};
    base_error += le.message;
    rb_raise(rb_eRuntimeError, "%s\n", base_error.c_str());
  }

  return self;
}

bk_sTexture::~bk_sTexture()
{
  this->loader->unload();
  delete this->loader;
}

void
bk_sTexture::load_image()
{
  SDL_Surface *image{nullptr};

  // Load file image from file.
  {
    SDL_Surface *raw_surface{nullptr};
    raw_surface = IMG_Load(this->texture_path.c_str());
    if(raw_surface == nullptr)
      rb_raise(bk_eError, "Failed to load image. SDL2_image Error: %s",
               IMG_GetError());

    image = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    if(image == nullptr)
      rb_raise(bk_eError, "Failed to convert image. SDL2 Error: %s",
               SDL_GetError());

    this->width = static_cast<uint32_t>(image->w);
    this->height = static_cast<uint32_t>(image->h);
    this->mip_levels = 1;

    SDL_FreeSurface(raw_surface);
  }

  // Load file image into a vulkan buffer.
  size_t image_size{static_cast<size_t>(
      image->format->BytesPerPixel * image->w * image->h)};
  BKVK::SourceBuffer source_image_buffer{device, image->pixels, image_size};

  // Create vulkan image.
  {
    try
    {
      VkExtent3D vk_extent3d{};
      vk_extent3d.width = this->width;
      vk_extent3d.height = this->height;
      vk_extent3d.depth = 1;

      BKVK::Image::create(
          this->device,
          &this->vk_image,
          &this->vk_device_memory,
          VK_FORMAT_R8G8B8A8_UNORM,
          vk_extent3d,
          this->mip_levels,
          VK_IMAGE_TILING_OPTIMAL,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    }
    catch(BKVK::Image::Error le)
    {
      throw Loader::Error{le.message};
    }
  }

  // Copy image from vulkan buffer into vulkan image.
  {
    auto queue_family{BKGE::engine->get_queues_families_with_graphics()[0]};
    auto queue{queue_family->get_queue()};
    BKVK::CommandPool command_pool{queue_family, 1};
    VkCommandBuffer vk_command_buffer{
      command_pool.get_vk_command_buffers()[0]};

    queue->submit_one_time_command(vk_command_buffer, [&](){
        move_image_state(
            vk_command_buffer, this->vk_image, VK_FORMAT_R8G8B8A8_UNORM,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

        VkBufferImageCopy image_copy{};
        image_copy.bufferOffset = 0;
        image_copy.bufferRowLength = 0;
        image_copy.bufferImageHeight = 0;
        image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy.imageSubresource.mipLevel = 0;
        image_copy.imageSubresource.baseArrayLayer = 0;
        image_copy.imageSubresource.layerCount = 1;
        image_copy.imageOffset = {0, 0, 0};
        image_copy.imageExtent = {this->width, this->height, 1};

        vkCmdCopyBufferToImage(
            vk_command_buffer, source_image_buffer.get_vk_buffer(),
            this->vk_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
            &image_copy);

        move_image_state(
            vk_command_buffer, this->vk_image, VK_FORMAT_R8G8B8A8_UNORM,
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      });
  }

  // Free resources.
  SDL_FreeSurface(image);
}

void
bk_sTexture::unload_image()
{
  vkDestroyImage(this->device->get_vk_device(), this->vk_image, nullptr);
  vkFreeMemory(this->device->get_vk_device(), this->vk_device_memory, nullptr);
}

void
bk_sTexture::load_sampler()
{
  VkSamplerCreateInfo sampler_info{};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.pNext = nullptr;
  sampler_info.flags = 0;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.anisotropyEnable = VK_TRUE;
  sampler_info.maxAnisotropy = 16;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_NEVER;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;
  sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  sampler_info.unnormalizedCoordinates = VK_FALSE;

  if(vkCreateSampler(this->device->get_vk_device(), &sampler_info, nullptr,
                     &this->vk_sampler) != VK_SUCCESS)
    throw Loader::Error{"Failed to create texture sampler."};
}

void
bk_sTexture::unload_sampler()
{
  vkDestroySampler(this->device->get_vk_device(), this->vk_sampler, nullptr);
}

void
bk_sTexture::load_view()
{
  try
  {
    BKVK::Image::create_view(
        this->device,
        &this->vk_view,
        this->vk_image,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_ASPECT_COLOR_BIT);
  }
  catch(BKVK::Image::Error le)
  {
    throw Loader::Error{le.message};
  }
}

void
bk_sTexture::unload_view()
{
  vkDestroyImageView(this->device->get_vk_device(), this->vk_view, nullptr);
}

struct bk_texture_data*
bk_cTexture_get_data(VALUE self)
{
  struct bk_texture_data *ptr;

  TypedData_Get_Struct(self, struct bk_texture_data, &bk_texture_type, ptr);

  return ptr;
}
