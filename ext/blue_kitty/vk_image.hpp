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
}

#endif /* BLUE_KITTY_VK_IMAGE_HPP */
