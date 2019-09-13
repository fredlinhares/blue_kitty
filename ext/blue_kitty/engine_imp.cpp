// SPDX-License-Identifier: MIT
#include "engine.h"
#include "engine_imp.hpp"

#include <algorithm>
#include <sstream>

#include <glm/ext.hpp>

#include "error.h"
#include "input_device.h"
#include "log.hpp"
#include "vector3d_imp.hpp"
#include "vk_uniform_buffer.hpp"

namespace
{
static ID id_at_current_camera;
static ID id_at_model;
static ID id_at_position;
static ID id_at_rotation;

static VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  VkDebugUtilsMessageTypeFlagsEXT message_type,
  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
  void* obj)
{
  // bk_sCoreData *self = static_cast<bk_sCoreData*>(obj);

  std::ostringstream txt;

  // Print message severy code.
  txt << "[\e[1;0mVK\e[0;0m ";
  if(message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    txt << "\e[1;32mV\e[0;0m";
  else if(message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    txt << "\e[1;34mI\e[0;0m";
  else if(message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    txt << "\e[1;33mW\e[0;0m";
  else if(message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    txt << "\e[1;31mE\e[0;0m";

  txt << "]";

  // Log message.
  txt << callback_data->pMessage;

  BKGE::Log::standard(txt.str());

  return VK_FALSE;
}

// Families that support fewer functionalities must be at the beginning of the
// vector to give them priority. So more general queue families are used only
// when more specific families are busy.
bool sort_queue(const std::shared_ptr<BKVK::QueueFamily> &a,
                const std::shared_ptr<BKVK::QueueFamily> &b)
{
  return a->num_properties() > b->num_properties();
}

}

namespace BKGE
{

ErrRender::ErrRender(const std::string &em):
    err_message{em}
{
}

ErrRender::ErrRender(const char &em):
    err_message{em}
{
}

Engine::Engine():
    loader{this},
    core_data{std::make_shared<bk_sCoreData>()}
{
  this->core_data->window = nullptr;

  this->loader.add(&Engine::load_variables, &Engine::unload_variables);
  this->loader.add(&Engine::load_sdl, &Engine::unload_sdl);
  this->loader.add(&Engine::load_window, &Engine::unload_window);
  this->loader.add(&Engine::load_vk_instance, &Engine::unload_vk_instance);
  this->loader.add(&Engine::load_vk_debug_callback,
                   &Engine::unload_vk_debug_callback);
  this->loader.add(&Engine::load_vk_devices, &Engine::unload_vk_devices);
  this->loader.add(&Engine::load_vk_swapchain, &Engine::unload_vk_swapchain);
  this->loader.add(&Engine::load_vk_descriptor_set_layouts,
                   &Engine::unload_vk_descriptor_set_layouts);
  this->loader.add(&Engine::load_vk_graphic_pipeline_layout,
                   &Engine::unload_vk_graphic_pipeline_layout);
  this->loader.add(&Engine::load_vk_graphic_pipelines,
                   &Engine::unload_vk_graphic_pipelines);
  this->loader.add(&Engine::load_vk_frame_sync, &Engine::unload_vk_frame_sync);
  this->loader.add(&Engine::load_vk_draw_command_pool,
                   &Engine::unload_vk_draw_command_pool);

  this->loader.load();
}

Engine::~Engine()
{
  // Wait all commands to finish.
  vkDeviceWaitIdle(this->devices[0]->get_vk_device());
  this->loader.unload();
}

void Engine::load_variables()
{
  VALUE config =
      rb_ivar_get(bk_mEngine, rb_intern("@@configurations"));

  if(rb_hash_aref(config, ID2SYM(rb_intern("debug"))) == Qtrue)
    this->core_data->debug = SDL_TRUE;
  else
    this->core_data->debug = SDL_FALSE;

  VALUE screen_resolution =
      rb_hash_aref(config, ID2SYM(rb_intern("screen_resolution")));
  this->core_data->screen_width = FIX2INT(rb_ary_entry(screen_resolution, 0));
  this->core_data->screen_height = FIX2INT(rb_ary_entry(screen_resolution, 1));

  VALUE game_name = rb_hash_aref(config, ID2SYM(rb_intern("game_name")));
  this->core_data->game_name = StringValueCStr(game_name);

  VALUE version =
      rb_hash_aref(config, ID2SYM(rb_intern("version")));
  this->core_data->game_vesion_major =
      FIX2INT(rb_hash_aref(version, ID2SYM(rb_intern("major"))));
  this->core_data->game_vesion_minor =
      FIX2INT(rb_hash_aref(version, ID2SYM(rb_intern("minor"))));
  this->core_data->game_vesion_patch =
      FIX2INT(rb_hash_aref(version, ID2SYM(rb_intern("patch"))));

  int max_fps = FIX2INT(rb_hash_aref(config, ID2SYM(rb_intern("max_fps"))));
  // Time is calculated in mileseconds by SDL.
  this->max_frame_duration = 1000/max_fps;
}

void Engine::unload_variables()
{
}

void Engine::load_sdl()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::string base_error {"SDL could not initialize! SDL Error → "};
    base_error += SDL_GetError();
    throw Loader::Error{base_error};
  }

  if(SDL_Vulkan_LoadLibrary(nullptr) != 0)
  {
    SDL_Quit();
    std::string base_error {"SDL could not initialize Vulkan! SDL_Error → "};
    base_error += SDL_GetError();
    throw Loader::Error{base_error};
  }
}

void Engine::unload_sdl()
{
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();
}

void Engine::load_window()
{
  this->core_data->window = SDL_CreateWindow(
      this->core_data->game_name, SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, this->core_data->screen_width,
      this->core_data->screen_height, SDL_WINDOW_VULKAN |
      SDL_WINDOW_RESIZABLE);

  if(this->core_data->window == nullptr)
  {
    std::string base_error{"Could not create SDL window → "};
    base_error += SDL_GetError();
    throw Loader::Error{base_error};
  }
}

void Engine::unload_window()
{
  SDL_DestroyWindow(this->core_data->window);
}

void Engine::load_vk_instance()
{
  this->instance = std::make_shared<BKVK::Instance>(this->core_data);
}

void Engine::unload_vk_instance()
{
  this->instance = nullptr;
}

void Engine::load_vk_debug_callback()
{
  if(!this->core_data->debug) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  PFN_vkCreateDebugUtilsMessengerEXT debug_messenger;

  // A Vulkan instance extension named VK_EXT_debug_utils and a Vulkan instance
  // layer named VK_LAYER_LUNARG_standard_validation are required to enable
  // this callback. These instance extension and instance layer are loaded at
  // Instance::load_vk_instance.
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = vk_debug_callback;
  create_info.pUserData = this->core_data.get();
  create_info.flags = 0;

  debug_messenger = (PFN_vkCreateDebugUtilsMessengerEXT)
                    vkGetInstanceProcAddr(this->instance->get_vk_instance(),
                                          "vkCreateDebugUtilsMessengerEXT");

  if(debug_messenger(this->instance->get_vk_instance(), &create_info, nullptr,
                     &this->vk_callback) != VK_SUCCESS)
    throw Loader::Error{"Failed to setup debug callback for Vulkan."};
}

void Engine::unload_vk_debug_callback()
{
  if(!this->core_data->debug) return;

  PFN_vkDestroyDebugUtilsMessengerEXT debug_messenger;

  debug_messenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
                    vkGetInstanceProcAddr(this->instance->get_vk_instance(),
                                          "vkDestroyDebugUtilsMessengerEXT");

  debug_messenger(this->instance->get_vk_instance(), this->vk_callback,
                  nullptr);
}

void Engine::load_vk_devices()
{
  uint32_t vk_physical_devices_count{0};
  std::vector<VkPhysicalDevice> vk_physical_devices;

  // Enumerate physical devices
  vkEnumeratePhysicalDevices(
      this->instance->get_vk_instance(), &vk_physical_devices_count, nullptr);
  if(vk_physical_devices_count < 1)
    throw Loader::Error{"Failed to find GPUs with Vulkan support."};
  vk_physical_devices.resize(vk_physical_devices_count);
  vkEnumeratePhysicalDevices(
      this->instance->get_vk_instance(), &vk_physical_devices_count,
      vk_physical_devices.data());

  if(this->core_data->debug)
    Log::header("Physical devices properties");
  this->devices.reserve(vk_physical_devices_count);
  for(uint32_t i = 0; i < vk_physical_devices_count; i++)
  {
    // Get queue families.
    uint32_t queue_family_count;
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_devices[i],
                                             &queue_family_count, nullptr);
    queue_family_properties.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        vk_physical_devices[i], &queue_family_count,
        queue_family_properties.data());

    // Use swapchain on first device.
    if(i == 0)
    {
      this->devices.push_back(std::make_shared<BKVK::Device>(
          this->instance, vk_physical_devices[i], queue_family_properties,
          true));
      this->device_with_swapchain = this->devices.back();
    }
    else
      this->devices.push_back(std::make_shared<BKVK::Device>(
          this->instance, vk_physical_devices[i], queue_family_properties,
          false));

    for(uint32_t ii = 0; ii < queue_family_count; ii++)
    {
      this->queues_families.push_back(std::make_shared<BKVK::QueueFamily>(
          this->devices.back(), ii, queue_family_properties[ii]));

      // Select families with graphics support.
      auto family_properties =
          this->queues_families.back()->get_vk_family_properties();
      if(family_properties. queueCount > 0 &&
         family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        this->queues_families_with_graphics.push_back(
            this->queues_families.back());

      // Select families with presentation support.
      VkBool32 present_supported;
      vkGetPhysicalDeviceSurfaceSupportKHR(
          vk_physical_devices[i], ii, this->instance->get_surface(),
          &present_supported);
      if(present_supported)
        this->queues_families_with_presentation.push_back(
            this->queues_families.back());
    }

    // Organize vectors.
    std::sort(queues_families_with_graphics.begin(),
              queues_families_with_graphics.end(), sort_queue);
    std::sort(queues_families_with_presentation.begin(),
              queues_families_with_presentation.end(), sort_queue);

 }
}

void Engine::unload_vk_devices()
{
  this->queues_families.clear();
  this->queues_families_with_graphics.clear();
  this->queues_families_with_presentation.clear();

  this->devices.clear();
  this->device_with_swapchain = nullptr;
}

void Engine::load_vk_swapchain()
{
  this->swapchain = std::make_shared<BKVK::Swapchain>(
      this->device_with_swapchain);
}

void Engine::unload_vk_swapchain()
{
  this->swapchain = nullptr;
}

void Engine::load_vk_descriptor_set_layouts()
{
  this->dsl_model_instance = std::make_shared<BKVK::DSL::ModelInstance>(
      this->device_with_swapchain);
  this->dsl_view_projection = std::make_shared<BKVK::DSL::ViewProjection>(
      this->device_with_swapchain);
}

void Engine::unload_vk_descriptor_set_layouts()
{
  this->dsl_model_instance = nullptr;
  this->dsl_view_projection = nullptr;
}

void Engine::load_vk_graphic_pipeline_layout()
{
  this->graphic_pipeline_layout =
      std::make_shared<BKVK::GraphicPipelineLayout>(
          this->dsl_model_instance, this->dsl_view_projection);
}

void Engine::unload_vk_graphic_pipeline_layout()
{
  this->graphic_pipeline_layout = nullptr;
}

void Engine::load_vk_graphic_pipelines()
{
   this->graphic_pipeline = std::make_shared<BKVK::GraphicPipeline>(
       this->swapchain, this->graphic_pipeline_layout);
}

void Engine::unload_vk_graphic_pipelines()
{
  this->graphic_pipeline = nullptr;
}

void Engine::load_vk_frame_sync()
{
  this->vk_image_available_semaphores.resize(this->max_frames_in_flight);
  this->vk_render_finished_semaphores.resize(this->max_frames_in_flight);
  this->vk_in_flight_fences.resize(this->max_frames_in_flight);

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_info.pNext = nullptr;
  semaphore_info.flags = 0;

  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.pNext = nullptr;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for(int i = 0; i < this->max_frames_in_flight; i++)
  {
    if(vkCreateSemaphore(
           this->device_with_swapchain->get_vk_device(), &semaphore_info,
           nullptr, &this->vk_image_available_semaphores[i]) != VK_SUCCESS ||
       vkCreateSemaphore(
           this->device_with_swapchain->get_vk_device(), &semaphore_info,
           nullptr, &this->vk_render_finished_semaphores[i]) != VK_SUCCESS ||
       vkCreateFence(this->device_with_swapchain->get_vk_device(), &fence_info,
                     nullptr, &this->vk_in_flight_fences[i]) != VK_SUCCESS)
      throw Loader::Error{"Failed to create semaphores."};
  }
}

void Engine::unload_vk_frame_sync()
{
  vkDeviceWaitIdle(this->device_with_swapchain->get_vk_device());

  for(int i = 0; i < this->max_frames_in_flight; i++)
  {
    vkDestroySemaphore(this->device_with_swapchain->get_vk_device(),
                       this->vk_render_finished_semaphores[i], nullptr);
    vkDestroySemaphore(this->device_with_swapchain->get_vk_device(),
                       this->vk_image_available_semaphores[i], nullptr);
    vkDestroyFence(this->device_with_swapchain->get_vk_device(),
                   this->vk_in_flight_fences[i], nullptr);
  }
}

void Engine::load_vk_draw_command_pool()
{
  // FIXME: 3 is a magical number, triple buffering.
  this->draw_command_pool = std::make_unique<BKVK::CommandPool>(
      this->queues_families_with_presentation[0], 3);
}

void Engine::unload_vk_draw_command_pool()
{
  this->draw_command_pool = nullptr;
}

glm::mat4 Engine::update_ub_model_instance(
    uint32_t image_index, VALUE entity3d, bk_model_data* model_data)
{
  glm::vec3 entity_position = bk_cVector3D_get_data(
      rb_ivar_get(entity3d, id_at_position))->vec;
  glm::vec3 entity_rotation = bk_cVector3D_get_data(
      rb_ivar_get(entity3d, id_at_rotation))->vec;

  // Object matrix.
  glm::mat4 model{1.0f};
  model = glm::rotate(
      model, glm::radians(entity_rotation.x), glm::vec3{1.0, 0.0, 0.0});
  model = glm::rotate(
      model, glm::radians(entity_rotation.y), glm::vec3{0.0, 1.0, 0.0});
  model = glm::rotate(
      model, glm::radians(entity_rotation.z), glm::vec3{0.0, 0.0, 1.0});
  model = glm::translate(
      model, entity_position);

  return model;
}

void Engine::render(
    VALUE camera,
    const std::unordered_map<VALUE, std::vector<VALUE>> &model_entities)
{
  vkWaitForFences(this->devices[0]->get_vk_device(), 1,
                  &this->vk_in_flight_fences[this->current_frame], VK_TRUE,
                  std::numeric_limits<uint64_t>::max());
  vkResetFences(this->devices[0]->get_vk_device(), 1,
                &this->vk_in_flight_fences[this->current_frame]);

  uint32_t image_index;
  vkAcquireNextImageKHR(
      this->devices[0]->get_vk_device(), this->swapchain->get_vk_swapchain(),
      std::numeric_limits<uint64_t>::max(),
      this->vk_image_available_semaphores[this->current_frame],
      VK_NULL_HANDLE, &image_index);

  auto vk_command_buffer =
      this->draw_command_pool->get_vk_command_buffers()[image_index];

  // Load command.
  {
    vkResetCommandBuffer(vk_command_buffer, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT |
                       VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(vk_command_buffer, &begin_info) != VK_SUCCESS)
    {
      throw ErrRender{"Failed to beggin draw command buffer."};
    }

    // Dark gray blue.
    VkClearValue clear_color{0.12f, 0.12f, 0.18f, 1.0f};

    VkRenderPassBeginInfo render_pass_begin{};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.pNext = nullptr;
    render_pass_begin.renderPass =
        this->graphic_pipeline->get_vk_render_pass();
    render_pass_begin.framebuffer =
        this->graphic_pipeline->get_swapchain_framebuffers()[image_index];
    render_pass_begin.renderArea.offset = {0, 0};
    render_pass_begin.renderArea.extent = {
      this->core_data->screen_width, this->core_data->screen_height};
    render_pass_begin.clearValueCount = 1;
    render_pass_begin.pClearValues = &clear_color;

    vkCmdBeginRenderPass(
        vk_command_buffer, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport vk_viewport{};
    vk_viewport.width = static_cast<float>(this->core_data->screen_width);
    vk_viewport.height = static_cast<float>(this->core_data->screen_height);
    vk_viewport.minDepth = 0.0f;
    vk_viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vk_command_buffer, 0, 1, &vk_viewport);

    VkRect2D vk_scissor{};
    vk_scissor.extent.width = this->core_data->screen_width;
    vk_scissor.extent.height = this->core_data->screen_height;
    vk_scissor.offset.x = 0;
    vk_scissor.offset.y = 0;
    vkCmdSetScissor(vk_command_buffer, 0, 1, &vk_scissor);

    for(const auto& [model, entities]: model_entities)
    {
      bk_model_data* model_data = bk_cModel_get_data(model);

      model_data->draw(
          vk_command_buffer,
          image_index,
          entities.size(),
          this->graphic_pipeline->get_vk_graphic_pipeline(),
          this->graphic_pipeline->get_ds_view_projection()->
          get_vk_descriptor_sets()[this->current_frame],
          this->graphic_pipeline->get_graphic_pipeline_layout()->
          get_vk_pipeline_layout());
    }

    vkCmdEndRenderPass(vk_command_buffer);

    if(vkEndCommandBuffer(vk_command_buffer) != VK_SUCCESS)
    {
      throw ErrRender{"Failed to end draw command buffer."};
    }
  }

  // Update uniform buffers
  {
    glm::vec3 camera_position = bk_cVector3D_get_data(
        rb_ivar_get(camera, id_at_position))->vec;
    glm::vec3 camera_rotation = bk_cVector3D_get_data(
        rb_ivar_get(camera, id_at_rotation))->vec;

    BKVK::UBOViewProjection ubo_view_projection{};
    // View matrix.
    ubo_view_projection.view = glm::mat4{1.0f};
    ubo_view_projection.view = glm::translate(
        ubo_view_projection.view, camera_position);
    ubo_view_projection.view = glm::rotate(
        ubo_view_projection.view, glm::radians(camera_rotation.x),
        glm::vec3{1.0, 0.0, 0.0});
    ubo_view_projection.view = glm::rotate(
        ubo_view_projection.view, glm::radians(camera_rotation.y),
        glm::vec3{0.0, 1.0, 0.0});
    ubo_view_projection.view = glm::rotate(
        ubo_view_projection.view, glm::radians(camera_rotation.z),
        glm::vec3{0.0, 0.0, 1.0});
    ubo_view_projection.view = glm::inverse(ubo_view_projection.view);

    // Projection matrix.
    ubo_view_projection.proj = glm::perspective(
        glm::radians(45.0f),
        core_data->screen_width /
        static_cast<float>(core_data->screen_height),
        0.1f, 10.0f);
    ubo_view_projection.proj[1][1] *= -1;

    this->graphic_pipeline->get_ub_view_projection()[image_index]->
        copy_data(&ubo_view_projection);

    for(const auto& [model, entities3d]: model_entities)
    {
      BKVK::UBOModelInstance ubo_model_instance;
      int entity_index{0};
      bk_model_data* model_data = bk_cModel_get_data(model);

      for(const auto &entity3d: entities3d)
      {
        ubo_model_instance.model[entity_index] = update_ub_model_instance(
            image_index, entity3d, model_data);

        entity_index++;
      }

      model_data->ub_model_instance[image_index]->
          copy_data(&ubo_model_instance);
    }
  }

  // Submit drawing command.
  {
    auto queue =
        this->draw_command_pool->get_queue_family()->get_queue();

    VkSemaphore wait_semaphores[]{
      this->vk_image_available_semaphores[this->current_frame]};
    VkPipelineStageFlags wait_stages[] =
      {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signal_semaphores[]{
      this->vk_render_finished_semaphores[this->current_frame]};

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if(vkQueueSubmit(
           queue->get_vk_queue(), 1, &submit_info,
           this->vk_in_flight_fences[this->current_frame]) != VK_SUCCESS)
    {
      throw ErrRender{"Failed to submit draw command buffer."};
    }

    VkSwapchainKHR swap_chains[]{this->swapchain->get_vk_swapchain()};

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    vkQueuePresentKHR(queue->get_vk_queue(), &present_info);

    current_frame = (current_frame + 1) % this->max_frames_in_flight;
  }
}

Engine *engine{nullptr};

}

VALUE
bk_mEngine_load_core(VALUE self)
{
  id_at_current_camera = rb_intern("@current_camera");
  id_at_model = rb_intern("@model");
  id_at_position = rb_intern("@position");
  id_at_rotation = rb_intern("@rotation");

  if(BKGE::engine == nullptr)
  {
    try
    {
      BKGE::engine = new BKGE::Engine();
      engine_inilialized = SDL_TRUE;
    }
    catch(Loader::Error le)
    {
      std::string base_error{"Failed to inilialize BlueKitty → "};
      base_error += le.message;
      rb_raise(bk_eError, "%s", base_error.c_str());
    }
  }

  return self;
}

VALUE
bk_mEngine_loop_stage(VALUE self, VALUE current_stage)
{
  SDL_Event event;

  const ID id_at_at_quit_stage = rb_intern("@@quit_stage");
  const ID id_at_controller = rb_intern("@controller");
  const ID id_at_entities3d = rb_intern("@entities3d");
  const ID id_at_input_device = rb_intern("@input_device");
  const ID id_call_command = rb_intern("call_command");
  const ID id_tick = rb_intern("tick");

  VALUE current_camera = rb_ivar_get(current_stage, id_at_current_camera);
  VALUE controller = rb_ivar_get(current_stage, id_at_controller);
  VALUE entities3d = rb_ivar_get(current_stage, id_at_entities3d);
  VALUE input_device = rb_ivar_get(current_stage, id_at_input_device);
  VALUE sym_quit_game = ID2SYM(rb_intern("quit_game"));

  unsigned long frame_start = 0;
  int frame_stop = 0;
  VALUE frame_last_duration = rb_float_new(0.0);

  while(TYPE(rb_ivar_get(self, id_at_at_quit_stage)) == T_FALSE)
  {
    // The key is the model and the value is a vector of entities.
    // Several entities can have the same model. This map group entities based
    // on their models so they can all be rendered as instance with only one
    // call to VkCmdDraw[Indexed][Indirect].
    std::unordered_map<VALUE, std::vector<VALUE>> model_entities;

    // Initial frame ticks.
    frame_start = SDL_GetTicks();

    // Get input
    while(SDL_PollEvent(&event) != 0)
    {
      if(event.type == SDL_KEYDOWN)
        rb_funcall(controller, id_call_command, 1,
                   bk_Event_cInputInterface_get_keydown(
                       input_device, INT2NUM(event.key.keysym.sym)));

      else if(event.type == SDL_KEYUP)
        rb_funcall(controller, id_call_command, 1,
                   bk_Event_cInputInterface_get_keyup(
                       input_device, INT2NUM(event.key.keysym.sym)));

      else if(event.type == SDL_QUIT)
        rb_funcall(controller, id_call_command, 1, sym_quit_game);
    }

    rb_funcall(current_stage, id_tick, 1, frame_last_duration);

    // Default entities operations.
    {
      int ary_len = RARRAY_LEN(entities3d);
      for(int i{0}; i < ary_len; i++)
      {
        VALUE entity3d = rb_ary_entry(entities3d, i);

        // Group entities by models.
        VALUE model = rb_ivar_get(entity3d, id_at_model);
        model_entities[model].push_back(entity3d);

        // TODO: tick entities.
      }
    }

    BKGE::engine->render(current_camera, model_entities);

    // Control frame speed.
    // SDL_GetTicks return time im miliseconds, so I need to divide by 1000.
    frame_stop = SDL_GetTicks() - frame_start;
    rb_gc_mark(frame_last_duration);

    // If frame take less time than maximum allowed.
    if(BKGE::engine->get_max_frame_duration() > frame_stop)
    {
      SDL_Delay(BKGE::engine->get_max_frame_duration() - frame_stop);
      frame_last_duration =
          rb_float_new(BKGE::engine->get_max_frame_duration()/1000.0);
    }
    // If frame take too long time.
    else
    {
      frame_last_duration = rb_float_new((double)frame_stop/1000.0);
    }
  }

  return self;
}

VALUE
bk_mEngine_unload_core(VALUE self)
{
  if(BKGE::engine != nullptr)
  {
    delete BKGE::engine;
    engine_inilialized = SDL_FALSE;
  }

  return self;
}
