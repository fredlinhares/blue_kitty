// SPDX-License-Identifier: MIT
#include "engine.h"

SDL_bool engine_inilialized;
VALUE bk_mEngine;

/*
 * Document-module: BlueKitty::Engine
 *
 * This module contains basic engine functionalities.
 * @author Frederico Linhares
 */

/*
 * Document-method: BlueKitty::Engine.load_core
 *
 * This method is automatically called for you by BlueKitty::Engine.run and is
 * not recommended that you use it directly. Load internal sub-systems, all
 * functionalities in this engine depends on those subsystems so this methods
 * must be called before instantiating any class from BlueKitty.
 *
 * @see BlueKitty::Engine.run
 * @return [BlueKitty::Engine]
 * @author Frederico Linhares
 */

/*
 * Document-method: BlueKitty::Engine.unload_core
 *
 * Unload internal sub-systems. Must be called to free all resources allocated
 * by BlueKitty::Engine.load.
 *
 * @see BlueKitty::Engine.load
 * @return [BlueKitty::Engine]
 * @author Frederico Linhares
 */
void
Init_blue_kitty_engine(void)
{
  engine_inilialized = SDL_FALSE;

  bk_mEngine = rb_define_module_under(bk_m, "Engine");
  rb_define_module_function(bk_mEngine, "load_core", bk_mEngine_load_core, 0);
  rb_define_module_function(bk_mEngine, "loop_stage", bk_mEngine_loop_stage,
                            1);
  rb_define_module_function(bk_mEngine, "unload_core",
                            bk_mEngine_unload_core, 0);
}
