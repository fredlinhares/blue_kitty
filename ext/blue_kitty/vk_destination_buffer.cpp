// SPDX-License-Identifier: MIT
#include "vk_destination_buffer.hpp"

namespace BKVK
{
  DestinationBuffer::DestinationBuffer(
      const std::shared_ptr<QueueFamily> &queue_family,
      const std::shared_ptr<SourceBuffer> &source_buffer,
      VkBufferUsageFlags vk_buffer_usage):
      loader{this},
      queue_family{queue_family},
      source_buffer{source_buffer}
  {
    this->device = this->queue_family->get_device();
    this->vk_device_size = source_buffer->get_size();
    this->vk_buffer_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      vk_buffer_usage;
    this->vk_memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    this->loader.add(&DestinationBuffer::load_buffer,
      &DestinationBuffer::unload_buffer);
    this->loader.add(&DestinationBuffer::load_memory,
      &DestinationBuffer::unload_memory);
    this->loader.add(&DestinationBuffer::load_command,
      &DestinationBuffer::unload_command);

    try
    {
      this->loader.load();
    }
    catch(Loader::Error le)
    {
      throw Loader::Error{
        "Could not initialize Vulkan destination buffer: " + le.message};
    }
  }

  DestinationBuffer::~DestinationBuffer()
  {
    this->loader.unload();
  }

  void DestinationBuffer::load_command()
  {
    std::shared_ptr<CommandPool> command_pool;
    std::shared_ptr<Queue> transfer_queue;

    command_pool = std::make_shared<CommandPool>(this->queue_family, 1);
    transfer_queue = this->queue_family->get_queue();
    this->vk_command_buffer = command_pool->get_vk_command_buffers()[0];

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = this->vk_device_size;

    vkBeginCommandBuffer(this->vk_command_buffer, &begin_info);

    vkCmdCopyBuffer(
        this->vk_command_buffer, this->source_buffer->get_vk_buffer(),
        this->vk_buffer, 1, &copy_region);

    vkEndCommandBuffer(this->vk_command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &this->vk_command_buffer;

    vkQueueSubmit(transfer_queue->get_vk_queue(), 1, &submit_info,
                  VK_NULL_HANDLE);

    vkQueueWaitIdle(transfer_queue->get_vk_queue());
  }

  void DestinationBuffer::unload_command()
  {
  }

}
