#include "engine.h"
#include "screen.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

extern SDL_Rect board_rect_get(int screenWidth, int screenHeight);
 
extern Square board_pixels_square(SDL_Rect boardRect, int width, int height);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern SDL_Texture* PIECE_TEXTURES[12];

extern void board_square_textures_destroy();

extern void board_square_textures_load(SDL_Renderer* renderer);

extern void screen_destroy(Screen* screen);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

extern void board_textures_destroy(BoardTextures* boardTextures);


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


void board_render(Screen screen, BoardTextures boardTextures, Piece holdingPiece)
{
  texture_rect_render(screen.renderer, boardTextures.squares, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.checks, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.moved, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.moves, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.marks, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.pieces, screen.boardRect);

  texture_rect_render(screen.renderer, boardTextures.arrows, screen.boardRect);

  if(holdingPiece != PIECE_NONE)
  {
    // Render texture on the square you are hovering over
    Square square = board_pixels_square(screen.boardRect, screen.mouseX, screen.mouseY);

    if(square != SQUARE_NONE)
    {
      texture_square_render(screen.renderer, HOVER_SQUARE_TEXTURE, screen.boardRect, square);
    }

    texture_pixels_center_render(screen.renderer, PIECE_TEXTURES[holdingPiece], screen.mouseX, screen.mouseY, 100, 100);
  }
}

void screen_display(Screen screen, BoardTextures boardTextures, Piece holdingPiece)
{
  SDL_RenderClear(screen.renderer);

  board_render(screen, boardTextures, holdingPiece);

  SDL_RenderPresent(screen.renderer);
}

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

  if(!screen_create(&screen, 500, 500, "PawnMatch"))
  {
    sdl_drivers_quit();

    error_print("Failed to create screen");

    return 1;
  }
  

  screen.boardRect = board_rect_get(screen.width, screen.height);


  board_square_textures_load(screen.renderer);


  // Important to initiate the struct empty,
  // otherwise, bad pointers occurs
  BoardTextures boardTextures = {0};

  squares_texture_create(&boardTextures.squares, screen.renderer, screen.boardRect);

  pieces_texture_create(&boardTextures.pieces, screen.renderer, screen.boardRect, position, SQUARE_NONE);



  Uint32 lastTicks = SDL_GetTicks();

  const int fps = 60;

  const int frameTicks = 1000 / fps;



  Square grabbedSquare = SQUARE_NONE;
  Square markedSquare = SQUARE_NONE;

  Piece holdingPiece = PIECE_NONE;


  Square rightHoldingSquare = SQUARE_NONE;



  U64 markedSquaresBoard = 0ULL;


  SDL_Event event;

  do
  {
    if(!SDL_WaitEvent(&event))
    {
      error_print("SDL_WaitEvent: %s", SDL_GetError());
    }



    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      int width = event.button.x;
      int height = event.button.y;

      Square square = board_pixels_square(screen.boardRect, width, height);

      Piece piece = boards_square_piece(position.boards, square); 

      if(event.button.button == SDL_BUTTON_LEFT)
      {
        markedSquaresBoard = 0ULL;

        texture_destroy(&boardTextures.marks);


        // If a square is marked and the user clicks on another square, try to make that move
        if(markedSquare != SQUARE_NONE && markedSquare != square)
        {
          // Try to make the move (markedSquare -> square);
          Move move = create_move(position.boards, markedSquare, square);

          if(move_fully_legal(position, move))
          {
            make_move(&position, move);

            markedSquare = SQUARE_NONE;
            grabbedSquare = SQUARE_NONE;


            texture_destroy(&boardTextures.moves);
          }
        }

        // If the user cant make a move or it was an illegal move

        if(piece != PIECE_NONE) 
        {
          grabbedSquare = square;
          holdingPiece = piece;

          texture_destroy(&boardTextures.pieces);

          pieces_texture_create(&boardTextures.pieces, screen.renderer, screen.boardRect, position, grabbedSquare);

          texture_destroy(&boardTextures.moves);

          moves_texture_create(&boardTextures.moves, screen.renderer, screen.boardRect, position, grabbedSquare);
        }
        else
        {
          markedSquare = SQUARE_NONE;
          grabbedSquare = SQUARE_NONE;
        }
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        rightHoldingSquare = square;
      }
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
      int width = event.button.x;
      int height = event.button.y;

      Square square = board_pixels_square(screen.boardRect, width, height);


      if(event.button.button == SDL_BUTTON_LEFT)
      {
        if(grabbedSquare != SQUARE_NONE && square != grabbedSquare)
        {
          // Try to make the move (grabbedSquare -> square);
          Move move = create_move(position.boards, grabbedSquare, square);

          if(move_fully_legal(position, move))
          {
            make_move(&position, move);

            markedSquare = SQUARE_NONE;
            grabbedSquare = SQUARE_NONE;


            texture_destroy(&boardTextures.moves);

            moves_texture_create(&boardTextures.moves, screen.renderer, screen.boardRect, position, grabbedSquare);
          }
        }

        // If you are already holding a piece and release it on the same square,
        // stop holding it
        if(markedSquare != SQUARE_NONE && markedSquare == square)
        {
          markedSquare = SQUARE_NONE;
          grabbedSquare = SQUARE_NONE;

          texture_destroy(&boardTextures.moves);
        }
        else if(square != markedSquare && grabbedSquare != SQUARE_NONE)
        {
          markedSquare = grabbedSquare;
        }

        if(holdingPiece != PIECE_NONE)
        {
          holdingPiece = PIECE_NONE;

          texture_destroy(&boardTextures.pieces);

          pieces_texture_create(&boardTextures.pieces, screen.renderer, screen.boardRect, position, SQUARE_NONE);
        }
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        if(rightHoldingSquare != SQUARE_NONE && square != SQUARE_NONE)
        {
          if(rightHoldingSquare == square)
          {
            if(BOARD_SQUARE_GET(markedSquaresBoard, square))
            {
              markedSquaresBoard = BOARD_SQUARE_POP(markedSquaresBoard, square);
            }
            else markedSquaresBoard = BOARD_SQUARE_SET(markedSquaresBoard, square);

            texture_destroy(&boardTextures.marks);

            marks_texture_create(&boardTextures.marks, screen.renderer, screen.boardRect, markedSquaresBoard);
          }
          else
          {
            fprintf(stdout, "Create arrow: (%d -> %d)\n", rightHoldingSquare, square);
          }
        }

        rightHoldingSquare = SQUARE_NONE;
      }
    }
    else if(event.type == SDL_KEYDOWN)
    {
      if(event.key.keysym.sym == SDLK_SPACE)
      {
        // Pause or resume music
        // music_toggle();
      }
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
      // If holding a piece, update the piece's position
      
      // fprintf(stdout, "Mouse: (%d, %d)\n", event.motion.x, event.motion.y);

      screen.mouseX = event.motion.x;
      screen.mouseY = event.motion.y;
    }
    else if(event.type == SDL_WINDOWEVENT)
    {
      if(event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        screen.width = event.window.data1;
        screen.height = event.window.data2;

        // SDL_SetWindowSize(screen.window, newWidth, newHeight);

        screen.boardRect = board_rect_get(screen.width, screen.height);
      }
    }

    Uint32 currentTicks = SDL_GetTicks();

    if(currentTicks - lastTicks >= frameTicks)
    {
      lastTicks = currentTicks;

      screen_display(screen, boardTextures, holdingPiece);
    }
  }
  while(event.type != SDL_QUIT);


  board_textures_destroy(&boardTextures);

  board_square_textures_destroy();


  screen_destroy(&screen);


  sdl_drivers_quit();
  
  return 0;
}
