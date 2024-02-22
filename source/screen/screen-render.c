#include "../logic.h"
#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern Square screen_pixels_square(SDL_Rect boardRect, int width, int height);

extern bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern SDL_Texture* HOVER_SQUARE_TEXTURE;
extern SDL_Texture* MOVED_SQUARE_TEXTURE;

extern SDL_Texture* PIECE_TEXTURES[12];


void screen_board_render(ScreenBoard board, SDL_Renderer* renderer)
{
  texture_rect_render(renderer, board.textures.squares, board.rect);

  texture_rect_render(renderer, board.textures.check, board.rect);

  texture_rect_render(renderer, board.textures.moved, board.rect);

  if(board.meta.grabbedSquare != SQUARE_NONE)
  {
    screen_texture_square_render(renderer, MOVED_SQUARE_TEXTURE, board.rect, board.meta.grabbedSquare);
  }

  texture_rect_render(renderer, board.textures.marks, board.rect);

  texture_rect_render(renderer, board.textures.pieces, board.rect);

  texture_rect_render(renderer, board.textures.moves, board.rect);

  texture_rect_render(renderer, board.textures.arrows, board.rect);
}

void screen_holding_piece_render()
{
  /*
  if(board.meta.holdingPiece != PIECE_NONE)
  {
    // Render texture on the square you are hovering over
    Square square = screen_pixels_square(board.rect, screen.mouseX, screen.mouseY);

    if(square != SQUARE_NONE)
    {
      screen_texture_square_render(renderer, HOVER_SQUARE_TEXTURE, board.rect, square);
    }

    SDL_Texture* pieceTexture = PIECE_TEXTURES[board.meta.holdingPiece];

    int pieceWidth = board.rect.w / BOARD_FILES;
    int pieceHeight = board.rect.h / BOARD_RANKS;

    texture_pixels_center_render(renderer, pieceTexture, screen.mouseX, screen.mouseY, pieceWidth, pieceHeight);
  }
  */
}

void screen_menu_board_render(Screen screen)
{
  ScreenMenuBoard menu = *((ScreenMenuBoard*) screen.menu);

  texture_pixels_crop_render(screen.renderer, menu.menu.background, 0, 0, screen.width, screen.height);

  screen_board_render(menu.board, screen.renderer);
}

void screen_menu_render(Screen screen)
{
  switch(screen.menu->type)
  {
    case SMT_MAIN:
      break;

    case SMT_ENGINE:
      break;

    case SMT_PLAYER:
      break;

    case SMT_BOARD:
      screen_menu_board_render(screen);
  }
}

void screen_display(Screen screen)
{
  SDL_RenderClear(screen.renderer);

  // If there is no menu to display, return
  if(screen.menu == NULL) return; 

  screen_menu_render(screen);

  SDL_RenderPresent(screen.renderer);
}
