#include "../screen.h"

extern bool render_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern SDL_Rect board_square_rect(SDL_Rect boardRect, Square square);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

extern bool render_target_texture_setup(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern bool render_target_texture_render(SDL_Texture** texture, SDL_Renderer* renderer);

extern bool board_square_attacked(Position position, Square square, Side side);

extern bool texture_pixels_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);


extern SDL_Texture* WHITE_SQUARE_TEXTURE;
extern SDL_Texture* BLACK_SQUARE_TEXTURE;

extern SDL_Texture* PIECE_TEXTURES[12];

extern SDL_Texture* MOVED_SQUARE_TEXTURE;
extern SDL_Texture* MARK_SQUARE_TEXTURE;
extern SDL_Texture* CHECK_SQUARE_TEXTURE;
extern SDL_Texture* MOVE_SQUARE_TEXTURE;

// The texture should be the same dimensions as the board
bool marks_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, U64 marks)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  int count = 0;
  while(marks && count++ < BOARD_SQUARES)
  {
    int index = board_ls1b_index(marks);

    texture_square_render(renderer, MARK_SQUARE_TEXTURE, width, height, index);

    marks = BOARD_SQUARE_POP(marks, index);
  }

  return render_target_texture_render(texture, renderer);
}

bool arrow_textures_create(SDL_Texture* arrowTextures[4096], int* arrowAmount, SDL_Renderer* renderer, int width, int height, const U64 arrows[64])
{
  *arrowAmount = 0;

  for(Square source = 0; source < BOARD_SQUARES; source++)
  {
    U64 targets = arrows[source];

    int count = 0;
    while(targets && count++ < BOARD_SQUARES)
    {
      int index = board_ls1b_index(targets);

      arrow_straight_texture_create(&arrowTextures[(*arrowAmount)++], renderer, width, height, source, index);

      targets = BOARD_SQUARE_POP(targets, index);
    }
  }
  return true;
}

void textures_destroy(SDL_Texture* textures[], int amount)
{
  for(int index = 0; index < amount; index++)
  {
    texture_destroy(&textures[index]);
  }
}

bool arrows_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, const U64 arrows[64])
{
  SDL_Texture* arrowTextures[4096];
  int arrowAmount = 0;

  if(!arrow_textures_create(arrowTextures, &arrowAmount, renderer, width, height, arrows)) return false;

  if(!render_target_texture_setup(texture, renderer, width, height))
  {
    textures_destroy(arrowTextures, arrowAmount);
    return false;
  }

  for(int index = 0; index < arrowAmount; index++)
  {
    texture_pixels_render(renderer, arrowTextures[index], 0, 0, width, height);

    texture_destroy(&arrowTextures[index]);
  }

  return render_target_texture_render(texture, renderer);
}

extern SDL_Rect square_rect(int width, int height, Square square);

bool squares_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    SDL_Rect squareRect = square_rect(width, height, square);

    if(((square % BOARD_FILES) + (square / BOARD_FILES)) % 2 == 0)
    {
      texture_rect_render(renderer, WHITE_SQUARE_TEXTURE, squareRect);
    }
    else texture_rect_render(renderer, BLACK_SQUARE_TEXTURE, squareRect);
  }

  return render_target_texture_render(texture, renderer);
}

bool check_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  Piece kingPiece = (position.side == SIDE_WHITE) ? PIECE_WHITE_KING : PIECE_BLACK_KING;

  Square kingSquare = board_ls1b_index(position.boards[kingPiece]);

  if(kingSquare == -1)
  {
    texture_destroy(texture);

    error_print("King does not exist");

    return false;
  }

  Side enemySide = (position.side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

  if(board_square_attacked(position, kingSquare, enemySide))
  {
    texture_square_render(renderer, CHECK_SQUARE_TEXTURE, width, height, kingSquare);
  }

  return render_target_texture_render(texture, renderer);
}

bool pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position, Square liftedSquare)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    // Not rendering the liftedSquare
    if(square == liftedSquare) continue;

    Piece piece = boards_square_piece(position.boards, square);

    if(piece != PIECE_NONE)
    {
      SDL_Rect squareRect = square_rect(width, height, square);
      
      texture_rect_render(renderer, PIECE_TEXTURES[piece], squareRect);
    }
  }

  return render_target_texture_render(texture, renderer);
}

bool moves_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height, Position position, Square square)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  // This dont include pawn moves forward
  U64 pieceLookupAttacks = piece_lookup_attacks(position, square);

  // This includes black pawn movement
  pieceLookupAttacks |= (1ULL << (square + BOARD_FILES)) | (1ULL << (square + BOARD_FILES * 2));

  // This includes white pawn movement
  pieceLookupAttacks |= (1ULL << (square - BOARD_FILES)) | (1ULL << (square - BOARD_FILES * 2));

  int loopCount = 0;
  while(pieceLookupAttacks && loopCount++ < BOARD_SQUARES)
  {
    int index = board_ls1b_index(pieceLookupAttacks);

    Move move = create_move(position.boards, square, index);

    if(move_fully_legal(position, move))
    {
      texture_square_render(renderer, MOVE_SQUARE_TEXTURE, width, height, index);
    }

    pieceLookupAttacks = BOARD_SQUARE_POP(pieceLookupAttacks, index);
  }

  return render_target_texture_render(texture, renderer);
}

void screen_board_textures_create(ScreenBoardTextures* boardTextures, Screen screen, Position position)
{
  squares_texture_create(&boardTextures->squares, screen.renderer, screen.board.rect.w, screen.board.rect.h);

  pieces_texture_create(&boardTextures->pieces, screen.renderer, screen.board.rect.w, screen.board.rect.h, position, SQUARE_NONE);
}

void screen_board_textures_destroy(ScreenBoardTextures* boardTextures)
{
  texture_destroy(&boardTextures->squares);

  texture_destroy(&boardTextures->pieces);

  texture_destroy(&boardTextures->moved);

  texture_destroy(&boardTextures->marks);

  texture_destroy(&boardTextures->check);

  texture_destroy(&boardTextures->moves);

  texture_destroy(&boardTextures->arrows);

  info_print("Destroyed board textures");
}
