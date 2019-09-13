// SPDX-License-Identifier: MIT
#include "vk_source_buffer.hpp"

namespace BKVK
{

SourceBuffer::SourceBuffer(const std::shared_ptr<Device> &device, void *data,
                           size_t data_size):
    loader{this},
    data{data}
{
  this->device = device;
  this->vk_device_size = data_size;
  this->vk_buffer_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  this->vk_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  this->loader.add(&SourceBuffer::load_buffer, &SourceBuffer::unload_buffer);
  this->loader.add(&SourceBuffer::load_memory, &SourceBuffer::unload_memory);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    throw Loader::Error{"Could not initialize Vulkan source buffer: " +
          le.message};
  }
  this->copy_data();
}

SourceBuffer::~SourceBuffer()
{
  this->loader.unload();
}

void SourceBuffer::copy_data()
{
  void *dst_data;
  vkMapMemory(this->device->get_vk_device(), this->vk_device_memory, 0,
              this->vk_device_size, 0, &dst_data);
  memcpy(dst_data, this->data, static_cast<size_t>(this->vk_device_size));
  vkUnmapMemory(this->device->get_vk_device(), this->vk_device_memory);
}

}
