// SPDX-License-Identifier: MIT
#include "vk_uniform_buffer.hpp"

namespace BKVK
{
UniformBuffer::UniformBuffer(std::shared_ptr<Device> device,
                             VkDeviceSize data_size):
    initializer{this}
{
  this->device = device;
  this->vk_device_size = data_size;
  this->vk_buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  this->vk_memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  this->initializer.add(&UniformBuffer::load_buffer,
                        &UniformBuffer::unload_buffer);
  this->initializer.add(&UniformBuffer::load_memory,
                        &UniformBuffer::unload_memory);

  try
  {
    this->initializer.load();
  }
  catch(Loader::Error le)
  {
    throw Loader::Error{"Could not initialize Vulkan uniform buffer → " +
          le.message};
  }
}

UniformBuffer::~UniformBuffer()
{
  this->initializer.unload();
}

void UniformBuffer::copy_data(void *ubo)
{
  void *data;
  vkMapMemory(this->device->get_vk_device(), this->vk_device_memory, 0,
              this->vk_device_size, 0, &data);
  memcpy(data, ubo, this->vk_device_size);
  vkUnmapMemory(this->device->get_vk_device(), this->vk_device_memory);
}
}
