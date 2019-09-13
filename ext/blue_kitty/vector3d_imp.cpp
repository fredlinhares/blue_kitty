// SPDX-License-Identifier: MIT
#include "vector3d.h"
#include "vector3d_imp.hpp"

#include <glm/gtc/matrix_transform.hpp>

VALUE bk_cVector3D;

void
bk_free_vector3d(void* obj)
{
  struct bk_vector3d_data *ptr;
  ptr = static_cast<bk_vector3d_data*>(obj);
  delete ptr;
}

size_t
bk_memsize_vector3d(const void* obj)
{
  // TODO
  return 0;
}

static const rb_data_type_t
bk_vector3d_type = {
    "blue_kitty_vector3d",
    {0, bk_free_vector3d, bk_memsize_vector3d,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE
bk_alloc_vector3d(VALUE klass)
{
  VALUE obj;
  struct bk_vector3d_data *ptr;

  ptr = new bk_vector3d_data{};
  obj = TypedData_Wrap_Struct(klass, &bk_vector3d_type, ptr);

  return obj;
}

VALUE
bk_cVector3D_initialize(VALUE self, VALUE x, VALUE y, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(y);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x = NUM2DBL(x);
  ptr->vec.y = NUM2DBL(y);
  ptr->vec.z = NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_set_x(VALUE self, VALUE x)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x = NUM2DBL(x);

  return self;
}

VALUE
bk_cVector3D_set_y(VALUE self, VALUE y)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(y);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.y = NUM2DBL(y);

  return self;
}

VALUE
bk_cVector3D_set_z(VALUE self, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.z = NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_set_xy(VALUE self, VALUE x, VALUE y)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(y);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x = NUM2DBL(x);
  ptr->vec.y = NUM2DBL(y);

  return self;
}

VALUE
bk_cVector3D_set_yz(VALUE self, VALUE y, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(y);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.y = NUM2DBL(y);
  ptr->vec.z = NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_set_xz(VALUE self, VALUE x, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x = NUM2DBL(x);
  ptr->vec.z = NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_set_xyz(VALUE self, VALUE x, VALUE y, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(y);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x = NUM2DBL(x);
  ptr->vec.y = NUM2DBL(y);
  ptr->vec.z = NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_get_x(VALUE self)
{
  struct bk_vector3d_data *ptr;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  return rb_float_new(ptr->vec.x);
}

VALUE
bk_cVector3D_get_y(VALUE self)
{
  struct bk_vector3d_data *ptr;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  return rb_float_new(ptr->vec.y);
}

VALUE
bk_cVector3D_get_z(VALUE self)
{
  struct bk_vector3d_data *ptr;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  return rb_float_new(ptr->vec.z);
}

VALUE
bk_cVector3D_get_xy(VALUE self)
{
  struct bk_vector3d_data *ptr;
  VALUE vec;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  vec = rb_ary_new();
  rb_ary_push(vec, rb_float_new(ptr->vec.x));
  rb_ary_push(vec, rb_float_new(ptr->vec.y));

  return vec;
}

VALUE
bk_cVector3D_get_yz(VALUE self)
{
  struct bk_vector3d_data *ptr;
  VALUE vec;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  vec = rb_ary_new();
  rb_ary_push(vec, rb_float_new(ptr->vec.y));
  rb_ary_push(vec, rb_float_new(ptr->vec.z));

  return vec;
}

VALUE
bk_cVector3D_get_xz(VALUE self)
{
  struct bk_vector3d_data *ptr;
  VALUE vec;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  vec = rb_ary_new();
  rb_ary_push(vec, rb_float_new(ptr->vec.x));
  rb_ary_push(vec, rb_float_new(ptr->vec.z));

  return vec;
}

VALUE
bk_cVector3D_get_xyz(VALUE self)
{
  struct bk_vector3d_data *ptr;
  VALUE vec;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  vec = rb_ary_new();
  rb_ary_push(vec, rb_float_new(ptr->vec.x));
  rb_ary_push(vec, rb_float_new(ptr->vec.y));
  rb_ary_push(vec, rb_float_new(ptr->vec.z));

  return vec;
}

VALUE
bk_cVector3D_update_x(VALUE self, VALUE x)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x += NUM2DBL(x);

  return self;
}

VALUE
bk_cVector3D_update_y(VALUE self, VALUE y)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(y);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.y += NUM2DBL(y);

  return self;
}

VALUE
bk_cVector3D_update_z(VALUE self, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.z += NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_update_xy(VALUE self, VALUE x, VALUE y)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(y);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x += NUM2DBL(x);
  ptr->vec.y += NUM2DBL(y);

  return self;
}

VALUE
bk_cVector3D_update_yz(VALUE self, VALUE y, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(y);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.y += NUM2DBL(y);
  ptr->vec.z += NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_update_xz(VALUE self, VALUE x, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x += NUM2DBL(x);
  ptr->vec.z += NUM2DBL(z);

  return self;
}

VALUE
bk_cVector3D_update_xyz(VALUE self, VALUE x, VALUE y, VALUE z)
{
  struct bk_vector3d_data *ptr;

  RB_FLOAT_TYPE_P(x);
  RB_FLOAT_TYPE_P(y);
  RB_FLOAT_TYPE_P(z);

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  ptr->vec.x += NUM2DBL(x);
  ptr->vec.y += NUM2DBL(y);
  ptr->vec.z += NUM2DBL(z);

  return self;
}

VALUE
vk_cVector3D_rotate(VALUE self, VALUE v3d)
{
  struct bk_vector3d_data *ptr;
  struct bk_vector3d_data *r;

  if(!rb_obj_is_kind_of(v3d, bk_cVector3D))
    rb_raise(rb_eArgError, "%s", "Rotate expect a Vector3D as argument.");

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);
  TypedData_Get_Struct(v3d, struct bk_vector3d_data, &bk_vector3d_type, r);

  glm::dmat4 m{1.0};
  m = glm::rotate(m, glm::radians(r->vec.x), glm::dvec3{1.0, 0.0, 0.0});
  m = glm::rotate(m, glm::radians(r->vec.y), glm::dvec3{0.0, 1.0, 0.0});
  m = glm::rotate(m, glm::radians(r->vec.z), glm::dvec3{0.0, 0.0, 1.0});

  glm::dvec4 result = m * glm::dvec4{ptr->vec.x, ptr->vec.y, ptr->vec.z, 1.0};

  ptr->vec.x = result.x;
  ptr->vec.y = result.y;
  ptr->vec.z = result.z;

  return self;
}

struct bk_vector3d_data*
bk_cVector3D_get_data(VALUE self)
{
  struct bk_vector3d_data *ptr;

  TypedData_Get_Struct(self, struct bk_vector3d_data, &bk_vector3d_type, ptr);

  return ptr;
}
