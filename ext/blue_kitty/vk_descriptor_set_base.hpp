// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_BASE_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_BASE_HPP 1

#include <memory>
#include <vector>

#include "vk_descriptor_set_layout_base.hpp"
#include "vk_uniform_buffer.hpp"

namespace BKVK::DS // Descriptor set.
{
class Base
{
 public:
  virtual ~Base(){};

  inline std::shared_ptr<DSL::Base> get_descriptor_set_layout() const
  { return this->descriptor_set_layout; };
  inline std::vector<VkDescriptorSet> get_vk_descriptor_sets() const
  { return this->vk_descriptor_sets; };

 protected:
  std::vector<std::shared_ptr<UniformBuffer>> uniform_buffers;
  VkDescriptorPool vk_descriptor_pool;
  std::shared_ptr<DSL::Base> descriptor_set_layout;
  std::vector<VkDescriptorSet> vk_descriptor_sets;

  void load_pool();
  void unload_pool();

  void load_sets();
  void unload_sets();

};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_BASE_HPP */
