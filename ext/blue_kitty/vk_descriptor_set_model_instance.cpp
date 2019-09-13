// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_model_instance.hpp"

namespace BKVK::DS // Descriptor set.
{
ModelInstance::ModelInstance(
    const std::shared_ptr<DSL::Base> &layout,
    const std::vector<std::shared_ptr<UniformBuffer>> &uniform_buffers):
    loader{this}
{
  this->descriptor_set_layout = layout;
  this->uniform_buffers = uniform_buffers;

  this->loader.add(&ModelInstance::load_pool, &ModelInstance::unload_pool);
  this->loader.add(&ModelInstance::load_sets, &ModelInstance::unload_sets);
  this->loader.add(&ModelInstance::load_buffers,
                   &ModelInstance::unload_buffers);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{
      "Could not initialize Vulkan descriptor set for model instance → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

ModelInstance::~ModelInstance()
{
  this->loader.unload();
}

void ModelInstance::load_buffers()
{
  for(size_t i = 0; i < this->uniform_buffers.size(); i++)
  {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = this->uniform_buffers[i]->get_vk_buffer();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UBOModelInstance);

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

void ModelInstance::unload_buffers()
{
}

}
