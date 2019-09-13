// SPDX-License-Identifier: MIT
#include "error.h"

VALUE
bk_eError;

/*
 * Document-class: BlueKitty::Error
 *
 * Default error class for BlueKitty engine.
 * @author Frederico Linhares
 */
void
Init_blue_kitty_error(void)
{
  bk_eError = rb_define_class_under(bk_m, "Error", rb_eStandardError);
}
