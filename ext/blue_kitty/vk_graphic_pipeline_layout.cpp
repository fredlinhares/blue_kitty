// SPDX-License-Identifier: MIT
#include "vk_graphic_pipeline_layout.hpp"

#include <array>

namespace BKVK
{
GraphicPipelineLayout::GraphicPipelineLayout(
      const std::shared_ptr<BKVK::DSL::ModelInstance> &dsl_model_instance,
      const std::shared_ptr<BKVK::DSL::ViewProjection> &dsl_view_projection):
    loader{this},
    device{dsl_view_projection->get_device()},
    dsl_model_instance{dsl_model_instance},
    dsl_view_projection{dsl_view_projection}
{
  this->loader.add(&GraphicPipelineLayout::load_pipeline_layout,
                   &GraphicPipelineLayout::unload_pipeline_layout);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{
      "Could not initialize Vulkan graphic pipeline layout → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

GraphicPipelineLayout::~GraphicPipelineLayout()
{
  this->loader.unload();
}

void GraphicPipelineLayout::load_pipeline_layout()
{
  std::array<VkDescriptorSetLayout, 2> set_layouts{
    this->dsl_model_instance->get_vk_descriptor_set_layout(),
    this->dsl_view_projection->get_vk_descriptor_set_layout()};

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = set_layouts.size();
  pipeline_layout_info.pSetLayouts = set_layouts.data();
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;

  if(vkCreatePipelineLayout(
         this->device->get_vk_device(), &pipeline_layout_info, nullptr,
         &this->vk_pipeline_layout) != VK_SUCCESS)
  {
    throw Loader::Error{"Failed to create Vulkan pipeline layout."};
  }
}

void GraphicPipelineLayout::unload_pipeline_layout()
{
  vkDestroyPipelineLayout(this->device->get_vk_device(),
                          this->vk_pipeline_layout, nullptr);
}

}
