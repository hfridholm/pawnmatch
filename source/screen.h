#ifndef SCREEN_H
#define SCREEN_H

#include "logic.h"
#include "debug.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
  Square grabbedSquare;
  Square markedSquare; // Delete or change this to bool

  Piece holdingPiece;

  Square rightHoldingSquare;

  U64 markedSquaresBoard;

  U64 arrows[64];
} ScreenBoardMeta;

typedef struct
{
  SDL_Texture* squares; // Every square on the board
  SDL_Texture* check;   // The kings square marked on check
  SDL_Texture* moved;   // The last move and the current grabbed square
  SDL_Texture* moves;   // All squares the grabbed piece can move to
  SDL_Texture* marks;   // Every square the user has marked
  SDL_Texture* pieces;  // The pieces (except the one the user holds) 
  SDL_Texture* arrows;  // Every arrow the user has drawn
} ScreenBoardTextures;

typedef struct
{
  // Maybe add head struct for inheritance to abstract screen field struct
  SDL_Rect rect;
  ScreenBoardTextures textures;
  ScreenBoardMeta meta;
}
ScreenBoard;

typedef enum
{
  SMT_MAIN,
  SMT_ENGINE,
  SMT_PLAYER,
  SMT_BOARD
}
ScreenMenuType;

// This struct needs to first in every menu struct
typedef struct
{
  ScreenMenuType type;
  SDL_Texture* background;
}
ScreenMenu;

typedef struct
{
  ScreenMenu menu; // Allows inheritance
  ScreenBoard board;
}
ScreenMenuEngine;

typedef struct
{
  ScreenMenu menu; // Allows inheritance
  ScreenBoard board;
}
ScreenMenuPlayer;

typedef struct
{
  ScreenMenu menu; // Allows inheritance
  ScreenBoard board;
}
ScreenMenuBoard;

typedef struct
{
  ScreenMenu menu; // Allows inheritance
}
ScreenMenuMain;

typedef struct
{
  int x;
  int y;
}
ScreenMouse;

typedef struct
{
  int width;
  int height;
  SDL_Window* window;
  SDL_Renderer* renderer;
  ScreenMouse mouse;
  ScreenMenu* menu;
}
Screen;

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

extern bool arrows_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, const U64 arrows[64]);

extern bool texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Square square);

extern bool screen_texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect boardRect, Square square);

extern void texture_destroy(SDL_Texture** texture);

extern void screen_event_handler(Screen* screen, Position* position, SDL_Event event);

extern void screen_display(Screen screen);

extern bool texture_width_height(SDL_Texture* texture, int* width, int* height);

extern bool texture_rect_crop_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect dstrect);

extern bool texture_pixels_crop_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern bool background_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern bool background_base_textures_load(SDL_Renderer* renderer);

extern void background_base_textures_destroy(void);

extern bool texture_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* srcrect, SDL_Rect* dstrect);

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);

extern bool texture_rect_texture(SDL_Texture** target, SDL_Renderer* renderer, SDL_Texture* source, SDL_Rect srcrect);

extern bool texture_pixels_texture(SDL_Texture** target, SDL_Renderer* renderer, SDL_Texture* source, int x, int y, int width, int height);

extern bool arrow_straight_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Square source, Square target);

extern void screen_menu_board_destroy(ScreenMenuBoard* menu);

extern bool screen_menu_board_create(ScreenMenuBoard* menu, Screen screen);

#endif // SCREEN_H
