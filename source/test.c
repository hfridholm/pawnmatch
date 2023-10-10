#include "header.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
  int width, height;
  SDL_Window* window;
  SDL_Renderer* renderer;
} Screen;



const char* PIECE_TYPE_STRINGS[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};

SDL_Texture* PIECE_TEXTURES[12];

SDL_Texture* WHITE_SQUARE_TEXTURE;
SDL_Texture* BLACK_SQUARE_TEXTURE;



bool window_create(SDL_Window** window, int width, int height, const char title[])
{
  if((*window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)) == NULL)
  {
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());

    return false;
  }
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

  SDL_SetWindowMinimumSize(*window, 400, 400);

  return true;
}

bool renderer_create(SDL_Renderer** renderer, SDL_Window* window)
{
  if((*renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
  {
    fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());

    return false;
  }
  return true;
}

bool screen_create(Screen* screen, int width, int height, const char title[])
{
  if(!window_create(&screen->window, width, height, title))
  {
    return false;
  }
  if(!renderer_create(&screen->renderer, screen->window))
  {
    return false;
  }
  screen->width = width;
  screen->height = height;

  return true;
}

void screen_destroy(Screen screen)
{
  if(screen.renderer != NULL) SDL_DestroyRenderer(screen.renderer);

  if(screen.window != NULL) SDL_DestroyWindow(screen.window);
}

bool image_surface_load(SDL_Surface** surface, const char filePath[])
{
  if((*surface = IMG_Load(filePath)) == NULL)
  {
    fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());

    return false;
  }
  return true;
}

bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[])
{
  SDL_Surface* surface;

  if(!image_surface_load(&surface, filePath))
  {
    return false;
  }

  *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  if(*texture == NULL)
  {
    fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());

    return false;
  }
  return true;
}

bool pieces_texture_create(SDL_Renderer* renderer)
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    char filePath[128];
    
    sprintf(filePath, "../source/screen/images/white-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    char filePath[128];
    
    sprintf(filePath, "../source/screen/images/black-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  fprintf(stdout, "Created textures for all pieces!\n");

  return true;
}

bool squares_texture_create(SDL_Renderer* renderer)
{
  if(image_texture_load(&WHITE_SQUARE_TEXTURE, renderer, "../source/screen/images/white-square.png"));

  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));

  fprintf(stdout, "Created textures for squares!\n");

  return true;
}

void squares_texture_destroy()
{
  SDL_DestroyTexture(WHITE_SQUARE_TEXTURE);
  SDL_DestroyTexture(BLACK_SQUARE_TEXTURE);

  fprintf(stdout, "Destroyed square textures!\n");
}

void pieces_texture_destroy()
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    SDL_DestroyTexture(PIECE_TEXTURES[piece]);
  }
  fprintf(stdout, "Destroyed piece textures!\n");
}

bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect)
{
  if(SDL_RenderCopy(renderer, texture, NULL, &destRect) != 0)
  {
    fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());

    return false;
  }
  return true;
}

bool texture_pixels_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height)
{
  SDL_Rect destRect = {x, y, width, height};

  return texture_rect_render(renderer, texture, destRect);
}

bool texture_pixels_center_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height)
{
  // Add an offset to x and y depending on width and height
  return texture_pixels_render(renderer, texture, x - (width / 2), y - (height / 2), width, height);
}

void sdl_drivers_quit()
{
  Mix_CloseAudio();

  // SDL_WasInit
  SDL_Quit();

  IMG_Quit();

  TTF_Quit();

  Mix_Quit();
}

bool sdl_drivers_init()
{
  if(SDL_Init(SDL_INIT_VIDEO) != 0) // | SDL_INIT_AUDIO
  {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
  
    return false;
  }
  if(IMG_Init(IMG_INIT_PNG) == 0)
  {
    fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
  
    return false;
  }
  if(TTF_Init() == -1)
  {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
  
    return false;
  }
  if(Mix_Init(0) != 0)
  {
    fprintf(stderr, "TTF_Init: %s\n", Mix_GetError());
  
    return false;
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
  {
    fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
    
    return false;
  }
  return true;
}

bool chunk_load(Mix_Chunk** chunk, const char filePath[])
{
  if((*chunk = Mix_LoadWAV(filePath)) == NULL)
  {
    fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());

    return false;
  }
  return true;
}

SDL_Rect board_rect_get(int screenWidth, int screenHeight)
{
  int boardLength = (screenWidth < screenHeight) ? screenWidth : screenHeight;

  return (SDL_Rect) {0, 0, boardLength, boardLength};
}

SDL_Rect board_square_rect(SDL_Rect boardRect, Square square)
{
  int squareWidth = boardRect.w / BOARD_FILES;
  int squareHeight = boardRect.h / BOARD_RANKS;

  int file = (square % BOARD_FILES);
  int rank = (square / BOARD_FILES);

  return (SDL_Rect) {file * squareWidth, rank * squareHeight, squareWidth, squareHeight};
}

Square board_pixels_square(SDL_Rect boardRect, int width, int height)
{
  int file = ((float) width / (float) (boardRect.w + 1)) * BOARD_FILES;
  int rank = ((float) height / (float) (boardRect.h + 1)) * BOARD_RANKS;

  return (rank * BOARD_FILES) + file;
}

bool squares_pieces_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, SDL_Rect boardRect, Position position)
{
  *texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, boardRect.w, boardRect.h);

  if(*texture == NULL)
  {
    fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());

    return false;
  }

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  for(Square square = 0; square < BOARD_SQUARES; square++)
  {
    SDL_Rect squareRect = board_square_rect(boardRect, square);

    if(((square % BOARD_FILES) + (square / BOARD_FILES)) % 2 == 0)
    {
      texture_rect_render(renderer, WHITE_SQUARE_TEXTURE, squareRect);
    }
    else
    {
      texture_rect_render(renderer, BLACK_SQUARE_TEXTURE, squareRect);
    }

    Piece piece = boards_square_piece(position.boards, square);

    if(piece != PIECE_NONE)
    {
      texture_rect_render(renderer, PIECE_TEXTURES[piece], squareRect);
    }
  }

  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    fprintf(stderr, "SDL_SetRenderTarget: %s\n", SDL_GetError());

    SDL_DestroyTexture(*texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  fprintf(stdout, "Created squares pieces texture!\n");

  return true;
}

bool board_render(Screen screen, Position position)
{
  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);


  SDL_Texture* squaresPiecesTexture;

  if(squares_pieces_texture_create(&squaresPiecesTexture, screen.renderer, boardRect, position))
  {
    fprintf(stdout, "Created squares pieces texture!\n");


    SDL_RenderCopy(screen.renderer, squaresPiecesTexture, NULL, &boardRect);


    SDL_DestroyTexture(squaresPiecesTexture);
  }
  else fprintf(stderr, "Could not create squares pieces texture!\n");

  return true;
}

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


  /*
  SDL_RenderClear(screen.renderer);


  board_render(screen, position);


  SDL_RenderPresent(screen.renderer);



  SDL_Rect boardRect = board_rect_get(screen.width, screen.height);



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
        int width = event.button.x;
        int height = event.button.y;

        fprintf(stdout, "Moving at %d, %d\n", width, height);

        Square square = board_pixels_square(boardRect, width, height);

        fprintf(stdout, "Square: %d\n", square);
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        int width = event.button.x;
        int height = event.button.y;

        fprintf(stdout, "Marking at %d, %d\n", width, height);

        Square square = board_pixels_square(boardRect, width, height);

        fprintf(stdout, "Square: %d\n", square);
      }
    }
    else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
      SDL_RenderClear(screen.renderer);


      board_render(screen, position);


      SDL_RenderPresent(screen.renderer);

      int newWidth = event.window.data1;
      int newHeight = event.window.data2;

      screen.width = newWidth;
      screen.height = newHeight;

      // SDL_SetWindowSize(screen.window, newWidth, newHeight);

      fprintf(stdout, "Window: %dx%d\n", screen.width, screen.height);
    }
  }
  while(event.type != SDL_QUIT);
  */




  SDL_DestroyTexture(squaresPiecesTexture);

  pieces_texture_destroy();

  squares_texture_destroy();

  screen_destroy(screen);


  /*
  SDL_Window* window;

  if(window_create(&window, 400, 400, "PawnMatch"))
  {
    fprintf(stdout, "Created window!\n");

    SDL_Renderer* renderer;

    if(renderer_create(&renderer, window))
    {
      fprintf(stdout, "Created renderer!\n");

      SDL_Texture* texture;

      if(image_texture_load(&texture, renderer, "../source/screen/images/image-1.png"))
      {
        fprintf(stdout, "Loaded image!\n");

        SDL_RenderClear(renderer);

        SDL_Rect destRect = {10, 10, 390, 390};

        if(SDL_RenderCopy(renderer, texture, NULL, &destRect) != 0)
        {
          fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(5000);

        fprintf(stdout, "Destroyed texture!\n");

        SDL_DestroyTexture(texture);
      }

      fprintf(stdout, "Destroyed renderer!\n");
    
      SDL_DestroyRenderer(renderer);
    }

    fprintf(stdout, "Destroyed window!\n");

    SDL_DestroyWindow(window);
  }
  */


  // Mix_LoadMUS()
  
  // Mix_PlayingMusic()
  // Mix_Volume()
  // Mix_PlayMusic()

  // Mix_PausedMusic()
  // Mix_ResumeMusic()
  // Mix_PauseMusic()

  // Mix_FreeMusic()



  // Mix_LoadWAV()

  // MIX_MAX_VOLUME * procent

  // Mix_Volume()
  // Mix_PlayChannel()

  // Mix_FreeChunk()

  /*
  SDL_Delay(2000);
  

  Mix_Chunk* chunk;
  if(chunk_load(&chunk, "chunk-1.wav"))
  {
    fprintf(stdout, "Loaded chunk\n");

    int volume = (int) ((float) MIX_MAX_VOLUME * 0.5f);
    Mix_Volume(-1, volume);

    fprintf(stdout, "Sat the volume to: %d\n", volume);

    fprintf(stdout, "Playing chunk!\n");
    
    if(Mix_PlayChannel(-1, chunk, 1) == -1)
    {
      fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
    }

    Mix_FreeChunk(chunk);
  }

  SDL_Delay(2000);
  // usleep(10000000);
  */

  sdl_drivers_quit();
  
  return 0;
}
