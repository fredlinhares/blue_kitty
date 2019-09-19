// SPDX-License-Identifier: MIT
#include "texture.h"
#include "texture_imp.hpp"

VALUE bk_cTexture;

/*
  Basic functions all Ruby classes need.
 */

void
bk_free_texture(void* obj)
{
  struct bk_texture_data *ptr;
  ptr = static_cast<bk_texture_data*>(obj);

  delete ptr;
}

size_t
bk_memsize_texture(const void* obj)
{
  // TODO
  return 0;
}

static const rb_data_type_t
bk_texture_type = {
  "blue_kitty_texture",
  {0, bk_free_texture, bk_memsize_texture,},
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE
bk_alloc_texture(VALUE klass)
{
  VALUE obj;
  struct bk_texture_data *ptr;

  ptr = new bk_texture_data{};
  obj = TypedData_Wrap_Struct(klass, &bk_texture_type, ptr);

  return obj;
}

/*
  External interface.
*/

VALUE
bk_cTexture_initialize(VALUE self, VALUE file_path)
{
  // TODO
  return self;
}

struct bk_texture_data*
bk_cTexture_get_data(VALUE self)
{
  struct bk_texture_data *ptr;

  TypedData_Get_Struct(self, struct bk_texture_data, &bk_texture_type, ptr);

  return ptr;
}
