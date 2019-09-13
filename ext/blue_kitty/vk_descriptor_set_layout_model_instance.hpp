// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_MODEL_INSTANCE_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_MODEL_INSTANCE_HPP 1

#include "vk_descriptor_set_layout_base.hpp"

namespace BKVK::DSL // Descriptor set layout.
{
class ModelInstance: public Base
{
  ModelInstance(const ModelInstance &mi) = delete;
  ModelInstance& operator=(const ModelInstance &mi) = delete;
  ModelInstance(const ModelInstance &&mi) = delete;
  ModelInstance& operator=(const ModelInstance &&mi) = delete;

 public:
  explicit ModelInstance(const std::shared_ptr<Device> &device);
  ~ModelInstance();

};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_MODEL_INSTANCE_HPP */
