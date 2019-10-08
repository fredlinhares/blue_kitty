// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_IMAGE_HPP
#define BLUE_KITTY_VK_IMAGE_HPP 1

#include <memory>
#include <string>

#include "vk_device.hpp"

namespace BKVK::Image
{
class Error
{
 public:
  Error();
  Error(const std::string &m);
  Error(const char &m);
  std::string message;
};

void create(
    std::shared_ptr<Device> device,
    VkImage *vk_image,
    VkDeviceMemory *vk_image_memory,
    VkFormat vk_format,
    const VkExtent3D &vk_extent3d,
    uint32_t mip_levels,
    VkImageTiling vk_image_tiling,
    VkImageUsageFlags vk_usage);

void create_view(
    std::shared_ptr<Device> device,
    VkImageView *vk_image_view,
    const VkImage &vk_image,
    VkFormat vk_format,
    VkImageAspectFlags vk_image_aspect_flags);
}

#endif /* BLUE_KITTY_VK_IMAGE_HPP */
