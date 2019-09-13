// SPDX-License-Identifier: MIT
#include "vk_device.hpp"

#include <fstream>
#include <sstream>

#include "ruby.h"

#include "log.hpp"

namespace BKVK
{
Device::Device(const std::shared_ptr<Instance> &instance,
               const VkPhysicalDevice &vk_physical_device,
               const std::vector<VkQueueFamilyProperties>
               &queue_family_properties,
               bool with_swapchain):
    instance{instance},
    vk_physical_device{vk_physical_device},
    loader{this},
    with_swapchain{with_swapchain}
{
  // Get information from physical device.
  VkPhysicalDeviceProperties physical_properties = {};
  vkGetPhysicalDeviceProperties(vk_physical_device, &physical_properties);
  VkPhysicalDeviceFeatures supported_features = {};
  vkGetPhysicalDeviceFeatures(vk_physical_device, &supported_features);

  // Display physical device information.
  if(this->instance->get_core_data()->debug)
  {
    std::ostringstream txt;

    txt << "Name: " << physical_properties.deviceName << std::endl;
    txt << "API version: " << physical_properties.apiVersion << std::endl;
    txt << "Driver version: " << physical_properties.driverVersion <<
        std::endl;
    txt << "Vendor ID: " << physical_properties.vendorID << std::endl;
    txt << "Device ID: " << physical_properties.deviceID << std::endl;
    txt << "Device type: " << physical_properties.deviceType << std::endl;

    BKGE::Log::standard(txt.str());
  }

  std::vector<std::vector<float>> queue_priorities{
    queue_family_properties.size()};

  // Information for queue criation.
  std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
  device_queue_create_infos.resize(queue_family_properties.size());
  for(uint32_t i = 0; i < queue_family_properties.size(); i++)
  {
    // Give different priorities for queues.
    int queue_count = queue_family_properties[i].queueCount;
    float priority_unity = 1.0f/queue_count;
    for(int ii = 0; ii < queue_count; ii++)
      queue_priorities[i].push_back(priority_unity * (queue_count - ii));

    device_queue_create_infos[i].sType =
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_infos[i].pNext = nullptr;
    device_queue_create_infos[i].flags = 0;
    device_queue_create_infos[i].queueFamilyIndex = i;
    device_queue_create_infos[i].queueCount = queue_count;
    device_queue_create_infos[i].pQueuePriorities = queue_priorities[i].data();
  }

  VkPhysicalDeviceFeatures required_features = {};

  // Optional
  required_features.multiDrawIndirect = supported_features.multiDrawIndirect;

  // Required
  required_features.geometryShader = VK_TRUE;
  required_features.tessellationShader = VK_TRUE;
  required_features.samplerAnisotropy = VK_TRUE;

  std::vector<const char*> device_extensions;
  if(with_swapchain)
    device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueCreateInfoCount = device_queue_create_infos.size();
  device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
  device_create_info.enabledLayerCount = 0;
  device_create_info.ppEnabledLayerNames = nullptr;
  device_create_info.enabledExtensionCount = device_extensions.size();
  if(device_extensions.size() == 0)
    device_create_info.ppEnabledExtensionNames = nullptr;
  else    
    device_create_info.ppEnabledExtensionNames = device_extensions.data();

  device_create_info.pEnabledFeatures = &required_features;

  if(vkCreateDevice(this->vk_physical_device, &device_create_info, nullptr,
                    &this->vk_device) != VK_SUCCESS)
    throw Loader::Error{"Vulkan device could not be created."};

  this->loader.add(&Device::load_vk_shaders, &Device::unload_vk_shaders);

  this->loader.load();
}

Device::~Device()
{
  this->loader.unload();

  vkDeviceWaitIdle(this->vk_device);
  vkDestroyDevice(this->vk_device, nullptr);
}

void Device::load_vk_shaders()
{
  const ID id_gem = rb_intern("Gem");
  const ID id_datadir = rb_intern("datadir");

  VALUE gem_class;
  VALUE blue_kitty_str;
  VALUE path;

  blue_kitty_str = rb_str_new2("blue_kitty");
  gem_class = rb_const_get(rb_cObject, id_gem);

  path = rb_funcall(gem_class, id_datadir, 1, blue_kitty_str);

  char *datadir = StringValueCStr(path);

  std::string vert_path = datadir;
  vert_path += "/GLSL/vert.spv";
  std::string frag_path = datadir;
  frag_path += "/GLSL/frag.spv";

  this->vk_vert_shader_module = create_shader_module(vert_path);
  this->vk_frag_shader_module = create_shader_module(frag_path);

  rb_gc_mark(blue_kitty_str);
  rb_gc_mark(path);
}

void Device::unload_vk_shaders()
{
  vkDestroyShaderModule(this->vk_device, this->vk_vert_shader_module, nullptr);
  vkDestroyShaderModule(this->vk_device, this->vk_frag_shader_module, nullptr);
}

VkShaderModule Device::create_shader_module(
    const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
  {
    throw Loader::Error{"Failed to open shader module file."};
  }

  size_t file_size = (size_t) file.tellg();
  std::vector<char> code(file_size);

  file.seekg(0);
  file.read(code.data(), file_size);

  file.close();

  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(this->vk_device, &create_info, nullptr,
                           &shader_module) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create shader module.");
  }

  return shader_module;
}

uint32_t Device::select_memory_type(
    VkMemoryRequirements vk_memory_requirements,
    VkMemoryPropertyFlags vk_property_flags)
{
  VkPhysicalDeviceMemoryProperties vk_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(this->vk_physical_device,
                                      &vk_memory_properties);

  for (uint32_t memory_type = 0;
       memory_type < vk_memory_properties.memoryTypeCount; memory_type++)
  {
    if (vk_memory_requirements.memoryTypeBits & (1 << memory_type))
    {
      const VkMemoryType& type = vk_memory_properties.memoryTypes[memory_type];

      if ((type.propertyFlags & vk_property_flags) == vk_property_flags)
        return memory_type;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type.");
}

}
