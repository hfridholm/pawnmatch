#ifndef SCREEN_H
#define SCREEN_H

#include "engine.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
  int width, height;
  SDL_Window* window;
  SDL_Renderer* renderer;
} Screen;

extern bool sdl_drivers_init();

extern void sdl_drivers_quit();

extern bool screen_create(Screen* screen, int width, int height, const char title[]);

extern bool pieces_texture_create(SDL_Renderer* renderer);

extern bool chunk_play(Mix_Chunk* chunk);

extern void volume_set(int procent);

extern bool sounds_load();

extern void sounds_free();

extern bool music_play();

extern bool music_resume();

extern bool music_pause();

#endif // SCREEN_H
