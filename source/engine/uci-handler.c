#include "../engine.h"

Square square_parse(const char squareString[])
{
  int file = squareString[0] - 'a';
  int rank = BOARD_RANKS - (squareString[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return SQUARE_NONE;

  return (rank * BOARD_FILES) + file;
}

Move move_parse(const char moveString[])
{
  Move parseMove = 0;

  Square sourceSquare = square_parse(moveString += 0);
  if(sourceSquare == SQUARE_NONE) return 0;

  Square targetSquare = square_parse(moveString += 2);
  if(targetSquare == SQUARE_NONE) return 0;

  parseMove |= MOVE_SET_SOURCE(sourceSquare);
  parseMove |= MOVE_SET_TARGET(targetSquare);

  Piece promotePiece = SYMBOL_PIECES[(unsigned char) *moveString];

  if(promotePiece != PIECE_WHITE_PAWN) parseMove |= MOVE_SET_PROMOTE(promotePiece);

  return parseMove;
}

char* move_string(char* moveString, Move move)
{
  const char* sourceString = SQUARE_STRINGS[MOVE_GET_SOURCE(move)];
  const char* targetString = SQUARE_STRINGS[MOVE_GET_TARGET(move)];

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  if(promotePiece == PIECE_WHITE_PAWN) sprintf(moveString, "%s%s", sourceString, targetString);

  else sprintf(moveString, "%s%s%c", sourceString, targetString, PIECE_SYMBOLS[promotePiece]);

  return moveString;
}

Move uci_bestmove_parse(char bestmoveString[])
{
  if(strncmp(bestmoveString, "bestmove", 8)) return MOVE_NONE;

  return move_parse(bestmoveString + 9);
}

char* uci_position_string(char* positionString, Position position)
{
  char fen[128];
  memset(fen, '\0', sizeof(fen));

  fen_create(fen, position);

  sprintf(positionString, "position fen %s", fen);

  return positionString;
}
