// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_TEXTURE_H
#define BLUE_KITTY_TEXTURE_H 1

#include <vulkan/vulkan.h>

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern VALUE bk_cTexture;

VALUE
bk_alloc_texture(VALUE klass);

VALUE
bk_cTexture_initialize(VALUE self, VALUE file_path);

void
Init_blue_kitty_texture(void);

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_TEXTURE_H */
