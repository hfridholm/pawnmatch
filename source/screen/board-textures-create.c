#include "../screen.h"

extern bool render_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern SDL_Rect board_square_rect(SDL_Rect boardRect, Square square);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);


extern SDL_Texture* WHITE_SQUARE_TEXTURE;
extern SDL_Texture* BLACK_SQUARE_TEXTURE;

extern SDL_Texture* PIECE_TEXTURES[12];

extern SDL_Texture* MOVED_SQUARE_TEXTURE;
extern SDL_Texture* MARK_SQUARE_TEXTURE;
extern SDL_Texture* CHECK_SQUARE_TEXTURE;
extern SDL_Texture* MOVE_SQUARE_TEXTURE;

extern SDL_Texture* ARROW_HEAD_TEXTURE;
extern SDL_Texture* ARROW_BODY_TEXTURE;


bool marks_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, U64 marks)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  int count = 0;
  while(marks && count++ < BOARD_SQUARES)
  {
    int index = board_ls1b_index(marks);

    texture_square_render(renderer, MARK_SQUARE_TEXTURE, boardRect, index);

    marks = BOARD_SQUARE_POP(marks, index);
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  return true;
}

bool squares_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

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
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  return true;
}

bool pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position, Square liftedSquare)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    // Not rendering the liftedSquare
    if(square == liftedSquare) continue;

    Piece piece = boards_square_piece(position.boards, square);

    if(piece != PIECE_NONE)
    {
      SDL_Rect squareRect = board_square_rect(boardRect, square);
      
      texture_rect_render(renderer, PIECE_TEXTURES[piece], squareRect);
    }
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  return true;
}

bool moves_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position, Square square)
{
  if(!render_texture_create(texture, renderer, boardRect.w, boardRect.h)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  // This dont include pawn moves forward
  U64 pieceLookupAttacks = piece_lookup_attacks(position, square);

  int loopCount = 0;
  while(pieceLookupAttacks && loopCount++ < 64)
  {
    int index = board_ls1b_index(pieceLookupAttacks);

    Move move = create_move(position.boards, square, index);

    if(move_pseudo_legal(position, move))
    {
      texture_square_render(renderer, MOVE_SQUARE_TEXTURE, boardRect, index);
    }

    pieceLookupAttacks = BOARD_SQUARE_POP(pieceLookupAttacks, index);
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  return true;
}

void board_textures_destroy(BoardTextures* boardTextures)
{
  texture_destroy(&boardTextures->squares);

  texture_destroy(&boardTextures->pieces);

  texture_destroy(&boardTextures->moved);

  texture_destroy(&boardTextures->marks);

  texture_destroy(&boardTextures->checks);

  texture_destroy(&boardTextures->moves);

  texture_destroy(&boardTextures->arrows);
}
