// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_MODEL_H
#define BLUE_KITTY_MODEL_H 1

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern VALUE bk_cModel;

VALUE
bk_alloc_model(VALUE klass);

VALUE
bk_cModel_initialize(VALUE self, VALUE file_path);

void
Init_blue_kitty_model(void);

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_MODEL_IMP_H */
