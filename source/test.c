#include "header.h"
#include "screen.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

extern bool squares_texture_create(SDL_Renderer* renderer);

extern SDL_Rect board_rect_get(int screenWidth, int screenHeight);
 
extern bool squares_pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position);

extern Square board_pixels_square(SDL_Rect boardRect, int width, int height);

extern bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect);

extern bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

extern SDL_Texture* PIECE_TEXTURES[12];

extern void pieces_texture_destroy();

extern void squares_texture_destroy();

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
  

  pieces_texture_create(screen.renderer);

  squares_texture_create(screen.renderer);



  SDL_Texture* squaresPiecesTexture;


  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);

  if(!squares_pieces_texture_create(&squaresPiecesTexture, screen.renderer, boardRect, position));




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

        fprintf(stdout, "Moving at %d, %d\n", width, height);

        Square square = board_pixels_square(boardRect, width, height);

        fprintf(stdout, "Square: %d\n", square);

        grabbedPiece = boards_square_piece(position.boards, square);



        // Remove the grabbed piece and update the pieces texture
        boardRect = board_rect_get(screen.width, screen.height);

        if(!squares_pieces_texture_create(&squaresPiecesTexture, screen.renderer, boardRect, position));
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        // Either create arrow from square or mark the actual square
        // Remember the square

        int width = event.button.x;
        int height = event.button.y;

        fprintf(stdout, "Marking at %d, %d\n", width, height);

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
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        // Either draw an arrow from where the right button was clicked on
        // or mark the square, if the right button was released on the same square
      }
    }




    Uint32 currentTicks = SDL_GetTicks();

    if(currentTicks - lastTicks < frameTicks) continue;

    lastTicks = currentTicks;



    SDL_RenderClear(screen.renderer);

    // Update! Make squares texture and pieces texture seperate,
    // so that the markings can be renderered in between them

    // Render board squares

    // Render check markings

    // Render moved markings

    // Render possible squares

    // Render markings by user

    // Render pieces

    // Render arrows

    texture_rect_render(screen.renderer, squaresPiecesTexture, boardRect);

    if(grabbedPiece != PIECE_NONE)
    {
      texture_pixels_center_render(screen.renderer, PIECE_TEXTURES[grabbedPiece], mouseX, mouseY, 100, 100);
    }

    SDL_RenderPresent(screen.renderer);



    if(event.type == SDL_MOUSEMOTION)
    {
      // If holding a piece, update the piece's position
      
      fprintf(stdout, "Mouse: (%d, %d)\n", event.motion.x, event.motion.y);

      mouseX = event.motion.x, mouseY = event.motion.y;
    }

    if(event.type == SDL_WINDOWEVENT)
    {
      fprintf(stdout, "event.window.event: %d\n", event.window.event);

      if(event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        int newWidth = event.window.data1;
        int newHeight = event.window.data2;

        screen.width = newWidth;
        screen.height = newHeight;

        // SDL_SetWindowSize(screen.window, newWidth, newHeight);

        fprintf(stdout, "Window: %dx%d\n", screen.width, screen.height);


        boardRect = board_rect_get(screen.width, screen.height);

        if(!squares_pieces_texture_create(&squaresPiecesTexture, screen.renderer, boardRect, position));
      }
    }
  }
  while(event.type != SDL_QUIT);


  SDL_DestroyTexture(squaresPiecesTexture);

  pieces_texture_destroy();

  squares_texture_destroy();

  screen_destroy(screen);


  sdl_drivers_quit();
  
  return 0;
}
