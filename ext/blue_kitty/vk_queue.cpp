// SPDX-License-Identifier: MIT
#include "vk_queue.hpp"

#include "loader.hpp"

namespace BKVK
{
Queue::Queue(VkDevice vk_device, VkQueue vk_queue, QueueState *state,
             std::mutex *queue_request):
    vk_device{vk_device},
    vk_queue{vk_queue},
    state{state},
    queue_request{queue_request}
{
  *this->state = QueueState::busy;
}

Queue::~Queue()
{
  std::unique_lock<std::mutex> lock{*this->queue_request};
  *this->state = QueueState::free;
}

}
