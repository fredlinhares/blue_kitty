// SPDX-License-Identifier: MIT
#include "keycode.h"

#include <SDL2/SDL.h>

VALUE bk_mKeycode;

void
Init_blue_kitty_keycode(void)
{
  bk_mKeycode = rb_define_module_under(bk_m, "Keycode");

  rb_define_const(bk_mKeycode, "A", INT2NUM(SDLK_a));
  rb_define_const(bk_mKeycode, "B", INT2NUM(SDLK_b));
  rb_define_const(bk_mKeycode, "C", INT2NUM(SDLK_c));
  rb_define_const(bk_mKeycode, "D", INT2NUM(SDLK_d));
  rb_define_const(bk_mKeycode, "E", INT2NUM(SDLK_e));
  rb_define_const(bk_mKeycode, "F", INT2NUM(SDLK_f));
  rb_define_const(bk_mKeycode, "G", INT2NUM(SDLK_g));
  rb_define_const(bk_mKeycode, "H", INT2NUM(SDLK_h));
  rb_define_const(bk_mKeycode, "I", INT2NUM(SDLK_i));
  rb_define_const(bk_mKeycode, "J", INT2NUM(SDLK_j));
  rb_define_const(bk_mKeycode, "K", INT2NUM(SDLK_k));
  rb_define_const(bk_mKeycode, "L", INT2NUM(SDLK_l));
  rb_define_const(bk_mKeycode, "M", INT2NUM(SDLK_m));
  rb_define_const(bk_mKeycode, "N", INT2NUM(SDLK_n));
  rb_define_const(bk_mKeycode, "O", INT2NUM(SDLK_o));
  rb_define_const(bk_mKeycode, "P", INT2NUM(SDLK_p));
  rb_define_const(bk_mKeycode, "Q", INT2NUM(SDLK_q));
  rb_define_const(bk_mKeycode, "R", INT2NUM(SDLK_r));
  rb_define_const(bk_mKeycode, "S", INT2NUM(SDLK_s));
  rb_define_const(bk_mKeycode, "T", INT2NUM(SDLK_t));
  rb_define_const(bk_mKeycode, "U", INT2NUM(SDLK_u));
  rb_define_const(bk_mKeycode, "V", INT2NUM(SDLK_v));
  rb_define_const(bk_mKeycode, "W", INT2NUM(SDLK_w));
  rb_define_const(bk_mKeycode, "X", INT2NUM(SDLK_x));
  rb_define_const(bk_mKeycode, "Y", INT2NUM(SDLK_y));
  rb_define_const(bk_mKeycode, "Z", INT2NUM(SDLK_z));

  rb_define_const(bk_mKeycode, "KEY_UP", INT2NUM(SDLK_UP));
  rb_define_const(bk_mKeycode, "KEY_DOWN", INT2NUM(SDLK_DOWN));
  rb_define_const(bk_mKeycode, "KEY_LEFT", INT2NUM(SDLK_LEFT));
  rb_define_const(bk_mKeycode, "KEY_RIGHT", INT2NUM(SDLK_RIGHT));
}
