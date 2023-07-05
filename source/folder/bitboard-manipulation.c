#include "../header.h"

int board_bit_amount(U64 bitboard)
{
  int count = 0;

  // for(count = 0; bitboard; ++count)
  while(bitboard)
  {
    count++;
    bitboard &= bitboard - 1;
  }

  return count;
}

int board_ls1b_index(U64 bitboard)
{
  if(!bitboard) return -1;

  return board_bit_amount((bitboard & -bitboard) - 1);
}