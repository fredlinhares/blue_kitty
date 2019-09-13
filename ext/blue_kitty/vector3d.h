// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_INPUT_VECTOR3D_H
#define BLUE_KITTY_INPUT_VECTOR3D_H 1

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern VALUE bk_cVector3D;

VALUE
bk_alloc_vector3d(VALUE klass);

VALUE
bk_cVector3D_initialize(VALUE self, VALUE x, VALUE y, VALUE z);

VALUE
bk_cVector3D_set_x(VALUE self, VALUE x);

VALUE
bk_cVector3D_set_y(VALUE self, VALUE y);

VALUE
bk_cVector3D_set_z(VALUE self, VALUE z);

VALUE
bk_cVector3D_set_xy(VALUE self, VALUE x, VALUE y);

VALUE
bk_cVector3D_set_yz(VALUE self, VALUE y, VALUE z);

VALUE
bk_cVector3D_set_xz(VALUE self, VALUE x, VALUE z);

VALUE
bk_cVector3D_set_xyz(VALUE self, VALUE x, VALUE y, VALUE z);

VALUE
bk_cVector3D_get_x(VALUE self);

VALUE
bk_cVector3D_get_y(VALUE self);

VALUE
bk_cVector3D_get_z(VALUE self);

VALUE
bk_cVector3D_get_xy(VALUE self);

VALUE
bk_cVector3D_get_yz(VALUE self);

VALUE
bk_cVector3D_get_xz(VALUE self);

VALUE
bk_cVector3D_get_xyz(VALUE self);

VALUE
bk_cVector3D_update_x(VALUE self, VALUE x);

VALUE
bk_cVector3D_update_y(VALUE self, VALUE y);

VALUE
bk_cVector3D_update_z(VALUE self, VALUE z);

VALUE
bk_cVector3D_update_xy(VALUE self, VALUE x, VALUE y);

VALUE
bk_cVector3D_update_yz(VALUE self, VALUE y, VALUE z);

VALUE
bk_cVector3D_update_xz(VALUE self, VALUE x, VALUE z);

VALUE
bk_cVector3D_update_xyz(VALUE self, VALUE x, VALUE y, VALUE z);

VALUE
vk_cVector3D_rotate(VALUE self, VALUE v3d);

void
Init_blue_kitty_vector3d(void);

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_INPUT_VECTOR3D_H */
