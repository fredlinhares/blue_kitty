// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_model_instance.hpp"

#include <array>

namespace BKVK::DS // Descriptor set.
{
ModelInstance::ModelInstance(
    const std::shared_ptr<DSL::Base> &layout,
    const std::shared_ptr<bk_sTexture> &texture,
    const std::vector<std::shared_ptr<UniformBuffer>> &uniform_buffers):
    loader{this},
    texture{texture}
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

void ModelInstance::load_pool()
{
  std::array<VkDescriptorPoolSize, 2> descriptor_pool_sizes{};
  descriptor_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_pool_sizes[0].descriptorCount = this->uniform_buffers.size();
  descriptor_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptor_pool_sizes[1].descriptorCount = this->uniform_buffers.size();

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.pNext = nullptr;
  pool_info.flags = 0;
  pool_info.maxSets = this->uniform_buffers.size();
  pool_info.poolSizeCount = descriptor_pool_sizes.size();
  pool_info.pPoolSizes = descriptor_pool_sizes.data();

  if(vkCreateDescriptorPool(
         this->descriptor_set_layout->get_device()->get_vk_device(),
         &pool_info, nullptr, &this->vk_descriptor_pool) != VK_SUCCESS)
    throw Loader::Error{"Failed to create a Vulkan descriptor pool."};
}

void ModelInstance::unload_pool()
{
  vkDestroyDescriptorPool(
      this->descriptor_set_layout->get_device()->get_vk_device(),
      this->vk_descriptor_pool, nullptr);
}

void ModelInstance::load_buffers()
{
  for(size_t i = 0; i < this->uniform_buffers.size(); i++)
  {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = this->uniform_buffers[i]->get_vk_buffer();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UBOModelInstance);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = this->texture->vk_view;
    image_info.sampler = this->texture->vk_sampler;

    std::array<VkWriteDescriptorSet, 2> write_descriptors{};
    write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptors[0].dstSet = this->vk_descriptor_sets[i];
    write_descriptors[0].dstBinding = 0;
    write_descriptors[0].dstArrayElement = 0;
    write_descriptors[0].descriptorCount = 1;
    write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptors[0].pBufferInfo = &buffer_info;
    write_descriptors[0].pImageInfo = nullptr;
    write_descriptors[0].pTexelBufferView = nullptr;

    write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptors[1].dstSet = this->vk_descriptor_sets[i];
    write_descriptors[1].dstBinding = 1;
    write_descriptors[1].dstArrayElement = 0;
    write_descriptors[1].descriptorCount = 1;
    write_descriptors[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_descriptors[1].pBufferInfo = nullptr;
    write_descriptors[1].pImageInfo = &image_info;
    write_descriptors[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(
        this->descriptor_set_layout->get_device()->get_vk_device(),
        write_descriptors.size(), write_descriptors.data(), 0, nullptr);
  }
}

void ModelInstance::unload_buffers()
{
}

}
