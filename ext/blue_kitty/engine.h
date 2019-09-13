// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_ENGINE_H
#define BLUE_KITTY_ENGINE_H 1

#include <SDL2/SDL.h>

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern SDL_bool engine_inilialized;
extern VALUE bk_mEngine;

VALUE
bk_mEngine_load_core(VALUE self);

VALUE
bk_mEngine_loop_stage(VALUE self, VALUE current_stage);

VALUE
bk_mEngine_unload_core(VALUE self);

void
Init_blue_kitty_engine(void);

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_ENGINE_H */
