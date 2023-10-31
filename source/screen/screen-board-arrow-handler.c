#include "../screen.h"

extern bool render_target_texture_setup(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern bool render_target_texture_render(SDL_Texture** texture, SDL_Renderer* renderer);

extern bool board_square_attacked(Position position, Square square, Side side);

extern bool texture_pixels_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern SDL_Texture* ARROW_HEAD_TEXTURE;
extern SDL_Texture* ARROW_BODY_TEXTURE;
extern SDL_Texture* ARROW_TALE_TEXTURE;

bool arrow_flat_straight_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int boardWidth, int boardHeight, double bodySquares)
{
  int squareWidth = (boardWidth / BOARD_FILES);
  int squareHeight = (boardHeight / BOARD_RANKS);

  int arrowWidth = (squareWidth * (bodySquares + 2));
  int arrowHeight = squareHeight;

  if(!render_target_texture_setup(texture, renderer, arrowWidth, arrowHeight)) return false;

  texture_pixels_render(renderer, ARROW_TALE_TEXTURE, 0, 0, squareWidth, squareHeight);

  texture_pixels_render(renderer, ARROW_BODY_TEXTURE, squareWidth, 0, squareWidth * bodySquares, squareHeight);

  texture_pixels_render(renderer, ARROW_HEAD_TEXTURE, squareWidth * (bodySquares + 1), 0, squareWidth, squareHeight);

  return render_target_texture_render(texture, renderer);
}

bool arrow_straight_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int boardWidth, int boardHeight, Square source, Square target)
{
  int squareWidth = (boardWidth / BOARD_FILES);
  int squareHeight = (boardHeight / BOARD_RANKS);

  int sourceRank = (source / BOARD_FILES);
  int sourceFile = (source % BOARD_FILES);

  int fileDiff = (target % BOARD_FILES) - (source % BOARD_FILES);
  int rankDiff = (target / BOARD_FILES) - (source / BOARD_FILES);

  double radians = atan2((double) rankDiff, (double) fileDiff);

  double angle = (radians * 180.0 / (double) M_PI);

  double bodySquares = sqrt(pow(abs(fileDiff), 2) + pow(abs(rankDiff), 2)) - 1;

  SDL_Rect dstrect = {squareWidth * sourceFile, squareHeight * sourceRank, squareWidth * (bodySquares + 2), squareHeight};

  SDL_Point point = {squareWidth / 2, squareHeight / 2};

  SDL_Texture* flatTexture = NULL;
  if(!arrow_flat_straight_texture_create(&flatTexture, renderer, boardWidth, boardHeight, bodySquares))
  {
    printf("arrow_flat_straight_texture_create\n");
    return false;
  }

  if(!render_target_texture_setup(texture, renderer, boardWidth, boardHeight))
  {
    printf("!render_target_texture_setup\n");
    texture_destroy(&flatTexture);
    return false;
  }

  // https://wiki.libsdl.org/SDL2/SDL_RenderCopyEx
  if(SDL_RenderCopyEx(renderer, flatTexture, NULL, &dstrect, angle, &point, 0) != 0)
  {
    error_print("SDL_RenderCopyEx: %s\n", SDL_GetError());
  }

  texture_destroy(&flatTexture);

  if(!render_target_texture_render(texture, renderer))
  {
    printf("!render_target_texture_render\n");
    return false;
  }
  return true;
}

