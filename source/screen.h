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

  SDL_Rect boardRect;

  int mouseX, mouseY;
} Screen;

typedef struct
{
  SDL_Texture* squares; // Every square on the board
  SDL_Texture* check;   // The kings square marked on check
  SDL_Texture* moved;   // The last move and the current grabbed square
  SDL_Texture* moves;   // All squares the grabbed piece can move to
  SDL_Texture* marks;   // Every square the user has marked
  SDL_Texture* pieces;  // The pieces (except the one the user holds) 
  SDL_Texture* arrows;  // Every arrow the user has drawn
} BoardTextures;

extern void info_print(char* format, ...);

extern void error_print(char* format, ...);

extern bool sdl_drivers_init();

extern void sdl_drivers_quit();

extern bool screen_create(Screen* screen, int width, int height, const char title[]);

extern bool chunk_play(Mix_Chunk* chunk);

extern void volume_set(int procent);

extern bool sounds_load();

extern void sounds_free();

extern bool music_play();

extern bool pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position, Square liftedSquare);

extern bool moves_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position, Square square);

extern bool check_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position);

extern bool marks_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, U64 marks);

extern bool texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Square square);

extern bool screen_texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect boardRect, Square square);

extern void texture_destroy(SDL_Texture** texture);

#endif // SCREEN_H
