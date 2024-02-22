#include "../logic.h"
#include "../screen.h"

void screen_board_piece_release(ScreenBoard* board, SDL_Renderer* renderer, Position position)
{
  board->meta.holdingPiece = PIECE_NONE;

  pieces_texture_create(&board->textures.pieces, renderer, board->rect.w, board->rect.h, position, SQUARE_NONE);
}

void screen_board_square_mark(ScreenBoard* board, SDL_Renderer* renderer, Square square)
{
  if(BOARD_SQUARE_GET(board->meta.markedSquaresBoard, square))
  {
    board->meta.markedSquaresBoard = BOARD_SQUARE_POP(board->meta.markedSquaresBoard, square);
  }
  else board->meta.markedSquaresBoard = BOARD_SQUARE_SET(board->meta.markedSquaresBoard, square);

  marks_texture_create(&board->textures.marks, renderer, board->rect.w, board->rect.h, board->meta.markedSquaresBoard);
}

void screen_board_arrow_create(ScreenBoard* board, SDL_Renderer* renderer, Square source, Square target)
{
  if(source < 0 || source >= BOARD_SQUARES) return;
  if(target < 0 || target >= BOARD_SQUARES) return;

  if(BOARD_SQUARE_GET(board->meta.arrows[source], target))
  {
    board->meta.arrows[source] = BOARD_SQUARE_POP(board->meta.arrows[source], target);
  }
  else board->meta.arrows[source] = BOARD_SQUARE_SET(board->meta.arrows[source], target);

  arrows_texture_create(&board->textures.arrows, renderer, board->rect.w, board->rect.h, board->meta.arrows);
}

void screen_board_piece_ungrab(ScreenBoard* board)
{
  board->meta.markedSquare = SQUARE_NONE;
  board->meta.grabbedSquare = SQUARE_NONE;
  
  texture_destroy(&board->textures.moves);
}

void screen_board_move_make(ScreenBoard* board, SDL_Renderer* renderer, Position* position, Move move)
{
  make_move(position, move);

  screen_board_piece_ungrab(board);

  // Update the check square texture
  check_texture_create(&board->textures.check, renderer, board->rect.w, board->rect.h, *position);

  // Update the pieces texture
  pieces_texture_create(&board->textures.pieces, renderer, board->rect.w, board->rect.h, *position, SQUARE_NONE);
}

void screen_board_piece_grab(ScreenBoard* board, SDL_Renderer* renderer, Position position, Square square)
{
  Piece piece = boards_square_piece(position.boards, square);

  // Assign grabbed and holding piece
  board->meta.grabbedSquare = square;
  board->meta.holdingPiece = piece;

  // Update pieces texture to remove the holding piece
  pieces_texture_create(&board->textures.pieces, renderer, board->rect.w, board->rect.h, position, board->meta.grabbedSquare);

  // Create the texture that displays valid moves
  moves_texture_create(&board->textures.moves, renderer, board->rect.w, board->rect.h, position, board->meta.grabbedSquare);
}
