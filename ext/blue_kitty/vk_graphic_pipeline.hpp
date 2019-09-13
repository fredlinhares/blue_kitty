// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_GRAPHIC_PIPELINE_HPP
#define BLUE_KITTY_GRAPHIC_PIPELINE_HPP 1

#include <memory>
#include <vector>

#include "vk_descriptor_set_view_projection.hpp"
#include "vk_device.hpp"
#include "vk_graphic_pipeline_layout.hpp"
#include "vk_swapchain.hpp"
#include "vk_uniform_buffer.hpp"

namespace BKVK
{
class GraphicPipeline
{
  friend class Loader::Stack<GraphicPipeline>;

  GraphicPipeline(const GraphicPipeline &gp) = delete;
  GraphicPipeline& operator=(const GraphicPipeline &gp) = delete;
  GraphicPipeline(const GraphicPipeline &&gp) = delete;
  GraphicPipeline& operator=(const GraphicPipeline &&gp) = delete;

 public:
  explicit GraphicPipeline(
      const std::shared_ptr<Swapchain> &swapchain,
      const std::shared_ptr<GraphicPipelineLayout> &graphic_pipeline_layout);
  ~GraphicPipeline();

  inline VkRenderPass get_vk_render_pass() const
  { return this->vk_render_pass; };
  inline VkPipeline get_vk_graphic_pipeline() const
  { return this->vk_graphic_pipeline; };
  inline std::shared_ptr<GraphicPipelineLayout>
  get_graphic_pipeline_layout() const
  { return this->graphic_pipeline_layout; };
  inline std::vector<VkFramebuffer> get_swapchain_framebuffers() const
  { return this->swapchain_framebuffers; };
  inline std::shared_ptr<DS::ViewProjection> get_ds_view_projection() const
  { return this->ds_view_projection; };
  inline std::vector<std::shared_ptr<UniformBuffer>>
  get_ub_view_projection() const { return this->ub_view_projection; };

 private:
  std::shared_ptr<Device> device;
  std::shared_ptr<Swapchain> swapchain;
  std::shared_ptr<GraphicPipelineLayout> graphic_pipeline_layout;
  std::vector<VkFramebuffer> swapchain_framebuffers;
  std::vector<std::shared_ptr<UniformBuffer>> ub_view_projection;

  VkRenderPass vk_render_pass;
  VkPipeline vk_graphic_pipeline;

  Loader::Stack<GraphicPipeline> loader;
  std::shared_ptr<DS::ViewProjection> ds_view_projection;

  void load_uniform_buffers();
  void unload_uniform_buffers();

  void load_descriptor_sets();
  void unload_descriptor_sets();

  void load_render_pass();
  void unload_render_pass();

  void load_framebuffer();
  void unload_framebuffer();

  void load_pipeline();
  void unload_pipeline();
};
}

#endif /* BLUE_KITTY_GRAPHIC_PIPELINE_HPP */
