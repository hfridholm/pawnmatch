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

extern bool chunk_play(Mix_Chunk* chunk);

extern void volume_set(int procent);

extern bool sounds_load();

extern void sounds_free();

extern bool music_play();

extern bool screen_textures_load(SDL_Renderer* renderer);

extern void screen_textures_destroy();

extern bool squares_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect);

extern bool pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position, Square liftedSquare);

#endif // SCREEN_H
