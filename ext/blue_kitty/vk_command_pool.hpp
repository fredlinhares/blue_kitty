// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_COMMAND_POOL_HPP
#define BLUE_KITTY_VK_COMMAND_POOL_HPP 1

#include <memory>
#include <vector>

#include "loader.hpp"

#include "vk_queue_family.hpp"

namespace BKVK
{
/*
  This class handle the command buffer lifecycle so the classes using
  command buffers don't need to do it.
*/
class CommandPool
{
  friend class Loader::Stack<CommandPool>;

  CommandPool(const CommandPool &t) = delete;
  CommandPool& operator=(const CommandPool &t) = delete;
  CommandPool(const CommandPool &&t) = delete;
  CommandPool& operator=(const CommandPool &&t) = delete;

 public:
  explicit CommandPool(const std::shared_ptr<QueueFamily> &queue_family,
                       uint32_t buffers_quantity);
  ~CommandPool();

  inline std::shared_ptr<QueueFamily> get_queue_family() const
  { return this->queue_family; };
  inline std::vector<VkCommandBuffer> get_vk_command_buffers() const
  { return this->vk_command_buffers; };

 private:
  Loader::Stack<CommandPool> loader;

  std::shared_ptr<QueueFamily> queue_family;
  VkCommandPool vk_command_pool;
  std::vector<VkCommandBuffer> vk_command_buffers;

  void load_command_pool();
  void unload_command_pool();

  void load_command_buffers();
  void unload_command_buffers();

};
}

#endif /* BLUE_KITTY_VK_COMMAND_POOL_HPP */
