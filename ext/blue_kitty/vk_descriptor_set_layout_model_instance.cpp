// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_layout_model_instance.hpp"

namespace BKVK::DSL // Descriptor set layout.
{
ModelInstance::ModelInstance(
    const std::shared_ptr<Device> &device):
    Base{device}
{
  VkDescriptorSetLayoutBinding layout_binding{};
  layout_binding.binding = 0;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_binding.descriptorCount = 1;
  layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layout_binding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.pNext = nullptr;
  layout_info.flags = 0;
  layout_info.bindingCount = 1;
  layout_info.pBindings = &layout_binding;

  if(vkCreateDescriptorSetLayout(
         this->device->get_vk_device(), &layout_info, nullptr,
         &this->vk_descriptor_set_layout) != VK_SUCCESS)
    throw Loader::Error{
      "Failed to create Vulkan descriptor set layout for model instance."};
}

ModelInstance::~ModelInstance()
{
  vkDestroyDescriptorSetLayout(this->device->get_vk_device(),
                               this->vk_descriptor_set_layout, nullptr);
}
}
