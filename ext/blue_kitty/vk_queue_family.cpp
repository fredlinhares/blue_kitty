// SPDX-License-Identifier: MIT
#include "vk_queue_family.hpp"

#include <random>
#include <sstream>
#include <stdexcept>

#include "log.hpp"
#include "vk_device.hpp"

namespace BKVK
{
QueueFamily::QueueFamily(const std::shared_ptr<BKVK::Device> &device,
                         uint32_t queue_family_index,
                         const VkQueueFamilyProperties &vk_family_properties):
    device{device},
    family_index{queue_family_index},
    vk_family_properties{vk_family_properties}
{
  if(this->device->get_instance()->get_core_data()->debug)
  {
    std::ostringstream txt;

    txt << "Queue quantity: " << vk_family_properties.queueCount << std::endl;
    txt << "Graphics: " <<
        (vk_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT ?
         "true" : "false") << std::endl;
    txt << "Compute: " <<
        (vk_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT ?
         "true" : "false") << std::endl;
    txt << "Transfer: " <<
        (vk_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT ?
         "true" : "false") << std::endl;
    txt << "Sparse Binding: " <<
        (vk_family_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ?
         "true" : "false") << std::endl;

    BKGE::Log::standard(txt.str());
  }

  this->vk_queues.resize(vk_family_properties.queueCount);
  for(size_t i{0}; i < this->vk_queues.size(); i++)
  {
    vkGetDeviceQueue(this->device->get_vk_device(), queue_family_index, i,
                     &this->vk_queues[i].first);
    if(this->vk_queues[i].first == VK_NULL_HANDLE)
      throw Loader::Error{"Failed to get Vulkan queue."};

    this->vk_queues[i].second = QueueState::free;
  }
}

int QueueFamily::num_properties() const
{
  int num = 0;

  if(vk_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) num++;
  if(vk_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) num++;
  if(vk_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) num++;
  if(vk_family_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) num++;

  return num;
}

std::unique_ptr<Queue> QueueFamily::get_queue()
{
  std::unique_lock<std::mutex> lock{this->queue_request};

  for(auto &vk_queue: this->vk_queues)
    if(vk_queue.second == QueueState::free)
    {
      std::unique_ptr<Queue> q{
        new Queue{
          this->device->get_vk_device(), vk_queue.first, &vk_queue.second,
          &this->queue_request}};
      return q;
    }

  // If all queues are busy...
  throw ErrNoFreeQueue{};
}

}
