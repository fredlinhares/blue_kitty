// SPDX-License-Identifier: MIT
#include "vk_graphic_pipeline.hpp"

#include "engine_imp.hpp"
#include "vk_vertex.hpp"

namespace BKVK
{
GraphicPipeline::GraphicPipeline(
    const std::shared_ptr<Swapchain> &swapchain,
    const std::shared_ptr<GraphicPipelineLayout> &graphic_pipeline_layout):
    device{swapchain->get_device()},
    swapchain{swapchain},
    graphic_pipeline_layout{graphic_pipeline_layout},
    loader{this}
{
  this->loader.add(&GraphicPipeline::load_uniform_buffers,
                   &GraphicPipeline::unload_uniform_buffers);
  this->loader.add(&GraphicPipeline::load_descriptor_sets,
                   &GraphicPipeline::unload_descriptor_sets);
  this->loader.add(&GraphicPipeline::load_render_pass,
                   &GraphicPipeline::unload_render_pass);
  this->loader.add(&GraphicPipeline::load_framebuffer,
                   &GraphicPipeline::unload_framebuffer);
  this->loader.add(&GraphicPipeline::load_pipeline,
                   &GraphicPipeline::unload_pipeline);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{"Could not initialize Vulkan graphic pipeline → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

GraphicPipeline::~GraphicPipeline()
{
  this->loader.unload();
}

void GraphicPipeline::load_uniform_buffers()
{
  for(std::vector<std::shared_ptr<UniformBuffer>>::size_type i = 0;
      i < this->swapchain->get_vk_image_views().size(); i++)
    this->ub_view_projection.push_back(std::make_shared<UniformBuffer>(
        this->device, sizeof(UBOViewProjection)));
}

void GraphicPipeline::unload_uniform_buffers()
{
  this->ub_view_projection.clear();
}

void GraphicPipeline::load_descriptor_sets()
{
  this->ds_view_projection = std::make_shared<DS::ViewProjection>(
      this->get_graphic_pipeline_layout()->get_dsl_view_projection(),
      this->ub_view_projection);
}

void GraphicPipeline::unload_descriptor_sets()
{
  this->ds_view_projection = nullptr;
}

void GraphicPipeline::load_render_pass()
{
  VkAttachmentDescription color_attachment = {};
  color_attachment.flags = 0;
  color_attachment.format = this->swapchain->get_vk_image_format();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.flags = 0;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = nullptr;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pResolveAttachments = nullptr;
  subpass.pDepthStencilAttachment = nullptr;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.pNext = nullptr;
  render_pass_info.flags = 0;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if(vkCreateRenderPass(this->device->get_vk_device(), &render_pass_info,
                        nullptr, &this->vk_render_pass) != VK_SUCCESS)
    throw Loader::Error("Failed to create Vulkan Render Pass.");
}

void GraphicPipeline::unload_render_pass()
{
  vkDestroyRenderPass(this->device->get_vk_device(), this->vk_render_pass,
                      nullptr);
}

void GraphicPipeline::load_framebuffer()
{
  auto vk_image_views = this->swapchain->get_vk_image_views();
  this->swapchain_framebuffers.resize(vk_image_views.size());
  for (size_t i = 0; i < vk_image_views.size(); i++)
  {
    VkImageView attachments[] = {
      vk_image_views[i]
    };

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = this->vk_render_pass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width =
        this->device->get_instance()->get_core_data()->screen_width;
    framebuffer_info.height =
        this->device->get_instance()->get_core_data()->screen_height;
    framebuffer_info.layers = 1;

    if(vkCreateFramebuffer(
           this->device->get_vk_device(), &framebuffer_info, nullptr,
           &this->swapchain_framebuffers[i]) != VK_SUCCESS)
      throw Loader::Error{"Failed to create Vulkan Framebuffer."};
  }
}

void GraphicPipeline::unload_framebuffer()
{
  for(auto framebuffer: this->swapchain_framebuffers)
    vkDestroyFramebuffer(this->device->get_vk_device(), framebuffer, nullptr);
}

void GraphicPipeline::load_pipeline()
{
  VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
  vert_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.pNext = nullptr;
  vert_shader_stage_info.flags = 0;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = this->device->get_vk_vert_shader_module();
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
  frag_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.pNext = nullptr;
  frag_shader_stage_info.flags = 0;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = this->device->get_vk_frag_shader_module();
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shader_stages[] = {
    vert_shader_stage_info,
    frag_shader_stage_info
  };

  VkVertexInputBindingDescription vertex_input_binding{};
  vertex_input_binding.binding = 0;
  vertex_input_binding.stride = sizeof(Vertex);
  vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 4> vertex_attribute{};
  // Position.
  vertex_attribute[0].location = 0;
  vertex_attribute[0].binding = 0;
  vertex_attribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attribute[0].offset = offsetof(Vertex, position);
  // Normal.
  vertex_attribute[1].location = 1;
  vertex_attribute[1].binding = 0;
  vertex_attribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attribute[1].offset = offsetof(Vertex, normal);
  // Color.
  vertex_attribute[2].location = 2;
  vertex_attribute[2].binding = 0;
  vertex_attribute[2].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertex_attribute[2].offset = offsetof(Vertex, color);
  // Texture coordinate.
  vertex_attribute[3].location = 3;
  vertex_attribute[3].binding = 0;
  vertex_attribute[3].format = VK_FORMAT_R32G32_SFLOAT;
  vertex_attribute[3].offset = offsetof(Vertex, texture_coord);

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.pNext = nullptr;
  vertex_input_info.flags = 0;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &vertex_input_binding;
  vertex_input_info.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex_attribute.size());
  vertex_input_info.pVertexAttributeDescriptions = vertex_attribute.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.pNext = nullptr;
  input_assembly.flags = 0;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(
      this->device->get_instance()->get_core_data()->screen_width);
  viewport.height = static_cast<float>(
      this->device->get_instance()->get_core_data()->screen_height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = {
    this->device->get_instance()->get_core_data()->screen_width,
    this->device->get_instance()->get_core_data()->screen_height
  };

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.pNext = nullptr;
  viewport_state.flags = 0;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.pNext = nullptr;
  rasterizer.flags = 0;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;
  rasterizer.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.pNext = nullptr;
  color_blending.flags = 0;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
  dynamic_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount = 2;
  dynamic_state_info.pDynamicStates = dynamic_states;

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.pNext = nullptr;
  pipeline_info.flags = 0;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pTessellationState = nullptr;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = nullptr;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_state_info;
  pipeline_info.layout =
      this->graphic_pipeline_layout->get_vk_pipeline_layout();
  pipeline_info.renderPass = this->vk_render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = -1;

  if(vkCreateGraphicsPipelines(
         this->device->get_vk_device(), VK_NULL_HANDLE, 1, &pipeline_info,
         nullptr, &this->vk_graphic_pipeline) != VK_SUCCESS)
    throw Loader::Error{"Failed to create graphics pipeline."};
}

void GraphicPipeline::unload_pipeline()
{
  vkDestroyPipeline(this->device->get_vk_device(), this->vk_graphic_pipeline,
                    nullptr);
}

}
