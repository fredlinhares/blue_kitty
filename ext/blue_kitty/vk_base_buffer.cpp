// SPDX-License-Identifier: MIT
#include "vk_base_buffer.hpp"

namespace BKVK
{
void BaseBuffer::load_buffer()
{
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.pNext = nullptr;
  buffer_info.flags = 0;
  buffer_info.size = this->vk_device_size;
  buffer_info.usage = this->vk_buffer_usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buffer_info.queueFamilyIndexCount = 0;
  buffer_info.pQueueFamilyIndices = nullptr;

  if(vkCreateBuffer(this->device->get_vk_device(), &buffer_info, nullptr,
                    &this->vk_buffer) != VK_SUCCESS)
    throw Loader::Error{"Failed to create vertex buffer."};
}

void BaseBuffer::unload_buffer()
{
  vkDestroyBuffer(this->device->get_vk_device(), this->vk_buffer, nullptr);
}

void BaseBuffer::load_memory()
{
  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(this->device->get_vk_device(),
                                this->vk_buffer, &memory_requirements);

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(this->device->get_vk_physical_device(),
                                      &memory_properties);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = nullptr;
  alloc_info.allocationSize = memory_requirements.size;
  try
  {
    alloc_info.memoryTypeIndex = device->select_memory_type(
        memory_requirements, this->vk_memory_properties);
  }
  catch(std::runtime_error error)
  {
    throw Loader::Error{error.what()};
  }

  if(vkAllocateMemory(this->device->get_vk_device(), &alloc_info, nullptr,
                      &this->vk_device_memory) != VK_SUCCESS)
    throw Loader::Error{"Could not allocate memory for Vulkan vertex buffer."};

  vkBindBufferMemory(this->device->get_vk_device(), this->vk_buffer,
                     this->vk_device_memory, 0);
}

void BaseBuffer::unload_memory()
{
  vkFreeMemory(this->device->get_vk_device(), this->vk_device_memory, nullptr);
}
}
