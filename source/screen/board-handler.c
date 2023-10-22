#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

SDL_Rect board_square_rect(SDL_Rect boardRect, Square square)
{
  int squareWidth = boardRect.w / BOARD_FILES;
  int squareHeight = boardRect.h / BOARD_RANKS;

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {file * squareWidth, rank * squareHeight, squareWidth, squareHeight};
}

bool texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect boardRect, Square square)
{
  SDL_Rect squareRect = board_square_rect(boardRect, square);

  return texture_rect_render(renderer, texture, squareRect);
}

SDL_Rect board_rect_get(int screenWidth, int screenHeight)
{
  int boardLength = (screenWidth < screenHeight) ? screenWidth : screenHeight;

  return (SDL_Rect) {0, 0, boardLength, boardLength};
}

Square board_pixels_square(SDL_Rect boardRect, int width, int height)
{
  int file = ((float) width / (float) (boardRect.w + 1)) * BOARD_FILES;
  int rank = ((float) height / (float) (boardRect.h + 1)) * BOARD_RANKS;

  return (rank * BOARD_FILES) + file;
}
