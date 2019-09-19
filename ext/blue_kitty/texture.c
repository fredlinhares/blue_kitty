// SPDX-License-Identifier: MIT
#include "texture.h"

void
Init_blue_kitty_texture(void)
{
  bk_cTexture = rb_define_class_under(bk_m, "Texture", rb_cData);
  rb_define_alloc_func(bk_cTexture, bk_alloc_texture);

  // If I call 'rb_define_method' from C++ it won't compile. So I call in a
  // different file.
  rb_define_method(bk_cTexture, "initialize", bk_cTexture_initialize, 1);
}
