// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_SOURCE_BUFFER_HPP
#define BLUE_KITTY_VK_SOURCE_BUFFER_HPP 1

#include "vk_base_buffer.hpp"
#include "vk_vertex.hpp"

namespace BKVK
{
class SourceBuffer: public BaseBuffer
{
  friend class Loader::Stack<SourceBuffer>;

  SourceBuffer(const SourceBuffer &t) = delete;
  SourceBuffer& operator=(const SourceBuffer &t) = delete;
  SourceBuffer(const SourceBuffer &&t) = delete;
  SourceBuffer& operator=(const SourceBuffer &&t) = delete;

 public:
  explicit SourceBuffer(const std::shared_ptr<Device> &device, void *data,
                        size_t data_size);
  ~SourceBuffer();

  void copy_data();

 private:
  Loader::Stack<SourceBuffer> loader;
  void *data;
};
}

#endif /* BLUE_KITTY_VK_SOURCE_BUFFER_HPP */
