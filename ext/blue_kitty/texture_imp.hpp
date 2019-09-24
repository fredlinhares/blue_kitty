// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_TEXTURE_IMP_HPP
#define BLUE_KITTY_TEXTURE_IMP_HPP 1

#include "texture.h"

#include <memory>

#include <SDL2/SDL_image.h>

#include "vk_device.hpp"

struct bk_texture_data
{
  std::shared_ptr<BKVK::Device> device;
  VkImage vk_image;
  VkSampler vk_sampler;
  VkImageView vk_view;
  VkDeviceMemory vk_device_memory;
  uint32_t width, height;
  uint32_t mip_levels;
};

struct bk_texture_data*
bk_cTexture_get_data(VALUE self);

#endif /* BLUE_KITTY_TEXTURE_IMP_HPP */
