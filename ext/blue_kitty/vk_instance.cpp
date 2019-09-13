// SPDX-License-Identifier: MIT
#include "vk_instance.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "log.hpp"
#include "main.h"

namespace BKVK
{

Instance::Instance(const std::shared_ptr<bk_sCoreData> &cd):
    loader{this},
    core_data{cd}
{
  this->loader.add(&Instance::load_vk_instance,
                   &Instance::unload_vk_instance);
  this->loader.add(&Instance::load_window_surface,
                   &Instance::unload_window_surface);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{"Could not initialize Vulkan Instance → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

Instance::~Instance()
{
  this->loader.unload();
}

void Instance::load_vk_instance()
{
  VkApplicationInfo app_info;
  VkInstanceCreateInfo create_info;

  uint32_t vk_extensions_count;
  std::vector<const char*> vk_extensions;
  std::vector<const char*> vk_required_extensions;

  uint32_t vk_sdl_extension_count;
  std::vector<const char*> vk_sdl_extensions;

  uint32_t vk_available_layers_count;
  std::vector<VkLayerProperties> vk_available_layers;
  std::vector<const char*> vk_available_layers_names;
  std::vector<const char*> vk_required_layers_names;

  // Load debuging layers.
  if(core_data->debug)
  {
    vk_required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    vk_required_layers_names.push_back("VK_LAYER_LUNARG_standard_validation");
  }

  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = this->core_data->game_name;
  app_info.applicationVersion = VK_MAKE_VERSION(
      this->core_data->game_vesion_major, this->core_data->game_vesion_minor,
      this->core_data->game_vesion_patch);
  app_info.pEngineName = "BlueKitty";
  app_info.engineVersion = VK_MAKE_VERSION(
      FIX2INT(rb_ivar_get(bk_m, rb_intern("@@VERSION_MAJOR"))),
      FIX2INT(rb_ivar_get(bk_m, rb_intern("@@VERSION_MINOR"))),
      FIX2INT(rb_ivar_get(bk_m, rb_intern("@@VERSION_PATCH"))));
  app_info.apiVersion = VK_API_VERSION_1_0;

  // Get extensions for SDL.
  if(!SDL_Vulkan_GetInstanceExtensions(
         this->core_data->window, &vk_sdl_extension_count, nullptr))
  {
    std::string error =
        "Vulkan extensions could not be loaded by SDL! SDL_Error: ";
    error += SDL_GetError();
    throw Loader::Error{error};
  }
  vk_sdl_extensions.resize(vk_sdl_extension_count);
  SDL_Vulkan_GetInstanceExtensions(
      this->core_data->window, &vk_sdl_extension_count,
      vk_sdl_extensions.data());

  // Combine all extensions.
  vk_extensions_count = vk_sdl_extension_count + vk_required_extensions.size();
  vk_extensions.resize(vk_extensions_count);
  for(uint32_t i = 0; i < vk_sdl_extension_count; i++)
    vk_extensions[i] = vk_sdl_extensions[i];
  for(uint32_t i = 0; i < vk_required_extensions.size(); i++)
    vk_extensions[i + vk_sdl_extension_count] = vk_required_extensions[i];

  if(this->core_data->debug)
  {
    BKGE::Log::header("Enabled VK extensions.");
    for(auto vk_extension: vk_extensions)
    {
      std::ostringstream txt;
      txt << "Extension name: " << vk_extension;
      BKGE::Log::standard(txt.str());
    }
  }

  // Get available instance layers.
  vkEnumerateInstanceLayerProperties(&vk_available_layers_count, nullptr);
  vk_available_layers.resize(vk_available_layers_count);
  vkEnumerateInstanceLayerProperties(&vk_available_layers_count,
                                     vk_available_layers.data());
  vk_available_layers_names.resize(vk_available_layers_count);
  if(this->core_data->debug)
    BKGE::Log::header("Available VK instance layers.");
  for(uint32_t i = 0; i < vk_available_layers_count; i++)
  {
    if(this->core_data->debug)
    {
      std::ostringstream txt;
      txt << "name: " << vk_available_layers[i].layerName << std::endl;
      txt << "Description: " << vk_available_layers[i].description <<
          std::endl;
      txt << "Spec version: " << vk_available_layers[i].specVersion <<
          std::endl;
      txt << "Implementation version: " <<
          vk_available_layers[i].implementationVersion << std::endl;
      BKGE::Log::standard(txt.str());
    }

    vk_available_layers_names[i] = vk_available_layers[i].layerName;
  }

  // If required layers are not all available.
  if(!std::includes(
         vk_available_layers_names.begin(), vk_available_layers_names.end(),
         vk_required_layers_names.begin(), vk_required_layers_names.begin()))
    throw Loader::Error{"Some required Vulkan layers are not available."};

  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = vk_extensions.size();
  create_info.ppEnabledExtensionNames = vk_extensions.data();
  create_info.enabledLayerCount = vk_required_layers_names.size();
  create_info.ppEnabledLayerNames = vk_required_layers_names.data();

  if(vkCreateInstance(&create_info, nullptr, &this->vk_instance) !=
     VK_SUCCESS)
    throw Loader::Error{"Failed to create Vulkan instance."};
}

void Instance::unload_vk_instance()
{
  vkDestroyInstance(this->vk_instance, nullptr);
}

void Instance::load_window_surface()
{
  if(!SDL_Vulkan_CreateSurface(this->core_data->window, this->vk_instance,
                               &this->surface))
  {
    std::string error = "Failed to create a window surface: ";
    error += SDL_GetError();
    throw Loader::Error{error};
  }
}

void Instance::unload_window_surface()
{
  vkDestroySurfaceKHR(this->vk_instance, this->surface, nullptr);
}

}
