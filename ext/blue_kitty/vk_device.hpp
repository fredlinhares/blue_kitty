// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DEVICE_HPP
#define BLUE_KITTY_VK_DEVICE_HPP 1

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "loader.hpp"
#include "vk_instance.hpp"

namespace BKVK
{
class Device
{
  friend Loader::Stack<Device>;

  Device(const Device &t) = delete;
  Device& operator=(const Device &t) = delete;
  Device(const Device &&t) = delete;
  Device& operator=(const Device &&t) = delete;

 public:
  explicit Device(const std::shared_ptr<Instance> &instance,
                  const VkPhysicalDevice &vk_physical_device,
                  const std::vector<VkQueueFamilyProperties>
                  &queue_family_properties,
                  bool with_swapchain);
  ~Device();

  inline std::shared_ptr<Instance> get_instance() const
  { return this->instance; };
  inline VkDevice get_vk_device() const { return this->vk_device; };
  inline VkPhysicalDevice get_vk_physical_device() const
  { return this->vk_physical_device; };
  inline VkShaderModule get_vk_vert_shader_module() const
  { return this->vk_vert_shader_module; };
  inline VkShaderModule get_vk_frag_shader_module() const
  { return this->vk_frag_shader_module; };

  uint32_t select_memory_type(VkMemoryRequirements vk_memory_requirements,
                              VkMemoryPropertyFlags vk_property_flags);

 private:
  std::shared_ptr<Instance> instance;
  VkDevice vk_device;
  VkPhysicalDevice vk_physical_device;
  VkShaderModule vk_vert_shader_module;
  VkShaderModule vk_frag_shader_module;

  Loader::Stack<Device> loader;

  bool with_swapchain;

  void load_vk_shaders();
  void unload_vk_shaders();

  VkShaderModule create_shader_module(const std::string& filename);

};
}

#endif /* BLUE_KITTY_VK_DEVICE_HPP */
