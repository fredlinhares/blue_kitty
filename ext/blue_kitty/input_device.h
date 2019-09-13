// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_INPUT_DEVICE_H
#define BLUE_KITTY_INPUT_DEVICE_H 1

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern VALUE bk_cInputDevice;

VALUE
bk_Event_cInputInterface_add_keydown(VALUE self, VALUE keycode,
                                     VALUE command_name);

VALUE
bk_Event_cInputInterface_add_keyup(VALUE self, VALUE keycode,
                                   VALUE command_name);

VALUE
bk_Event_cInputInterface_initialize(VALUE self);

// Internal use only, not for use in Ruby code.
VALUE
bk_Event_cInputInterface_get_keydown(VALUE self, VALUE keycode);

// Internal use only, not for use in Ruby code.
VALUE
bk_Event_cInputInterface_get_keyup(VALUE self, VALUE keycode);

void
Init_blue_kitty_input_device(void);

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_INPUT_DEVICE_H */
