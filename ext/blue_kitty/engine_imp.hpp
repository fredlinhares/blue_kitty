// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_ENGINE_IMP_HPP
#define BLUE_KITTY_ENGINE_IMP_HPP 1

#include <memory>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "ruby.h"

#include "core_data.h"
#include "loader.hpp"
#include "model_imp.hpp"
#include "vk_command_pool.hpp"
#include "vk_descriptor_set_layout_model_instance.hpp"
#include "vk_descriptor_set_layout_view_projection.hpp"
#include "vk_device.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_graphic_pipeline_layout.hpp"
#include "vk_instance.hpp"
#include "vk_queue_family.hpp"
#include "vk_swapchain.hpp"

namespace BKGE
{
struct ErrRender
{
  std::string err_message;

  ErrRender(const std::string &em);
  ErrRender(const char &em);
};

class Engine
{
  friend class Loader::Stack<Engine>;

 public:
  Engine();
  ~Engine();

  inline std::shared_ptr<bk_sCoreData> get_core_data() const
  { return this->core_data; };
  inline std::vector<std::shared_ptr<BKVK::Device>> get_devices() const
  { return this->devices; };
  inline double get_max_frame_duration() const
  { return this->max_frame_duration; };
  inline std::vector<std::shared_ptr<BKVK::QueueFamily>>
  get_queues_families_with_graphics() const
  { return this->queues_families_with_graphics; };
  inline std::shared_ptr<BKVK::Swapchain> get_swapchain() const
  { return this->swapchain; };
  inline std::shared_ptr<BKVK::GraphicPipelineLayout>
  get_graphic_pipeline_layout() const
  { return this->graphic_pipeline_layout; };

  void load_vk_draw_command_pool();
  void unload_vk_draw_command_pool();

  // Rendering to screen.
  void render(
      VALUE camera,
      const std::unordered_map<VALUE, std::vector<VALUE>> &model_entities);

 private:
  Loader::Stack<Engine> loader;
  std::shared_ptr<bk_sCoreData> core_data;

  VkDebugUtilsMessengerEXT vk_callback;

  std::shared_ptr<BKVK::Instance> instance;
  std::vector<std::shared_ptr<BKVK::Device>> devices;
  std::shared_ptr<BKVK::Device> device_with_swapchain;
  std::vector<std::shared_ptr<BKVK::QueueFamily>> queues_families;
  std::vector<std::shared_ptr<BKVK::QueueFamily>>
  queues_families_with_graphics;
  std::vector<std::shared_ptr<BKVK::QueueFamily>>
  queues_families_with_presentation;

  std::shared_ptr<BKVK::Swapchain> swapchain;
  std::shared_ptr<BKVK::DSL::ModelInstance> dsl_model_instance;
  std::shared_ptr<BKVK::DSL::ViewProjection> dsl_view_projection;
  std::shared_ptr<BKVK::GraphicPipelineLayout> graphic_pipeline_layout;
  std::shared_ptr<BKVK::GraphicPipeline> graphic_pipeline;

  std::unique_ptr<BKVK::CommandPool> draw_command_pool;

  double max_frame_duration;

  // Buffering control.
  const int max_frames_in_flight = 2;
  size_t current_frame = 0;
  std::vector<VkSemaphore> vk_image_available_semaphores;
  std::vector<VkSemaphore> vk_render_finished_semaphores;
  std::vector<VkFence> vk_in_flight_fences;

  // Initialization and destruction.
  void load_variables();
  void unload_variables();

  void load_sdl();
  void unload_sdl();

  void load_window();
  void unload_window();

  void load_vk_instance();
  void unload_vk_instance();

  void load_vk_debug_callback();
  void unload_vk_debug_callback();

  void load_vk_devices();
  void unload_vk_devices();

  void load_vk_swapchain();
  void unload_vk_swapchain();

  void load_vk_descriptor_set_layouts();
  void unload_vk_descriptor_set_layouts();

  void load_vk_graphic_pipeline_layout();
  void unload_vk_graphic_pipeline_layout();

  void load_vk_graphic_pipelines();
  void unload_vk_graphic_pipelines();

  void load_vk_frame_sync();
  void unload_vk_frame_sync();

  glm::mat4 update_ub_model_instance(
      uint32_t image_index, VALUE entity3d, bk_model_data* model_data);
};

extern Engine *engine;
}

#endif /* BLUE_KITTY_ENGINE_IMP_HPP */
