// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_INSTANCE_HPP
#define BLUE_KITTY_VK_INSTANCE_HPP 1

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "core_data.h"
#include "loader.hpp"

namespace BKVK
{
class Instance
{
  friend class Loader::Stack<Instance>;

  Instance(const Instance &t) = delete;
  Instance& operator=(const Instance &t) = delete;
  Instance(const Instance &&t) = delete;
  Instance& operator=(const Instance &&t) = delete;

 public:
  explicit Instance(const std::shared_ptr<bk_sCoreData> &cd);
  ~Instance();

  inline std::shared_ptr<bk_sCoreData> get_core_data() const
  { return this->core_data; }
  inline VkSurfaceKHR get_surface() const
  { return this->surface; };
  inline VkInstance get_vk_instance() const
  { return this->vk_instance; };

 private:
  Loader::Stack<Instance> loader;
  std::shared_ptr<bk_sCoreData> core_data;
  VkSurfaceKHR surface;
  VkInstance vk_instance;

  void load_vk_instance();
  void unload_vk_instance();

  void load_window_surface();
  void unload_window_surface();
};
}

#endif /* BLUE_KITTY_VK_INSTANCE_HPP */
