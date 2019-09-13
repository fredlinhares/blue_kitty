// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DESTINATION_BUFFER_HPP
#define BLUE_KITTY_VK_DESTINATION_BUFFER_HPP 1

#include "loader.hpp"
#include "vk_base_buffer.hpp"
#include "vk_command_pool.hpp"
#include "vk_queue.hpp"
#include "vk_source_buffer.hpp"

namespace BKVK
{
  class DestinationBuffer: public BaseBuffer
  {
    friend class Loader::Stack<DestinationBuffer>;

    DestinationBuffer(const DestinationBuffer &t) = delete;
    DestinationBuffer& operator=(const DestinationBuffer &t) = delete;
    DestinationBuffer(const DestinationBuffer &&t) = delete;
    DestinationBuffer& operator=(const DestinationBuffer &&t) = delete;

   public:
    explicit DestinationBuffer(
        const std::shared_ptr<QueueFamily> &queue_family,
        const std::shared_ptr<SourceBuffer> &source_buffer,
        VkBufferUsageFlags vk_buffer_usage);
    ~DestinationBuffer();

   private:
    Loader::Stack<DestinationBuffer> loader;

    std::shared_ptr<QueueFamily> queue_family;
    std::shared_ptr<SourceBuffer> source_buffer;
    VkCommandBuffer vk_command_buffer;

    void load_command();
    void unload_command();
  };
}

#endif /* BLUE_KITTY_VK_DESTINATION_BUFFER_HPP */
