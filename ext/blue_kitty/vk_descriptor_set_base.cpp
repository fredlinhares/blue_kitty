// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_base.hpp"

namespace BKVK::DS // Descriptor set.
{

void Base::load_pool()
{
  VkDescriptorPoolSize descriptor_pool_size{};
  descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_pool_size.descriptorCount = this->uniform_buffers.size();

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.pNext = nullptr;
  pool_info.flags = 0;
  pool_info.maxSets = this->uniform_buffers.size();
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &descriptor_pool_size;

  if(vkCreateDescriptorPool(
         this->descriptor_set_layout->get_device()->get_vk_device(),
         &pool_info, nullptr, &this->vk_descriptor_pool) != VK_SUCCESS)
    throw Loader::Error{"Failed to create a Vulkan descriptor pool."};
}

void Base::unload_pool()
{
  vkDestroyDescriptorPool(
      this->descriptor_set_layout->get_device()->get_vk_device(),
      this->vk_descriptor_pool, nullptr);
}

void Base::load_sets()
{
  std::vector<VkDescriptorSetLayout> layouts(
      this->uniform_buffers.size(),
      this->descriptor_set_layout->get_vk_descriptor_set_layout());

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = this->vk_descriptor_pool;
  alloc_info.descriptorSetCount = this->uniform_buffers.size();
  alloc_info.pSetLayouts = layouts.data();

  vk_descriptor_sets.resize(this->uniform_buffers.size());
  if(vkAllocateDescriptorSets(
         this->descriptor_set_layout->get_device()->get_vk_device(),
         &alloc_info, this->vk_descriptor_sets.data()) != VK_SUCCESS)
    Loader::Error{"Failed to create Vulkan descriptor set."};
}

void Base::unload_sets()
{
  // By destroying the pool the sets are also destroyed.
}

}
