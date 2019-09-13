// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_MODEL_INSTANCE_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_MODEL_INSTANCE_HPP 1

#include "loader.hpp"
#include "vk_descriptor_set_base.hpp"

namespace BKVK::DS // Descriptor set.
{
class ModelInstance: public Base
{
  friend class Loader::Stack<ModelInstance>;

  ModelInstance(const ModelInstance &mi) = delete;
  ModelInstance& operator=(const ModelInstance &mi) = delete;
  ModelInstance(const ModelInstance &&mi) = delete;
  ModelInstance& operator=(const ModelInstance &&mi) = delete;

 public:
  explicit ModelInstance(
      const std::shared_ptr<DSL::Base> &layout,
      const std::vector<std::shared_ptr<UniformBuffer>> &uniform_buffers);
  ~ModelInstance();

 private:
  Loader::Stack<ModelInstance> loader;

  void load_buffers();
  void unload_buffers();
};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_MODEL_INSTANCE_HPP */
