#include "engine.h"
#include "screen.h"

const unsigned int FRAMES_PER_SECOND = 60;

const unsigned int FRAME_TICKS = 1000 / FRAMES_PER_SECOND;

extern SDL_Rect board_rect(int screenWidth, int screenHeight);
 
extern Square screen_pixels_square(SDL_Rect boardRect, int width, int height);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern SDL_Texture* PIECE_TEXTURES[12];


extern void screen_destroy(Screen* screen);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

extern void screen_board_textures_create(ScreenBoardTextures* boardTextures, Screen screen, Position position);


extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);


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

int main(int argc, char* argv[])
{
  init_all();

  Position position;
  parse_fen(&position, FEN_START);


  if(!sdl_drivers_init())
  {
    sdl_drivers_quit();

    error_print("Failed to initialize SDL drivers");
    
    return 1;
  }


  Screen screen;

  if(!screen_create(&screen, 600, 500, "PawnMatch"))
  {
    sdl_drivers_quit();

    error_print("Failed to create screen");

    return 1;
  }

  // screen_background_texture_create();
  image_texture_load(&screen.backgroundTexture, screen.renderer, "../source/screen/images/background.png");

  // Creating the basic textures for the board 
  screen_board_textures_create(&screen.board.textures, screen, position);

  screen_display(screen);


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


  screen_destroy(&screen);

  sdl_drivers_quit();
  
  return 0;
}
