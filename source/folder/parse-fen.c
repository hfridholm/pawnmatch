#include "../header.h"

#include "engine-values.h"
#include "string-handler.h"

bool parse_fen_side(Side* side, const char string[])
{
  int stringLength = strlen(string);

  if(stringLength != 1) return false;

  if(*string == 'w')
  {
    *side = SIDE_WHITE;
    return true;
  }
  else if(*string == 'b')
  {
    *side = SIDE_BLACK;
    return true;
  }
  return false;
}

bool parse_fen_clock(int* clock, const char string[])
{
  int number = atoi(string);

  if((number == 0) && (*string != '0')) return false;

  *clock = number;
  return true;
}

bool parse_fen_turns(int* turns, const char string[])
{
  int number = atoi(string);

  if((number == 0) && (*string != '0')) return false;

  *turns = number;
  return true;
}

bool parse_fen_passant(Square* passant, const char string[])
{
  if(!strcmp(string, "-"))
  {
    *passant = SQUARE_NONE;
    return true;
  }

  if(strlen(string) != 2) return false;


  int file = string[0] - 'a';
  int rank = BOARD_RANKS - (string[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return false;

  *passant = (rank * BOARD_FILES) + file;

  return true;
}

bool parse_fen_castle(Castle* castle, const char string[])
{
  if(!strcmp(string, "-")) return true;

  int stringLength = strlen(string);

  // Future Fix: Check for duplicate symbols and string length
  for(int index = 0; index < stringLength; index++)
  {
    switch(string[index])
    {
      case 'K':
        *castle |= CASTLE_WHITE_KING;
        break;

      case 'Q':
        *castle |= CASTLE_WHITE_QUEEN;
        break;

      case 'k':
        *castle |= CASTLE_BLACK_KING;
        break;

      case 'q':
        *castle |= CASTLE_BLACK_QUEEN;
        break;

      default: return false;
    }
  }
  return true;
}

bool parse_fen_boards(U64 boards[12], const char string[])
{
  char stringArray[8][128];

  int stringLength = strlen(string);

  if(!split_string_delim(stringArray, string, stringLength, "/", BOARD_RANKS)) return false;

  for(int rank = 0; rank < BOARD_RANKS; rank++)
  {
    // Future Fix: check if rank length is less or equal to 8
    // and if there is any symbols that should not be in fen string
    int file = 0;

    for(int index = 0; index < strlen(stringArray[rank]); index++)
    {
      unsigned char symbol = stringArray[rank][index];


      Square square = (rank * BOARD_FILES) + file;

      if((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'))
      {
        Piece piece = SYMBOL_PIECES[symbol];

        boards[piece] = BOARD_SQUARE_SET(boards[piece], square);

        file++;
      }
      else if(symbol >= '0' && symbol <= '9')
      {
        file += (symbol - '0');
      }
      else return false;
    }
  }
  return true;
}

bool parse_fen(Position* position, const char fenString[])
{
  memset(position->boards, 0ULL, sizeof(position->boards));

  memset(position->covers, 0ULL, sizeof(position->covers));


  position->side = SIDE_WHITE;

  position->castle = 0;

  position->passant = SQUARE_NONE;


  int stringLength = strlen(fenString);

  char stringArray[6][128];

  if(!split_string_delim(stringArray, fenString, stringLength, " ", 6)) return false;


  if(!parse_fen_boards(position->boards, stringArray[0])) return false;

  if(!parse_fen_side(&position->side, stringArray[1])) return false;

  if(!parse_fen_castle(&position->castle, stringArray[2])) return false;

  if(!parse_fen_passant(&position->passant, stringArray[3])) return false;

  if(!parse_fen_clock(&position->clock, stringArray[4])) return false;

  if(!parse_fen_turns(&position->turns, stringArray[5])) return false;


  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    position->covers[SIDE_WHITE] |= position->boards[piece];
  }

  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    position->covers[SIDE_BLACK] |= position->boards[piece];
  }

  position->covers[SIDE_BOTH] = position->covers[SIDE_WHITE] | position->covers[SIDE_BLACK];


  return true;
}