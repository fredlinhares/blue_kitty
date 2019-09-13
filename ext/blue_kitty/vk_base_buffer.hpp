// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_BASE_BUFFER_HPP
#define BLUE_KITTY_VK_BASE_BUFFER_HPP 1

#include <memory>

#include <vulkan/vulkan.h>

#include "loader.hpp"
#include "vk_device.hpp"

namespace BKVK
{
class BaseBuffer
{
 public:
  virtual ~BaseBuffer(){};

  inline VkDeviceSize get_size() const { return this->vk_device_size; };
  inline VkBuffer get_vk_buffer() const { return this->vk_buffer; };

 protected:
  std::shared_ptr<Device> device;

  VkBuffer vk_buffer;
  VkDeviceMemory vk_device_memory;
  VkDeviceSize vk_device_size;
  VkBufferUsageFlags vk_buffer_usage;
  VkMemoryPropertyFlags vk_memory_properties;

  void load_buffer();
  void unload_buffer();

  void load_memory();
  void unload_memory();

};
}

#endif /* BLUE_KITTY_VK_BASE_BUFFER_HPP */
