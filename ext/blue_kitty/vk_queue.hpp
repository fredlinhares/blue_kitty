// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_QUEUE_HPP
#define BLUE_KITTY_VK_QUEUE_HPP 1

#include <memory>
#include <mutex>
#include <vector>

#include <vulkan/vulkan.h>

namespace BKVK
{
class QueueFamily;

enum class QueueState
{
  free,
  busy
};

/*
  This class handle the queue lifecycle so the classes using queue don't need
  to do it.
*/
class Queue
{
  friend class QueueFamily;

  Queue(const Queue &t) = delete;
  Queue& operator=(const Queue &t) = delete;
  Queue(const Queue &&t) = delete;
  Queue& operator=(const Queue &&t) = delete;

 public:
  ~Queue();

  inline VkQueue get_vk_queue() const { return this->vk_queue; };

  template<typename T>
  void submit_one_time_command(const VkCommandBuffer vk_command_buffer,
                               T commands);

 private:
  VkDevice vk_device;
  VkQueue vk_queue;
  QueueState *state;
  std::mutex *queue_request;

  explicit Queue(VkDevice vk_device, VkQueue vk_queue, QueueState *state,
                 std::mutex *queue_request);

};

template<typename T>
void Queue::submit_one_time_command(const VkCommandBuffer vk_command_buffer,
                                    T commands)
{
  VkCommandBufferBeginInfo buffer_begin_info{};
  buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(vk_command_buffer, &buffer_begin_info);

  commands();

  vkEndCommandBuffer(vk_command_buffer);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = nullptr;
  submit_info.waitSemaphoreCount = 0;
  submit_info.pWaitSemaphores = nullptr;
  submit_info.pWaitDstStageMask = nullptr;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &vk_command_buffer;
  submit_info.signalSemaphoreCount = 0;
  submit_info.pSignalSemaphores = nullptr;

  vkQueueSubmit(this->vk_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(this->vk_queue);
}

}

#endif /* BLUE_KITTY_VK_QUEUE_FAMILY_HPP */
