#include "engine.h"
#include "console.h"

void init_all(void)
{
  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  init_board_lookup_lines();
}

const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main(int argc, char* argv[])
{
  initscr();

  refresh();

  getch();

  endwin();

  return 0;
}
