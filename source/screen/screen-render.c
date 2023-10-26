#include "../engine.h"
#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern Square screen_pixels_square(SDL_Rect boardRect, int width, int height);

extern bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern SDL_Texture* HOVER_SQUARE_TEXTURE;

extern SDL_Texture* PIECE_TEXTURES[12];


void screen_board_render(Screen screen)
{
  texture_rect_render(screen.renderer, screen.board.textures.squares, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.check, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.moved, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.moves, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.marks, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.pieces, screen.board.rect);

  texture_rect_render(screen.renderer, screen.board.textures.arrows, screen.board.rect);

  if(screen.board.meta.holdingPiece != PIECE_NONE)
  {
    // Render texture on the square you are hovering over
    Square square = screen_pixels_square(screen.board.rect, screen.mouseX, screen.mouseY);

    if(square != SQUARE_NONE)
    {
      screen_texture_square_render(screen.renderer, HOVER_SQUARE_TEXTURE, screen.board.rect, square);
    }

    SDL_Texture* pieceTexture = PIECE_TEXTURES[screen.board.meta.holdingPiece];

    int pieceWidth = screen.board.rect.w / BOARD_FILES;
    int pieceHeight = screen.board.rect.h / BOARD_RANKS;

    texture_pixels_center_render(screen.renderer, pieceTexture, screen.mouseX, screen.mouseY, pieceWidth, pieceHeight);
  }
}

void screen_display(Screen screen)
{
  SDL_RenderClear(screen.renderer);

  texture_pixels_crop_render(screen.renderer, screen.backgroundTexture, 0, 0, screen.width, screen.height);

  screen_board_render(screen);

  SDL_RenderPresent(screen.renderer);
}
