// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_UNIFORM_BUFFER_HPP
#define BLUE_KITTY_VK_UNIFORM_BUFFER_HPP 1

#include <memory>

#include <glm/glm.hpp>

#include "vk_base_buffer.hpp"

namespace BKVK
{
struct UBOModelInstance
{
  glm::mat4 model[128];
};

struct UBOViewProjection
{
  glm::mat4 view;
  glm::mat4 proj;
};

class UniformBuffer: public BaseBuffer
{
  friend class Loader::Stack<UniformBuffer>;

 public:
  UniformBuffer(std::shared_ptr<Device> device, VkDeviceSize data_size);
  ~UniformBuffer();

  void copy_data(void* ubo);

 private:
  Loader::Stack<UniformBuffer> initializer;
};
}

#endif /* BLUE_KITTY_VK_UNIFORM_BUFFER_HPP */
