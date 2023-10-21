#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);


const char* PIECE_TYPE_STRINGS[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};

SDL_Texture* PIECE_TEXTURES[12];


bool piece_textures_load(SDL_Renderer* renderer)
{
  char filePath[128];

  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    sprintf(filePath, "../source/screen/images/white-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    sprintf(filePath, "../source/screen/images/black-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  fprintf(stdout, "Created textures for all pieces!\n");

  return true;
}

SDL_Texture* WHITE_SQUARE_TEXTURE;
SDL_Texture* BLACK_SQUARE_TEXTURE;

bool square_textures_load(SDL_Renderer* renderer)
{
  if(image_texture_load(&WHITE_SQUARE_TEXTURE, renderer, "../source/screen/images/white-square.png"));

  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));

  fprintf(stdout, "Created textures for squares!\n");

  return true;
}

bool other_textures_load(SDL_Renderer* renderer)
{
  /*
  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));
  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));
  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));
  */
  return true;
}

bool screen_textures_load(SDL_Renderer* renderer)
{
  square_textures_load(renderer);

  piece_textures_load(renderer);

  other_textures_load(renderer);

  return true;
}

void square_textures_destroy()
{
  SDL_DestroyTexture(WHITE_SQUARE_TEXTURE);
  SDL_DestroyTexture(BLACK_SQUARE_TEXTURE);

  fprintf(stdout, "Destroyed square textures!\n");
}

void piece_textures_destroy()
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    SDL_DestroyTexture(PIECE_TEXTURES[piece]);
  }
  fprintf(stdout, "Destroyed piece textures!\n");
}

void screen_textures_destroy()
{
  square_textures_destroy();

  piece_textures_destroy();
}

SDL_Rect board_square_rect(SDL_Rect boardRect, Square square)
{
  int squareWidth = boardRect.w / BOARD_FILES;
  int squareHeight = boardRect.h / BOARD_RANKS;

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {file * squareWidth, rank * squareHeight, squareWidth, squareHeight};
}

extern bool render_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

bool squares_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    SDL_Rect squareRect = board_square_rect(boardRect, square);

    if(((square % BOARD_FILES) + (square / BOARD_FILES)) % 2 == 0)
    {
      texture_rect_render(renderer, WHITE_SQUARE_TEXTURE, squareRect);
    }
    else
    {
      texture_rect_render(renderer, BLACK_SQUARE_TEXTURE, squareRect);
    }
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  fprintf(stdout, "Created squares texture!\n");

  return true;
}

bool pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position, Square liftedSquare)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    if(square == liftedSquare)
    {
      fprintf(stdout, "Rendering not square: %d\n", square);

      continue;
    }

    Piece piece = boards_square_piece(position.boards, square);

    if(piece != PIECE_NONE)
    {
      SDL_Rect squareRect = board_square_rect(boardRect, square);
      
      texture_rect_render(renderer, PIECE_TEXTURES[piece], squareRect);
    }
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  fprintf(stdout, "Updated pieces texture!\n");

  return true;
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
