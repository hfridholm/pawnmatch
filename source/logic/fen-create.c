#include "../logic.h"

void fen_clock_create(char* fen, int* length, int clock)
{
  char string[8];

  sprintf(string, "%d", clock);

  *length += strlen(string);

  strcat(fen, string);
}

void fen_turns_create(char* fen, int* length, int turns)
{
  char string[8];

  sprintf(string, "%d", turns);

  *length += strlen(string);

  strcat(fen, string);
}

void fen_castle_create(char* fen, int* length, Castle castle)
{
  int tempLength = *length;

  if(castle & CASTLE_WHITE_KING) fen[(*length)++] = 'K';

  if(castle & CASTLE_WHITE_QUEEN) fen[(*length)++] = 'Q';

  if(castle & CASTLE_BLACK_KING) fen[(*length)++] = 'k';

  if(castle & CASTLE_BLACK_QUEEN) fen[(*length)++] = 'q';

  if((*length) == tempLength) fen[(*length)++] = '-';
}

void fen_side_create(char* fen, int* length, Side side)
{
  fen[(*length)++] = side == SIDE_WHITE ? 'w' : 'b';
}

void fen_passant_create(char* fen, int* length, Square passant)
{
  if(passant != SQUARE_NONE) 
  {
    *length += 2;

    strcat(fen, SQUARE_STRINGS[passant]);
  }
  else fen[(*length)++] = '-';
}

void fen_board_create(char* fen, int* length, U64 boards[])
{
  int blanks = 0;

  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    for(int file = 0; file < BOARD_FILES; file++)
    {
      Square square = (rank * BOARD_FILES) + file;

      Piece piece = boards_square_piece(boards, square);

      if(piece != PIECE_NONE)
      {
        if(blanks > 0)
        {
          fen[(*length)++] = '0' + blanks;

          blanks = 0;
        }
        fen[(*length)++] = PIECE_SYMBOLS[piece];
      }
      else
      {
        blanks++;
      }
    }
    if(blanks > 0)
    {
      fen[(*length)++] = '0' + blanks;

      blanks = 0;
    }
    if(rank < (BOARD_RANKS - 1)) fen[(*length)++] = '/';
  }
}

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

bool fen_create(char* fen, Position position)
{
  if(fen == NULL) return false;

  int length = 0;

  fen_board_create(fen, &length, position.boards);

  fen[length++] = ' ';

  fen_side_create(fen, &length, position.side);

  fen[length++] = ' ';
  
  fen_castle_create(fen, &length, position.castle);
  
  fen[length++] = ' ';

  fen_passant_create(fen, &length, position.passant);

  fen[length++] = ' ';

  fen_clock_create(fen, &length, position.clock);

  fen[length++] = ' ';

  fen_turns_create(fen, &length, position.turns);

  return true;
}
