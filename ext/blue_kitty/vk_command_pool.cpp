// SPDX-License-Identifier: MIT
#include "vk_command_pool.hpp"

#include "vk_device.hpp"

namespace BKVK
{

CommandPool::CommandPool(const std::shared_ptr<QueueFamily> &queue_family,
                         uint32_t buffers_quantity):
    loader{this}
{
  this->queue_family = queue_family;
  this->vk_command_buffers.resize(buffers_quantity);

  this->loader.add(&CommandPool::load_command_pool,
                   &CommandPool::unload_command_pool);
  this->loader.add(&CommandPool::load_command_buffers,
                   &CommandPool::unload_command_buffers);

  this->loader.load();
}

CommandPool::~CommandPool()
{
  this->loader.unload();
}

void CommandPool::load_command_pool()
{
  VkCommandPoolCreateInfo command_pool_create_info;

  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex =
      this->queue_family->get_family_index();

  if(vkCreateCommandPool(
         this->queue_family->get_device()->get_vk_device(),
         &command_pool_create_info, nullptr, &this->vk_command_pool)
     != VK_SUCCESS)
    throw Loader::Error{"Vulkan command pool could not be created."};
}

void CommandPool::unload_command_pool()
{
  vkDestroyCommandPool(this->queue_family->get_device()->get_vk_device(),
                       this->vk_command_pool, nullptr);
}

void CommandPool::load_command_buffers()
{
  VkCommandBufferAllocateInfo command_buffer_info;
  command_buffer_info.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_info.pNext = nullptr;
  command_buffer_info.commandPool = this->vk_command_pool;
  command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_info.commandBufferCount = this->vk_command_buffers.size();

  if(vkAllocateCommandBuffers(
         this->queue_family->get_device()->get_vk_device(),
         &command_buffer_info, this->vk_command_buffers.data()) != VK_SUCCESS)
    throw Loader::Error{"Vulkan command buffers could not be allocated."};
}

void CommandPool::unload_command_buffers()
{
  // Command buffers are destroyed automatically during command pool
  // destruction.
}
}
