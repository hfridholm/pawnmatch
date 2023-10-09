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

bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect)
{
  if(SDL_RenderCopy(renderer, texture, NULL, &destRect) != 0)
  {
    fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());

    return false;
  }
  return true;
}

void sdl_drivers_quit()
{
  // SDL_WasInit
  SDL_Quit();

  IMG_Quit();

  TTF_Quit();

  Mix_CloseAudio();

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

int main(int argc, char* argv[])
{
  if(!sdl_drivers_init())
  {
    sdl_drivers_quit();
    fprintf(stdout, "Failed to initialize SDL drivers\n");
    
    return 1;
  }
  else fprintf(stdout, "Initialized SDL Drivers\n");


  Screen screen;

  if(screen_create(&screen, 500, 500, "PawnMatch"))
  {
    SDL_Texture* texture;

    if(image_texture_load(&texture, screen.renderer, "../source/screen/images/black-pawn.png"))
    {
      fprintf(stdout, "Loaded image!\n");

      SDL_RenderClear(screen.renderer);

      // SDL_Rect destRect = {10, 10, screen.width - 10, screen.height - 10};
      SDL_Rect destRect = board_rect_get(screen.width, screen.height);

      texture_rect_render(screen.renderer, texture, destRect);

      SDL_RenderPresent(screen.renderer);


      SDL_Event event;

      do
      {
        if(!SDL_WaitEvent(&event))
        {
          fprintf(stderr, "SDL_WaitEvent: %s\n", SDL_GetError());
        }

        /*
        switch(event.type)
        {
          case SDL_WINDOWEVENT:
            break;

          case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
              case SDLK_SPACE:
                fprintf(stdout, "Space!\n");
                break;
            }
            break;

          case SDL_KEYUP:
            break;

          case SDL_MOUSEBUTTONDOWN:
            break;

          case SDL_MOUSEBUTTONUP:
            break;
        }
        */

        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
          if(event.button.button == SDL_BUTTON_LEFT)
          {
            fprintf(stdout, "Moving at %d, %d\n", event.button.x, event.button.y);
          }
          else if(event.button.button == SDL_BUTTON_RIGHT)
          {
            fprintf(stdout, "Marking at %d, %d\n", event.button.x, event.button.y);
          }
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
          SDL_Rect destRect = board_rect_get(screen.width, screen.height);

          texture_rect_render(screen.renderer, texture, destRect);

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


      fprintf(stdout, "Destroyed texture!\n");

      SDL_DestroyTexture(texture);
    }
  }

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
