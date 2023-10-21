#ifndef COMPLETE_MOVE_H
#define COMPLETE_MOVE_H

extern Move create_double_move(Square sourceSquare, Square targetSquare, Piece piece);

extern Move create_promote_move(U64 boards[12], Square sourceSquare, Square targetSquare, Piece piece, Piece promotePiece);

extern Move create_castle_move(Square sourceSquare, Square targetSquare, Piece piece);

extern Move create_normal_move(U64 boards[12], Square sourceSquare, Square targetSquare, Piece piece);

extern bool ident_passant_move(U64 boards[12], Piece sourcePiece, Square sourceSquare, Square targetSquare);

#endif