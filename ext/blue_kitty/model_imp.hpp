// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_MODEL_IMP_HPP
#define BLUE_KITTY_MODEL_IMP_HPP 1

#include <vector>
#include <memory>

#include "texture_imp.hpp"
#include "vk_descriptor_set_model_instance.hpp"
#include "vk_destination_buffer.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_uniform_buffer.hpp"

typedef struct bk_sMesh_t
{
  glm::vec3 color;

  uint32_t vertex_base;
  uint32_t vertex_count;
  uint32_t index_base;
  uint32_t index_count;
} bk_sMesh;

struct bk_model_data
{
  Loader::Stack<bk_model_data> *loader;

  std::string model_path;
  std::shared_ptr<bk_sTexture> texture;

  uint32_t index_count;
  std::shared_ptr<BKVK::DestinationBuffer> index_buffer;
  std::shared_ptr<BKVK::DestinationBuffer> vertex_buffer;

  std::vector<std::shared_ptr<BKVK::UniformBuffer>> ub_model_instance;
  std::shared_ptr<BKVK::DS::ModelInstance> ds_model_instance;

  void load_mesh();
  void unload_mesh();

  void load_uniform_buffers();
  void unload_uniform_buffers();

  void load_descriptor_sets();
  void unload_descriptor_sets();

  void draw(VkCommandBuffer vk_command_buffer,
            uint32_t image_index,
            uint32_t instance_count,
            VkPipeline vk_graphic_pipeline,
            VkDescriptorSet vk_ds_view_projection,
            VkPipelineLayout vk_pipeline_layout);
};

struct bk_model_data*
bk_cModel_get_data(VALUE self);

#endif /* BLUE_KITTY_MODEL_IMP_HPP */
