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

extern void screen_textures_destroy();

extern void screen_destroy(Screen screen);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);


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


SDL_Texture* MOVED_SQUARE_TEXTURE;
SDL_Texture* MARKED_SQUARE_TEXTURE;
SDL_Texture* MOVE_SQUARE_TEXTURE;


int main(int argc, char* argv[])
{
  init_all();

  Position position;
  parse_fen(&position, FEN_START);


  if(!sdl_drivers_init())
  {
    sdl_drivers_quit();

    fprintf(stderr, "Failed to initialize SDL drivers\n");
    
    return 1;
  }
  else fprintf(stdout, "Initialized SDL Drivers\n");


  Screen screen;

  if(!screen_create(&screen, 500, 500, "PawnMatch"))
  {
    sdl_drivers_quit();

    fprintf(stderr, "Failed to create screen!\n");

    return 1;
  }
  else fprintf(stdout, "Created screen!\n");
  


  screen_textures_load(screen.renderer);

  image_texture_load(&MOVED_SQUARE_TEXTURE, screen.renderer, "../source/screen/images/moved-square.png");
  image_texture_load(&MARKED_SQUARE_TEXTURE, screen.renderer, "../source/screen/images/mark-square.png");
  image_texture_load(&MOVE_SQUARE_TEXTURE, screen.renderer, "../source/screen/images/move-square.png");


  SDL_Texture* piecesTexture;
  SDL_Texture* squaresTexture;
  SDL_Texture* movesTexture;


  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);


  squares_texture_create(&squaresTexture, screen.renderer, boardRect);

  pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, SQUARE_NONE);



  Uint32 lastTicks = SDL_GetTicks();

  const int fps = 60;

  const int frameTicks = 1000 / fps;



  Square grabbedSquare = SQUARE_NONE;
  Square markedSquare = SQUARE_NONE;

  Piece holdingPiece = PIECE_NONE;


  Square rightHoldingSquare = SQUARE_NONE;


  int mouseX = 0, mouseY = 0;


  U64 markedSquaresBoard = 0;


  SDL_Event event;

  do
  {
    if(!SDL_WaitEvent(&event))
    {
      fprintf(stderr, "SDL_WaitEvent: %s\n", SDL_GetError());
    }



    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      int width = event.button.x;
      int height = event.button.y;

      Square square = board_pixels_square(boardRect, width, height);

      Piece piece = boards_square_piece(position.boards, square); 

      if(event.button.button == SDL_BUTTON_LEFT)
      {
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

            markedSquaresBoard = 0ULL;
          }
        }

        // If the user cant make a move or it was an illegal move
        if(piece != PIECE_NONE) 
        {
          grabbedSquare = square;
          holdingPiece = piece;

          SDL_DestroyTexture(piecesTexture);

          pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, grabbedSquare);

          SDL_DestroyTexture(movesTexture);

          moves_texture_create(&movesTexture, screen.renderer, boardRect, position, grabbedSquare);
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

      Square square = board_pixels_square(boardRect, width, height);


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

            markedSquaresBoard = 0ULL;
          }
        }

        // If you are already holding a piece and release it on the same square,
        // stop holding it
        if(markedSquare != SQUARE_NONE && markedSquare == square)
        {
          markedSquare = SQUARE_NONE;
          grabbedSquare = SQUARE_NONE;
        }
        else if(square != markedSquare && grabbedSquare != SQUARE_NONE)
        {
          markedSquare = grabbedSquare;
        }

        if(holdingPiece != PIECE_NONE)
        {
          holdingPiece = PIECE_NONE;

          SDL_DestroyTexture(piecesTexture);

          pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, SQUARE_NONE);
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

      mouseX = event.motion.x, mouseY = event.motion.y;
    }
    else if(event.type == SDL_WINDOWEVENT)
    {
      if(event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        screen.width = event.window.data1;
        screen.height = event.window.data2;

        // SDL_SetWindowSize(screen.window, newWidth, newHeight);

        // fprintf(stdout, "Window: %dx%d\n", screen.width, screen.height);


        boardRect = board_rect_get(screen.width, screen.height);
      }
    }

    Uint32 currentTicks = SDL_GetTicks();

    if(currentTicks - lastTicks < frameTicks) continue;

    lastTicks = currentTicks;


    SDL_RenderClear(screen.renderer);

    // Render board squares
    texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render check markings
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render moved markings

    if(grabbedSquare != SQUARE_NONE)
    {
      texture_square_render(screen.renderer, MOVED_SQUARE_TEXTURE, boardRect, grabbedSquare);
    }
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render possible squares
    if(grabbedSquare != SQUARE_NONE)
    {
      texture_rect_render(screen.renderer, movesTexture, boardRect);
    }

    // Render markings by user
    U64 markedSquaresBoardCopy = markedSquaresBoard;
  
    int loopCount = 0;
    while(markedSquaresBoardCopy && loopCount++ < 64)
    {
      int index = board_ls1b_index(markedSquaresBoardCopy);

      texture_square_render(screen.renderer, MARKED_SQUARE_TEXTURE, boardRect, index);
      
      markedSquaresBoardCopy = BOARD_SQUARE_POP(markedSquaresBoardCopy, index);
    }

    // Render pieces
    texture_rect_render(screen.renderer, piecesTexture, boardRect);


    // Render arrows
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);


    if(holdingPiece != PIECE_NONE)
    {
      texture_pixels_center_render(screen.renderer, PIECE_TEXTURES[holdingPiece], mouseX, mouseY, 100, 100);
    }

    SDL_RenderPresent(screen.renderer);
  }
  while(event.type != SDL_QUIT);


  SDL_DestroyTexture(squaresTexture);
  SDL_DestroyTexture(piecesTexture);


  screen_textures_destroy();


  screen_destroy(screen);


  sdl_drivers_quit();
  
  return 0;
}
