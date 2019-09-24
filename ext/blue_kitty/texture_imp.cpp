// SPDX-License-Identifier: MIT
#include "texture.h"
#include "texture_imp.hpp"

#include "engine_imp.hpp"
#include "error.h"
#include "vk_command_pool.hpp"
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

  VkDevice vk_device{ptr->device->get_vk_device()};

  vkDestroyImageView(vk_device, ptr->vk_view, nullptr);
  vkDestroySampler(vk_device, ptr->vk_sampler, nullptr);
  vkDestroyImage(vk_device, ptr->vk_image, nullptr);
  vkFreeMemory(vk_device, ptr->vk_device_memory, nullptr);

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

  struct bk_texture_data *ptr;
  TypedData_Get_Struct(self, struct bk_texture_data, &bk_texture_type, ptr);

  std::shared_ptr<BKVK::Device> device{BKGE::engine->get_devices()[0]};
  ptr->device = device;

  SDL_Surface *image{nullptr};

  // Load file image from file.
  {
    SDL_Surface *raw_surface{nullptr};
    raw_surface = IMG_Load(StringValueCStr(file_path));
    if(raw_surface == nullptr)
      rb_raise(bk_eError, "Failed to load image. SDL2_image Error: %s",
               IMG_GetError());

    image = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    if(image == nullptr)
      rb_raise(bk_eError, "Failed to convert image. SDL2 Error: %s",
               SDL_GetError());

    ptr->width = static_cast<uint32_t>(image->w);
    ptr->height = static_cast<uint32_t>(image->h);
    ptr->mip_levels = 1;

    SDL_FreeSurface(raw_surface);
  }

  // Load file image into a vulkan buffer.
  size_t image_size{static_cast<size_t>(
      image->format->BytesPerPixel * image->w * image->h)};
  BKVK::SourceBuffer source_image_buffer{device, image->pixels, image_size};

  // Create vulkan image.
  {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = nullptr;
    image_info.flags = 0;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_info.extent.width = ptr->width;
    image_info.extent.height = ptr->height;
    image_info.extent.depth = 1;
    image_info.mipLevels = ptr->mip_levels;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                       VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if(vkCreateImage(
           device->get_vk_device(), &image_info, nullptr, &ptr->vk_image) !=
       VK_SUCCESS)
      rb_raise(bk_eError, "%s", "Failed to create Vulkan image.");

    VkMemoryRequirements vk_memory_requirements;
    vkGetImageMemoryRequirements(device->get_vk_device(), ptr->vk_image,
                                 &vk_memory_requirements);

    VkMemoryAllocateInfo memory_alloc_info{};
    memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_alloc_info.allocationSize = vk_memory_requirements.size;
    memory_alloc_info.memoryTypeIndex = device->select_memory_type(
        vk_memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if(vkAllocateMemory(device->get_vk_device(), &memory_alloc_info, nullptr,
                        &ptr->vk_device_memory) != VK_SUCCESS)
      rb_raise(bk_eError, "%s", "Failed to allocate memory for Vulkan image.");

    vkBindImageMemory(device->get_vk_device(), ptr->vk_image,
                      ptr->vk_device_memory, 0);
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
            vk_command_buffer, ptr->vk_image, VK_FORMAT_R8G8B8A8_UNORM,
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
        image_copy.imageExtent = {ptr->width, ptr->height, 1};

        vkCmdCopyBufferToImage(
            vk_command_buffer, source_image_buffer.get_vk_buffer(),
            ptr->vk_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
            &image_copy);

        move_image_state(
            vk_command_buffer, ptr->vk_image, VK_FORMAT_R8G8B8A8_UNORM,
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      });
  }

  // Create samplers.
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

    if(vkCreateSampler(device->get_vk_device(), &sampler_info, nullptr,
                       &ptr->vk_sampler) != VK_SUCCESS)
      rb_raise(bk_eError, "%s", "Failed to create texture sampler.");
  }

  // Create image view.
  {
    VkImageViewCreateInfo image_view_info{};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.pNext = nullptr;
    image_view_info.flags = 0;
    image_view_info.image = ptr->vk_image;
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = 1;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    if(vkCreateImageView(device->get_vk_device(), &image_view_info, nullptr,
                         &ptr->vk_view) != VK_SUCCESS)
      rb_raise(bk_eError, "%s", "Failed to create texture view.");
  }

  // Free resources.
  SDL_FreeSurface(image);

  return self;
}

struct bk_texture_data*
bk_cTexture_get_data(VALUE self)
{
  struct bk_texture_data *ptr;

  TypedData_Get_Struct(self, struct bk_texture_data, &bk_texture_type, ptr);

  return ptr;
}
