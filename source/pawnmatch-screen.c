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


const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main(int argc, char* argv[])
{
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



  SDL_Texture* piecesTexture;
  SDL_Texture* squaresTexture;


  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);


  squares_texture_create(&squaresTexture, screen.renderer, boardRect);

  pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, SQUARE_NONE);



  Uint32 lastTicks = SDL_GetTicks();

  const int fps = 20;

  const int frameTicks = 1000 / fps;



  Piece grabbedPiece = PIECE_NONE;

  int mouseX = 0, mouseY = 0;
  


  SDL_Event event;

  do
  {
    if(!SDL_WaitEvent(&event))
    {
      fprintf(stderr, "SDL_WaitEvent: %s\n", SDL_GetError());
    }



    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      if(event.button.button == SDL_BUTTON_LEFT)
      {
        // Highlight square with piece on it
        // Grab the piece on that square

        int width = event.button.x;
        int height = event.button.y;

        // fprintf(stdout, "Moving at %d, %d\n", width, height);

        Square square = board_pixels_square(boardRect, width, height);

        // fprintf(stdout, "Square: %d\n", square);

        grabbedPiece = boards_square_piece(position.boards, square);

        SDL_DestroyTexture(piecesTexture);

        pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, square);
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        // Either create arrow from square or mark the actual square
        // Remember the square

        int width = event.button.x;
        int height = event.button.y;

        // fprintf(stdout, "Marking at %d, %d\n", width, height);

        Square square = board_pixels_square(boardRect, width, height);

        fprintf(stdout, "Square: %d\n", square);
      }
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
      if(event.button.button == SDL_BUTTON_LEFT)
      {
        // Drop grabbed piece, if holding piece
        // Try to make move from where the left button was clicked on

        grabbedPiece = PIECE_NONE;

        SDL_DestroyTexture(piecesTexture);

        pieces_texture_create(&piecesTexture, screen.renderer, boardRect, position, SQUARE_NONE);
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        // Either draw an arrow from where the right button was clicked on
        // or mark the square, if the right button was released on the same square
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
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render possible squares
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render markings by user
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);

    // Render pieces
    texture_rect_render(screen.renderer, piecesTexture, boardRect);


    // Render arrows
    // texture_rect_render(screen.renderer, squaresTexture, boardRect);


    if(grabbedPiece != PIECE_NONE)
    {
      texture_pixels_center_render(screen.renderer, PIECE_TEXTURES[grabbedPiece], mouseX, mouseY, 100, 100);
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
