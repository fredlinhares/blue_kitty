// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_GRAPHIC_PIPELINE_LAYOUT_HPP
#define BLUE_KITTY_GRAPHIC_PIPELINE_LAYOUT_HPP 1

#include <memory>

#include "vk_descriptor_set_layout_model_instance.hpp"
#include "vk_descriptor_set_layout_view_projection.hpp"

namespace BKVK
{
class GraphicPipelineLayout
{
  friend class Loader::Stack<GraphicPipelineLayout>;

  GraphicPipelineLayout(const GraphicPipelineLayout &gpl) = delete;
  GraphicPipelineLayout& operator=(const GraphicPipelineLayout &gpl) = delete;
  GraphicPipelineLayout(const GraphicPipelineLayout &&gpl) = delete;
  GraphicPipelineLayout& operator=(const GraphicPipelineLayout &&gpl) = delete;

 public:
  explicit GraphicPipelineLayout(
      const std::shared_ptr<BKVK::DSL::ModelInstance> &dsl_model_instance,
      const std::shared_ptr<BKVK::DSL::ViewProjection> &dsl_view_projection);

  ~GraphicPipelineLayout();

  inline VkPipelineLayout get_vk_pipeline_layout() const
  { return this->vk_pipeline_layout; };
  inline std::shared_ptr<DSL::ModelInstance> get_dsl_model_instance() const
  { return this->dsl_model_instance; };
  inline std::shared_ptr<DSL::ViewProjection> get_dsl_view_projection() const
  { return this->dsl_view_projection; };

 private:
  Loader::Stack<GraphicPipelineLayout> loader;

  VkPipelineLayout vk_pipeline_layout;

  std::shared_ptr<Device> device;
  std::shared_ptr<BKVK::DSL::ModelInstance> dsl_model_instance;
  std::shared_ptr<BKVK::DSL::ViewProjection> dsl_view_projection;

  void load_pipeline_layout();
  void unload_pipeline_layout();

};
}

#endif /* BLUE_KITTY_GRAPHIC_PIPELINE_LAYOUT_HPP */
