// SPDX-License-Identifier: MIT
#include "input_device.h"

VALUE bk_cInputDevice;

static VALUE id_keydown_inputs;
static VALUE id_keyup_inputs;

VALUE
bk_Event_cInputInterface_add_keydown(VALUE self, VALUE keycode,
                                     VALUE command_name)
{
  FIXNUM_P(keycode);
  Check_Type(command_name, T_SYMBOL);

  rb_hash_aset(rb_ivar_get(self, id_keydown_inputs), keycode, command_name);

  return self;
}

VALUE
bk_Event_cInputInterface_add_keyup(VALUE self, VALUE keycode,
                                   VALUE command_name)
{
  FIXNUM_P(keycode);
  Check_Type(command_name, T_SYMBOL);

  rb_hash_aset(rb_ivar_get(self, id_keyup_inputs), keycode, command_name);

  return self;
}

VALUE
bk_Event_cInputInterface_initialize(VALUE self)
{
  rb_ivar_set(self, id_keydown_inputs, rb_hash_new());
  rb_ivar_set(self, id_keyup_inputs, rb_hash_new());

  return self;
}

VALUE
bk_Event_cInputInterface_get_keydown(VALUE self, VALUE keycode)
{
  return rb_hash_aref(rb_ivar_get(self, id_keydown_inputs), keycode);
}

VALUE
bk_Event_cInputInterface_get_keyup(VALUE self, VALUE keycode)
{
  return rb_hash_aref(rb_ivar_get(self, id_keyup_inputs), keycode);
}

void
Init_blue_kitty_input_device(void)
{
  id_keydown_inputs = rb_intern("@keydown_inputs");
  id_keyup_inputs = rb_intern("@keyup_inputs");

  bk_cInputDevice = rb_define_class_under(bk_m, "InputDevice", rb_cObject);
  rb_define_method(bk_cInputDevice, "initialize",
                   bk_Event_cInputInterface_initialize, 0);
  rb_define_method(bk_cInputDevice, "add_keydown",
                   bk_Event_cInputInterface_add_keydown, 2);
  rb_define_method(bk_cInputDevice, "add_keyup",
                   bk_Event_cInputInterface_add_keyup, 2);
}
