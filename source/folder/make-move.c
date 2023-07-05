#include "../header.h"

void make_castle_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  Piece rookPiece = (sourcePiece == PIECE_WHITE_KING) ? PIECE_WHITE_ROOK : PIECE_BLACK_ROOK;

  Square rookSourceSquare = (targetSquare > sourceSquare) ? (sourceSquare + 3) : (sourceSquare - 4);

  Square rookTargetSquare = (targetSquare > sourceSquare) ? (targetSquare - 1) : (targetSquare + 1);



  U64 kingMoveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  U64 rookMoveBoard = ((1ULL << rookSourceSquare) ^ (1ULL << rookTargetSquare));

  
  position->boards[sourcePiece] ^= kingMoveBoard;

  position->boards[rookPiece] ^= rookMoveBoard;


  position->covers[side] ^= (kingMoveBoard ^ rookMoveBoard) ;

  position->covers[SIDE_BOTH] ^= (kingMoveBoard ^ rookMoveBoard);



  position->castle &= (sourcePiece == PIECE_WHITE_KING) ? ~CASTLE_WHITE : ~CASTLE_BLACK;
}

void make_normal_quiet_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  position->boards[sourcePiece] ^= moveBoard;

  position->covers[side] ^= moveBoard;


  position->covers[SIDE_BOTH] ^= moveBoard;


  position->clock++;
}

// When an illegal move of capturing own piece, the function dont work 
void make_normal_capture_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  Piece capturePiece = boards_square_piece(position->boards, targetSquare);


  position->boards[sourcePiece] ^= moveBoard;

  position->covers[side] ^= moveBoard;


  if(capturePiece != PIECE_NONE)
  {
    Side captureSide = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

    position->boards[capturePiece] &= ~(1ULL << targetSquare);

    position->covers[captureSide] &= ~(1ULL << targetSquare);
  }


  position->covers[SIDE_BOTH] &= ~(1ULL << sourceSquare);

  position->clock = 0;
}

void make_normal_move(Position* position, Move move)
{
  if(move & MOVE_MASK_CAPTURE)
  {
    make_normal_capture_move(position, move);
  }
  else make_normal_quiet_move(position, move);

  // This is made for both normal moves
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Piece sourcePiece = MOVE_GET_PIECE(move);

  // Fix: Clear the castling rights if the rook gets captures as well!
  if(sourcePiece == PIECE_WHITE_ROOK || sourcePiece == PIECE_BLACK_ROOK)
  {
    if(sourceSquare == A1) position->castle &= ~CASTLE_WHITE_QUEEN;

    else if(sourceSquare == H1) position->castle &= ~CASTLE_WHITE_KING;

    else if(sourceSquare == A8) position->castle &= ~CASTLE_BLACK_QUEEN;

    else if(sourceSquare == H8) position->castle &= ~CASTLE_BLACK_KING;
  }

  if(sourcePiece == PIECE_WHITE_KING) position->castle &= ~CASTLE_WHITE;

  else if(sourcePiece == PIECE_BLACK_KING) position->castle &= ~CASTLE_BLACK;
}

void make_pawn_passant_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;



  Square passantSquare = (sourcePiece == PIECE_WHITE_PAWN) ? (targetSquare + BOARD_FILES) : (targetSquare - BOARD_FILES);

  Piece passantPawn = (sourcePiece == PIECE_WHITE_PAWN) ? PIECE_BLACK_PAWN : PIECE_WHITE_PAWN;

  Side passantSide = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));



  position->boards[passantPawn] &= ~(1ULL << passantSquare);

  position->boards[sourcePiece] ^= moveBoard;


  position->covers[side] ^= moveBoard;

  position->covers[passantSide] &= ~(1ULL << passantSquare);


  position->covers[SIDE_BOTH] ^= moveBoard;
  position->covers[SIDE_BOTH] &= ~(1ULL << passantSquare);
}

void make_pawn_double_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));
  
  // Move the pawn on the board
  position->boards[sourcePiece] ^= moveBoard;

  // Update the side occupancy/cover boards
  position->covers[side] ^= moveBoard;

  // Update the occupancy/cover boards for both sides
  position->covers[SIDE_BOTH] ^= moveBoard;


  position->passant = (sourcePiece == PIECE_WHITE_PAWN) ? (sourceSquare - BOARD_FILES) : (sourceSquare + BOARD_FILES);
}


void make_pawn_promote_quiet_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));


  position->boards[promotePiece] |= (1ULL << targetSquare);

  position->boards[sourcePiece] &= ~(1ULL << sourceSquare);


  position->covers[side] ^= moveBoard;

  position->covers[SIDE_BOTH] ^= moveBoard;
}

void make_pawn_promote_capture_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  Piece capturePiece = boards_square_piece(position->boards, targetSquare);


  position->boards[promotePiece] |= (1ULL << targetSquare);

  position->boards[sourcePiece] &= ~(1ULL << sourceSquare);


  position->covers[side] ^= moveBoard;


  if(capturePiece != PIECE_NONE)
  {
    Side captureSide = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

    position->boards[capturePiece] &= ~(1ULL << targetSquare);

    position->covers[captureSide] &= ~(1ULL << targetSquare);
  }

  position->covers[SIDE_BOTH] &= ~(1ULL << sourceSquare);
} 

void make_pawn_promote_move(Position* position, Move move)
{
  if(move & MOVE_MASK_CAPTURE)
  {
    make_pawn_promote_capture_move(position, move);
  }
  else make_pawn_promote_quiet_move(position, move);

  // This is made for both cases of promote moves
  // position->passant = PIECE_NONE;
}

// This has the same code as make_normal_quiet_move*
void make_pawn_normal_quiet_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  position->boards[sourcePiece] ^= moveBoard;

  position->covers[side] ^= moveBoard;


  position->covers[SIDE_BOTH] ^= moveBoard;
}

// This has the same code as make_normal_capture_move*
void make_pawn_normal_capture_move(Position* position, Move move)
{
  Square sourceSquare = MOVE_GET_SOURCE(move);
  Square targetSquare = MOVE_GET_TARGET(move);

  Piece sourcePiece = MOVE_GET_PIECE(move);

  Side side = (sourcePiece >= PIECE_WHITE_PAWN && sourcePiece <= PIECE_WHITE_KING) ? SIDE_WHITE : SIDE_BLACK;


  U64 moveBoard = ((1ULL << sourceSquare) ^ (1ULL << targetSquare));

  Piece capturePiece = boards_square_piece(position->boards, targetSquare);


  position->boards[sourcePiece] ^= moveBoard;

  position->covers[side] ^= moveBoard;


  if(capturePiece != PIECE_NONE)
  {
    Side captureSide = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

    position->boards[capturePiece] &= ~(1ULL << targetSquare);

    position->covers[captureSide] &= ~(1ULL << targetSquare);
  }


  position->covers[SIDE_BOTH] &= ~(1ULL << sourceSquare);
}

void make_pawn_normal_move(Position* position, Move move)
{
  if(move & MOVE_MASK_CAPTURE)
  {
    make_pawn_normal_capture_move(position, move);
  }
  else make_pawn_normal_quiet_move(position, move);

  // This is made for both pawn normal moves
  // position->passant = PIECE_NONE;
}

void make_pawn_move(Position* position, Move move)
{
  if(move & MOVE_MASK_DOUBLE)
  {
    make_pawn_double_move(position, move);
  }
  else if(move & MOVE_MASK_PROMOTE)
  {
    make_pawn_promote_move(position, move);
  }
  else if(move & MOVE_MASK_PASSANT)
  {
    make_pawn_passant_move(position, move);
  }
  else make_pawn_normal_move(position, move);

  // This is made for every pawn move
  position->clock = 0;
}

void make_move(Position* position, Move move)
{

  // This is the default for every move
  position->clock++;

  position->passant = SQUARE_NONE;


  Piece sourcePiece = MOVE_GET_PIECE(move);

  if(sourcePiece == PIECE_WHITE_PAWN || sourcePiece == PIECE_BLACK_PAWN)
  {
    make_pawn_move(position, move);
  }
  else if(move & MOVE_MASK_CASTLE)
  {
    make_castle_move(position, move);
  }
  else make_normal_move(position, move);

  // This is made for every move
  if(position->side == SIDE_BLACK) position->turns++;

  position->side = (position->side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;
}