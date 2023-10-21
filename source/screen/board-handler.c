#include "../screen.h"

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);


const char* PIECE_TYPE_STRINGS[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};

SDL_Texture* PIECE_TEXTURES[12];


bool pieces_texture_create(SDL_Renderer* renderer)
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

bool squares_texture_create(SDL_Renderer* renderer)
{
  if(image_texture_load(&WHITE_SQUARE_TEXTURE, renderer, "../source/screen/images/white-square.png"));

  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));

  fprintf(stdout, "Created textures for squares!\n");

  return true;
}

void squares_texture_destroy()
{
  SDL_DestroyTexture(WHITE_SQUARE_TEXTURE);
  SDL_DestroyTexture(BLACK_SQUARE_TEXTURE);

  fprintf(stdout, "Destroyed square textures!\n");
}

void pieces_texture_destroy()
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    SDL_DestroyTexture(PIECE_TEXTURES[piece]);
  }
  fprintf(stdout, "Destroyed piece textures!\n");
}

SDL_Rect board_square_rect(SDL_Rect boardRect, Square square)
{
  int squareWidth = boardRect.w / BOARD_FILES;
  int squareHeight = boardRect.h / BOARD_RANKS;

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {file * squareWidth, rank * squareHeight, squareWidth, squareHeight};
}

bool squares_pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position)
{
  *texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, boardRect.w, boardRect.h);

  if(*texture == NULL)
  {
    fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());

    return false;
  }

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

    Piece piece = boards_square_piece(position.boards, square);

    if(piece != PIECE_NONE)
    {
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

  fprintf(stdout, "Created squares pieces texture!\n");

  return true;
}

SDL_Rect board_rect_get(int screenWidth, int screenHeight)
{
  int boardLength = (screenWidth < screenHeight) ? screenWidth : screenHeight;

  return (SDL_Rect) {0, 0, boardLength, boardLength};
}

bool board_render(Screen screen, Position position)
{
  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);


  SDL_Texture* squaresPiecesTexture;

  if(squares_pieces_texture_create(&squaresPiecesTexture, screen.renderer, boardRect, position))
  {
    fprintf(stdout, "Created squares pieces texture!\n");


    SDL_RenderCopy(screen.renderer, squaresPiecesTexture, NULL, &boardRect);


    SDL_DestroyTexture(squaresPiecesTexture);
  }
  else fprintf(stderr, "Could not create squares pieces texture!\n");

  return true;
}

Square board_pixels_square(SDL_Rect boardRect, int width, int height)
{
  int file = ((float) width / (float) (boardRect.w + 1)) * BOARD_FILES;
  int rank = ((float) height / (float) (boardRect.h + 1)) * BOARD_RANKS;

  return (rank * BOARD_FILES) + file;
}

