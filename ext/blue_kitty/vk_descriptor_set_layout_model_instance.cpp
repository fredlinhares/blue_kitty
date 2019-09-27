// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_layout_model_instance.hpp"

#include <array>

namespace BKVK::DSL // Descriptor set layout.
{
ModelInstance::ModelInstance(
    const std::shared_ptr<Device> &device):
    Base{device}
{
  std::array<VkDescriptorSetLayoutBinding, 2> layout_bindings{};

  layout_bindings[0].binding = 0;
  layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_bindings[0].descriptorCount = 1;
  layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layout_bindings[0].pImmutableSamplers = nullptr;

  layout_bindings[1].binding = 1;
  layout_bindings[1].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layout_bindings[1].descriptorCount = 1;
  layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  layout_bindings[1].pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.pNext = nullptr;
  layout_info.flags = 0;
  layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
  layout_info.pBindings = layout_bindings.data();

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
