// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_view_projection.hpp"

namespace BKVK::DS // Descriptor set.
{

ViewProjection::ViewProjection(
    const std::shared_ptr<DSL::Base> &layout,
    const std::vector<std::shared_ptr<UniformBuffer>> &uniform_buffers):
    loader{this}
{
  this->descriptor_set_layout = layout;
  this->uniform_buffers = uniform_buffers;

  this->loader.add(&ViewProjection::load_pool, &ViewProjection::unload_pool);
  this->loader.add(&ViewProjection::load_sets, &ViewProjection::unload_sets);
  this->loader.add(&ViewProjection::load_buffers,
                   &ViewProjection::unload_buffers);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{
      "Could not initialize Vulkan descriptor set for view projection → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

ViewProjection::~ViewProjection()
{
  this->loader.unload();
}

void ViewProjection::load_pool()
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

void ViewProjection::unload_pool()
{
  vkDestroyDescriptorPool(
      this->descriptor_set_layout->get_device()->get_vk_device(),
      this->vk_descriptor_pool, nullptr);
}

void ViewProjection::load_buffers()
{
  for(size_t i = 0; i < this->uniform_buffers.size(); i++)
  {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = this->uniform_buffers[i]->get_vk_buffer();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UBOViewProjection);

    VkWriteDescriptorSet write_descriptor{};
    write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor.dstSet = this->vk_descriptor_sets[i];
    write_descriptor.dstBinding = 0;
    write_descriptor.dstArrayElement = 0;
    write_descriptor.descriptorCount = 1;
    write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor.pBufferInfo = &buffer_info;
    write_descriptor.pImageInfo = nullptr;
    write_descriptor.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        this->descriptor_set_layout->get_device()->get_vk_device(), 1,
        &write_descriptor, 0, nullptr);
  }
}

void ViewProjection::unload_buffers()
{
}

}
