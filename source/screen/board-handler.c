#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

SDL_Rect screen_square_rect(SDL_Rect boardRect, Square square)
{
  int squareWidth = boardRect.w / BOARD_FILES;
  int squareHeight = boardRect.h / BOARD_RANKS;

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {boardRect.x + file * squareWidth, boardRect.y + rank * squareHeight, squareWidth, squareHeight};
}

SDL_Rect square_rect(int width, int height, Square square)
{
  int squareWidth = (width / BOARD_FILES);
  int squareHeight = (height / BOARD_RANKS);

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {squareWidth * file, squareHeight * rank, squareWidth, squareHeight};
}

bool texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Square square)
{
  SDL_Rect squareRect = square_rect(width, height, square);

  return texture_rect_render(renderer, texture, squareRect);
}

bool screen_texture_square_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect boardRect, Square square)
{
  SDL_Rect squareRect = screen_square_rect(boardRect, square);
  
  return texture_rect_render(renderer, texture, squareRect);
}

SDL_Rect margin_rect(SDL_Rect rect, int top, int bottom, int left, int right)
{
  int x = (rect.x + left);
  int y = (rect.y + top);

  int w = (rect.w - left - right);
  int h = (rect.h - top - bottom);

  return (SDL_Rect) {x, y, w, h};
}

SDL_Rect board_rect(int screenWidth, int screenHeight)
{
  SDL_Rect bound = margin_rect((SDL_Rect) {0, 0, screenWidth, screenHeight}, 50, 50, 50, 50);

  int boardLength = (bound.w < bound.h) ? bound.w : bound.h;

  return (SDL_Rect) {bound.x, bound.y, boardLength, boardLength};
}

Square screen_pixels_square(SDL_Rect boardRect, int width, int height)
{
  int relativeWidth = (width - boardRect.x);
  int relativeHeight = (height - boardRect.y);

  int file = ((float) relativeWidth / (float) (boardRect.w + 1)) * BOARD_FILES;
  int rank = ((float) relativeHeight / (float) (boardRect.h + 1)) * BOARD_RANKS;

  if(file < 0 || file >= BOARD_FILES) return SQUARE_NONE;
  if(rank < 0 || rank >= BOARD_RANKS) return SQUARE_NONE;

  return (rank * BOARD_FILES) + file;
}
