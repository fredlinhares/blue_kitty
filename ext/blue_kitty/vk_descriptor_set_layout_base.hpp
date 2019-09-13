// SPDX-License-Identifier: MIT

#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_BASE_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_BASE_HPP 1

#include <memory>

#include <vulkan/vulkan.h>

#include "vk_device.hpp"

namespace BKVK::DSL // Descriptor set layout.
{
class Base
{
  Base(const Base &b) = delete;
  Base& operator=(const Base &b) = delete;
  Base(const Base &&b) = delete;
  Base& operator=(const Base &&b) = delete;

 public:
  Base(const std::shared_ptr<Device> &device);
  virtual ~Base(){};

  inline std::shared_ptr<Device> get_device() const
  { return this->device; };
  inline VkDescriptorSetLayout get_vk_descriptor_set_layout() const
  { return this->vk_descriptor_set_layout; };

 protected:
  std::shared_ptr<Device> device;
  VkDescriptorSetLayout vk_descriptor_set_layout;

};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_BASE_HPP */
