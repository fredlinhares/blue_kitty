// SPDX-License-Identifier: MIT
#include "main.h"

#include "engine.h"
#include "error.h"
#include "input_device.h"
#include "keycode.h"
#include "model.h"
#include "vector3d.h"

VALUE bk_m;

/*
 * Document-module: BlueKitty
 *
 * This module exist to prevent name clashes.
 * @author Frederico Linhares
 */
void
Init_blue_kitty(void)
{
  bk_m = rb_define_module("BlueKitty");

  Init_blue_kitty_error();
  Init_blue_kitty_vector3d();
  Init_blue_kitty_keycode();
  Init_blue_kitty_input_device();
  Init_blue_kitty_engine();
  Init_blue_kitty_model();
}
