// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_TEXTURE_IMP_HPP
#define BLUE_KITTY_TEXTURE_IMP_HPP 1

#include "texture.h"

#include <memory>

#include <SDL2/SDL_image.h>

#include "vk_device.hpp"

// Keep texture data into a separated object so it can be shared with a model
// even after the Ruby object is destroyed.
struct bk_sTexture
{
  std::shared_ptr<BKVK::Device> device;
  VkImage vk_image;
  VkSampler vk_sampler;
  VkImageView vk_view;
  VkDeviceMemory vk_device_memory;
  uint32_t width, height;
  uint32_t mip_levels;

  ~bk_sTexture();
};

struct bk_texture_data
{
  std::shared_ptr<bk_sTexture> texture;
};

struct bk_texture_data*
bk_cTexture_get_data(VALUE self);

#endif /* BLUE_KITTY_TEXTURE_IMP_HPP */
