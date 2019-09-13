// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_CORE_DATA_H
#define BLUE_KITTY_CORE_DATA_H 1

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  SDL_bool debug;

  SDL_Window* window;

  unsigned int screen_width;
  unsigned int screen_height;

  const char* game_name;

  unsigned int game_vesion_major;
  unsigned int game_vesion_minor;
  unsigned int game_vesion_patch;
} bk_sCoreData;

#ifdef __cplusplus
}
#endif

#endif /* BLUE_KITTY_CORE_DATA_H */
