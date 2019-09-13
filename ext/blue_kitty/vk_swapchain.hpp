// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_SWAPCHAIN_IMP_HPP
#define BLUE_KITTY_SWAPCHAIN_IMP_HPP 1

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "loader.hpp"
#include "vk_device.hpp"

namespace BKVK
{
class Swapchain
{
  friend class Loader::Stack<Swapchain>;

 public:
  Swapchain(const std::shared_ptr<Device> &device);
  ~Swapchain();

  inline std::shared_ptr<Device> get_device() const
  { return this->device; };
  inline VkSwapchainKHR get_vk_swapchain() const
  { return this->vk_swapchain; };
  inline  VkFormat get_vk_image_format() const
  { return this->vk_image_format; };
  inline std::vector<VkImageView> get_vk_image_views() const
  { return this->vk_image_views; };

 private:
  Loader::Stack<Swapchain> loader;
  std::shared_ptr<Device> device;

  VkSwapchainKHR vk_swapchain;
  VkFormat vk_image_format;
  std::vector<VkImage> vk_images;
  std::vector<VkImageView> vk_image_views;


  void load_swapchain();
  void unload_swapchain();

  void load_image_views();
  void unload_image_views();

};
}

#endif /* BLUE_KITTY_SWAPCHAIN_IMP_HPP */
