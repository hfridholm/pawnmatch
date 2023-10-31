#include "logic.h"
#include "screen.h"
#include "engine.h"
#include "debug.h"

#include <pthread.h>

const unsigned int FRAMES_PER_SECOND = 60;

const unsigned int FRAME_TICKS = 1000 / FRAMES_PER_SECOND;

extern SDL_Texture* PIECE_TEXTURES[12];

extern void screen_destroy(Screen* screen);

extern void screen_board_textures_create(ScreenBoardTextures* boardTextures, Screen screen, Position position);


U64 create_board_line(Square source, Square target)
{
  U64 board = 0ULL;

  int sourceRank = (source / BOARD_FILES);
  int sourceFile = (source % BOARD_FILES);

  int targetRank = (target / BOARD_FILES);
  int targetFile = (target % BOARD_FILES);

  int rankOffset = (targetRank - sourceRank);
  int fileOffset = (targetFile - sourceFile);

  int rankFactor = (rankOffset > 0) ? +1 : -1;
  int fileFactor = (fileOffset > 0) ? +1 : -1;

  int absRankOffset = (rankOffset * rankFactor);
  int absFileOffset = (fileOffset * fileFactor);

  // If the move is not diagonal nor straight, return empty board;
  if(!(absRankOffset == absFileOffset) && !((absRankOffset == 0) ^ (absFileOffset == 0))) return 0ULL;

  int rankScalor = (rankOffset == 0) ? 0 : rankFactor;
  int fileScalor = (fileOffset == 0) ? 0 : fileFactor;

  for(int rank = sourceRank, file = sourceFile; (rank != targetRank || file != targetFile); rank += rankScalor, file += fileScalor)
  {
    Square square = (rank * BOARD_FILES) + file;

    if(square == source || square == target) continue;

    board = BOARD_SQUARE_SET(board, square);
  }
  return board;
}

void init_board_lookup_lines(void)
{
  for(Square sourceSquare = A8; sourceSquare <= H1; sourceSquare++)
  {
    for(Square targetSquare = A8; targetSquare <= H1; targetSquare++)
    {
      U64 boardLines = create_board_line(sourceSquare, targetSquare);

      BOARD_LOOKUP_LINES[sourceSquare][targetSquare] = boardLines;
    }
  }
}

void init_all(void)
{
  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  init_board_lookup_lines();
}

const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

extern SDL_Texture* HOVER_SQUARE_TEXTURE;

extern SDL_Texture* MARK_SQUARE_TEXTURE;


Position position;

void* engine_routine(void* args)
{
  info_print("Engine routine running");

  // 1.  

  return NULL;
}

int main(int argc, char* argv[])
{
  // https://wiki.libsdl.org/SDL2/SDL_RenderFillRect

  init_all();

  parse_fen(&position, FEN_START);


  Screen screen;
  if(!screen_create(&screen, 800, 600, "PawnMatch"))
  {
    error_print("Failed to create screen");

    return 1;
  }

  screen_board_textures_create(&screen.board.textures, screen, position);
  screen_display(screen);

  screen_display(screen);


  pthread_t engineThread;

  if(pthread_create(&engineThread, NULL, &engine_routine, NULL) != 0)
  {
    error_print("Could not create engine thread");
  }
  else info_print("Created engine thread");




  Uint32 lastTicks = SDL_GetTicks();

  SDL_Event event;

  do
  {
    if(!SDL_WaitEvent(&event))
    {
      error_print("SDL_WaitEvent: %s", SDL_GetError());
    }

    screen_event_handler(&screen, &position, event);


    Uint32 currentTicks = SDL_GetTicks();

    if(currentTicks - lastTicks >= FRAME_TICKS)
    {
      lastTicks = currentTicks;

      screen_display(screen);
    }
  }
  while(event.type != SDL_QUIT);


  if(pthread_join(engineThread, NULL) != 0)
  {
    error_print("Could not join engine thread");
  }
  else info_print("Joined engine thread");


  screen_destroy(&screen);

  return 0;
}
