// SPDX-License-Identifier: MIT
#include "vector3d.h"

void
Init_blue_kitty_vector3d(void)
{
  bk_cVector3D = rb_define_class_under(bk_m, "Vector3D", rb_cData);
  rb_define_alloc_func(bk_cVector3D, bk_alloc_vector3d);

  // If I call 'rb_define_method' from C++ it won't compile. So I call in a
  // different file.
  rb_define_method(bk_cVector3D, "initialize", bk_cVector3D_initialize, 3);

  rb_define_method(bk_cVector3D, "x=", bk_cVector3D_set_x, 1);
  rb_define_method(bk_cVector3D, "r=", bk_cVector3D_set_x, 1);
  rb_define_method(bk_cVector3D, "x", bk_cVector3D_get_x, 0);
  rb_define_method(bk_cVector3D, "r", bk_cVector3D_get_x, 0);
  rb_define_method(bk_cVector3D, "update_x", bk_cVector3D_update_x, 1);
  rb_define_method(bk_cVector3D, "update_r", bk_cVector3D_update_x, 1);

  rb_define_method(bk_cVector3D, "y=", bk_cVector3D_set_y, 1);
  rb_define_method(bk_cVector3D, "g=", bk_cVector3D_set_y, 1);
  rb_define_method(bk_cVector3D, "y", bk_cVector3D_get_y, 0);
  rb_define_method(bk_cVector3D, "g", bk_cVector3D_get_y, 0);
  rb_define_method(bk_cVector3D, "update_y", bk_cVector3D_update_y, 1);
  rb_define_method(bk_cVector3D, "update_g", bk_cVector3D_update_y, 1);

  rb_define_method(bk_cVector3D, "z=", bk_cVector3D_set_z, 1);
  rb_define_method(bk_cVector3D, "b=", bk_cVector3D_set_z, 1);
  rb_define_method(bk_cVector3D, "z", bk_cVector3D_get_z, 0);
  rb_define_method(bk_cVector3D, "b", bk_cVector3D_get_z, 0);
  rb_define_method(bk_cVector3D, "update_z", bk_cVector3D_update_z, 1);
  rb_define_method(bk_cVector3D, "update_b", bk_cVector3D_update_z, 1);

  rb_define_method(bk_cVector3D, "set_xy", bk_cVector3D_set_xy, 2);
  rb_define_method(bk_cVector3D, "set_rg", bk_cVector3D_set_xy, 2);
  rb_define_method(bk_cVector3D, "xy", bk_cVector3D_get_xy, 0);
  rb_define_method(bk_cVector3D, "rg", bk_cVector3D_get_xy, 0);
  rb_define_method(bk_cVector3D, "update_xy", bk_cVector3D_update_xy, 2);
  rb_define_method(bk_cVector3D, "update_rg", bk_cVector3D_update_xy, 2);

  rb_define_method(bk_cVector3D, "set_yz", bk_cVector3D_set_yz, 2);
  rb_define_method(bk_cVector3D, "set_gb", bk_cVector3D_set_yz, 2);
  rb_define_method(bk_cVector3D, "yz", bk_cVector3D_get_yz, 0);
  rb_define_method(bk_cVector3D, "gb", bk_cVector3D_get_yz, 0);
  rb_define_method(bk_cVector3D, "update_yz", bk_cVector3D_update_yz, 2);
  rb_define_method(bk_cVector3D, "update_gb", bk_cVector3D_update_yz, 2);

  rb_define_method(bk_cVector3D, "set_xz", bk_cVector3D_set_xz, 2);
  rb_define_method(bk_cVector3D, "set_rb", bk_cVector3D_set_xz, 2);
  rb_define_method(bk_cVector3D, "xz", bk_cVector3D_get_xz, 0);
  rb_define_method(bk_cVector3D, "rb", bk_cVector3D_get_xz, 0);
  rb_define_method(bk_cVector3D, "update_xz", bk_cVector3D_update_xz, 2);
  rb_define_method(bk_cVector3D, "update_rb", bk_cVector3D_update_xz, 2);

  rb_define_method(bk_cVector3D, "set_xyz", bk_cVector3D_set_xyz, 3);
  rb_define_method(bk_cVector3D, "set_rgb", bk_cVector3D_set_xyz, 3);
  rb_define_method(bk_cVector3D, "xyz", bk_cVector3D_get_xyz, 0);
  rb_define_method(bk_cVector3D, "rgb", bk_cVector3D_get_xyz, 0);
  rb_define_method(bk_cVector3D, "update_xyz", bk_cVector3D_update_xyz, 3);
  rb_define_method(bk_cVector3D, "update_rgb", bk_cVector3D_update_xyz, 3);

  rb_define_method(bk_cVector3D, "rotate", vk_cVector3D_rotate, 1);
}
