// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_QUEUE_FAMILY_HPP
#define BLUE_KITTY_VK_QUEUE_FAMILY_HPP 1

#include <memory>
#include <mutex>
#include <vector>
#include <utility>

#include <vulkan/vulkan.h>

#include "loader.hpp"
#include "vk_device.hpp"
#include "vk_queue.hpp"

namespace BKVK
{
struct ErrNoFreeQueue
{
};

class QueueFamily
{
  QueueFamily(const QueueFamily &t) = delete;
  QueueFamily& operator=(const QueueFamily &t) = delete;
  QueueFamily(const QueueFamily &&t) = delete;
  QueueFamily& operator=(const QueueFamily &&t) = delete;

 public:
  explicit QueueFamily(const std::shared_ptr<BKVK::Device> &device,
              uint32_t queue_family_index,
              const VkQueueFamilyProperties &vk_family_properties);

  int num_properties() const;
  inline uint32_t get_family_index() const {return this->family_index;};
  inline std::shared_ptr<BKVK::Device> get_device() const
  { return this->device; };
  inline VkQueueFamilyProperties get_vk_family_properties() const
  { return this->vk_family_properties; };

  // Queues are limited resourses, if all queues are busy this function will
  // wait for one to be realesed before it returns.
  std::unique_ptr<Queue> get_queue();

 private:
  std::shared_ptr<Device> device;

  uint32_t family_index;
  VkQueueFamilyProperties vk_family_properties;
  std::vector<std::pair<VkQueue, QueueState>> vk_queues;

  std::mutex queue_request;

};
}

#endif /* BLUE_KITTY_VK_QUEUE_FAMILY_HPP */
