// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_INPUT_VECTOR3D_IMP_HPP
#define BLUE_KITTY_INPUT_VECTOR3D_IMP_HPP 1

#include <glm/vec3.hpp>

struct bk_vector3d_data
{
  glm::dvec3 vec;
};

struct bk_vector3d_data*
bk_cVector3D_get_data(VALUE self);

#endif /* BLUE_KITTY_INPUT_VECTOR3D_IMP_HPP */
