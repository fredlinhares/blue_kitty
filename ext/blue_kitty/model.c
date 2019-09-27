// SPDX-License-Identifier: MIT
#include "model.h"

void
Init_blue_kitty_model(void)
{
  bk_cModel = rb_define_class_under(bk_m, "Model", rb_cData);
  rb_define_alloc_func(bk_cModel, bk_alloc_model);

  // If I call 'rb_define_method' from C++ it won't compile. So I call in a
  // different file.
  rb_define_method(bk_cModel, "initialize", bk_cModel_initialize, 2);
}
