#include "logic.h"
#include "screen.h"
#include "engine.h"
#include "debug.h"
#include "console.h"

#include <pthread.h>

const unsigned int FRAMES_PER_SECOND = 60;

const unsigned int FRAME_TICKS = 1000 / FRAMES_PER_SECOND;

extern SDL_Texture* PIECE_TEXTURES[12];

extern void screen_destroy(Screen* screen);

extern void screen_board_textures_create(ScreenBoardTextures* boardTextures, Screen screen, Position position);


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
Clock cclock;

Screen screen;

bool gameIsRunning = true;

int engineSocket = -1;

bool engine_move_get(Move* move, Position position)
{
  char positionString[256];

  uci_position_string(positionString, position);

  engine_write(engineSocket, positionString);

  char goString[64];
  sprintf(goString, "go wtime %ld btime %ld winc %d binc %d", cclock.wtime, cclock.btime, cclock.winc, cclock.binc);

  engine_write(engineSocket, goString);


  char bestmoveString[16];
  
  engine_read(engineSocket, bestmoveString, sizeof(bestmoveString));

  Move bestmove = uci_bestmove_parse(bestmoveString);

  bestmove = complete_move(position.boards, bestmove);

  if(bestmove == MOVE_NONE) return false;

  *move = bestmove;

  return true;
}

void* engine_routine(void* args)
{
  while(gameIsRunning)
  {
    // Check for both SIDE_WHITE and SIDE_BLACK,
    // depending on which side the engine plays
    while(position.side != SIDE_BLACK)
    {
      info_print("Waiting for player move");

      usleep(1000000);
    }

    Move move;

    if(!engine_move_get(&move, position))
    {
      error_print("Could not get engine move");

      return NULL;
    }

    make_move(&position, move);
  
    // This is temporary, to be sure that the loop does not run wild
    position.side = SIDE_WHITE;
  }
  return NULL;
}

int main(int argc, char* argv[])
{
  // https://wiki.libsdl.org/SDL2/SDL_RenderFillRect

  init_all();

  parse_fen(&position, FEN_START);
  cclock = (Clock) {60000, 60000, 0, 0};

  if(!screen_create(&screen, 800, 600, "PawnMatch"))
  {
    error_print("Failed to create screen");

    return 1;
  }

  screen_board_textures_create(&screen.board.textures, screen, position);
  screen_display(screen);
  
  char engineAddress[] = "127.0.0.1";
  int enginePort = 5555;


  if(!engine_setup(&engineSocket, engineAddress, enginePort))
  {
    error_print("Could not create engine");

    screen_destroy(&screen);

    return 2;
  }

  pthread_t engineThread;

  if(pthread_create(&engineThread, NULL, &engine_routine, NULL) != 0)
  {
    error_print("Could not create engine thread");
  }
  else info_print("Created engine thread");



  bool engineScreenUpdateToken = true;

  Uint32 lastTicks = SDL_GetTicks();

  SDL_Event event;

  while(gameIsRunning)
  {
    while(SDL_PollEvent(&event))
    {
      screen_event_handler(&screen, &position, event);

      if(event.type == SDL_QUIT) gameIsRunning = false;
      
      usleep(1000);
    }

    if(position.side == SIDE_WHITE && engineScreenUpdateToken)
    {
      // Update the check square texture
      check_texture_create(&screen.board.textures.check, screen.renderer, screen.board.rect.w, screen.board.rect.h, position);

      // Update the pieces texture
      pieces_texture_create(&screen.board.textures.pieces, screen.renderer, screen.board.rect.w, screen.board.rect.h, position, SQUARE_NONE);

      engineScreenUpdateToken = false;
    }
    if(position.side == SIDE_BLACK)
    {
      engineScreenUpdateToken = true;
    }

    Uint32 currentTicks = SDL_GetTicks();

    if(currentTicks - lastTicks >= FRAME_TICKS)
    {
      lastTicks = currentTicks;

      screen_display(screen);
    }
  }

  printf("pthread_cancel: %d\n", pthread_cancel(engineThread));


  if(pthread_join(engineThread, NULL) != 0)
  {
    error_print("Could not join engine thread");
  }
  else info_print("Joined engine thread");

  engine_close(&engineSocket);


  screen_destroy(&screen);

  return 0;
}
