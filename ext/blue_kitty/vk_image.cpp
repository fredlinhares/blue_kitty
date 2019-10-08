// SPDX-License-Identifier: MIT
#include "vk_image.hpp"

namespace BKVK::Image
{
Error::Error()
{
}

Error::Error(const std::string &m):
    message{m}
{
}

Error::Error(const char &m):
    message{m}
{
}

void create(
    std::shared_ptr<Device> device,
    VkImage *vk_image,
    VkDeviceMemory *vk_image_memory,
    VkFormat vk_format,
    const VkExtent3D &vk_extent3d,
    uint32_t mip_levels,
    VkImageTiling vk_image_tiling,
    VkImageUsageFlags vk_usage)
{
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.pNext = nullptr;
  image_info.flags = 0;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.format = vk_format;
  image_info.extent = vk_extent3d;
  image_info.mipLevels = mip_levels;
  image_info.arrayLayers = 1;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.tiling = vk_image_tiling;
  image_info.usage = vk_usage;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.queueFamilyIndexCount = 0;
  image_info.pQueueFamilyIndices = nullptr;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  if(vkCreateImage(
         device->get_vk_device(), &image_info, nullptr, vk_image) !=
     VK_SUCCESS)
    throw Error{"Failed to create Vulkan image."};

  VkMemoryRequirements vk_memory_requirements;
  vkGetImageMemoryRequirements(device->get_vk_device(), *vk_image,
                               &vk_memory_requirements);

  VkMemoryAllocateInfo memory_alloc_info{};
  memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_alloc_info.allocationSize = vk_memory_requirements.size;
  memory_alloc_info.memoryTypeIndex = device->select_memory_type(
      vk_memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if(vkAllocateMemory(device->get_vk_device(), &memory_alloc_info, nullptr,
                      vk_image_memory) != VK_SUCCESS)
  {
    vkDestroyImage(device->get_vk_device(), *vk_image, nullptr);
    throw Error{"Failed to allocate memory for Vulkan image."};
  }

  vkBindImageMemory(device->get_vk_device(), *vk_image, *vk_image_memory, 0);
}

void create_view(
    std::shared_ptr<Device> device,
    VkImageView *vk_image_view,
    const VkImage &vk_image,
    VkFormat vk_format,
    VkImageAspectFlags vk_image_aspect_flags)
{
  VkImageViewCreateInfo image_view_info{};
  image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_info.pNext = nullptr;
  image_view_info.flags = 0;
  image_view_info.image = vk_image;
  image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_info.format = vk_format;
  image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_info.subresourceRange.aspectMask = vk_image_aspect_flags;
  image_view_info.subresourceRange.baseMipLevel = 0;
  image_view_info.subresourceRange.levelCount = 1;
  image_view_info.subresourceRange.baseArrayLayer = 0;
  image_view_info.subresourceRange.layerCount = 1;

  if(vkCreateImageView(device->get_vk_device(), &image_view_info,
                       nullptr, vk_image_view) != VK_SUCCESS)
    throw Error{"Failed to create texture view."};

}

}
