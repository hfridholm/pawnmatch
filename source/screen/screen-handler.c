#include "../screen.h"

// #include <stdarg.h>
// https://www.geeksforgeeks.org/variadic-functions-in-c/

void error_print(char* format, ...)
{
  fprintf(stderr, "ERROR: %s\n", format);
}

void info_print(char* format, ...)
{
  fprintf(stdout, "INFO: %s\n", format);
}

bool window_create(SDL_Window** window, int width, int height, const char title[])
{
  if((*window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)) == NULL)
  {
    error_print("SDL_CreateWindow: %s", SDL_GetError());

    return false;
  }
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

  // This forces the window to be at least this dimension
  // SDL_SetWindowMinimumSize(*window, 400, 400);

  return true;
}

void window_destroy(SDL_Window** window)
{
  SDL_DestroyWindow(*window);
  *window = NULL;
}

bool renderer_create(SDL_Renderer** renderer, SDL_Window* window)
{
  if((*renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
  {
    error_print("SDL_CreateRenderer: %s", SDL_GetError());

    return false;
  }
  return true;
}

void renderer_destroy(SDL_Renderer** renderer)
{
  SDL_DestroyRenderer(*renderer);
  *renderer = NULL;
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

  info_print("Created Screen");

  return true;
}

void screen_destroy(Screen* screen)
{
  renderer_destroy(&screen->renderer);

  window_destroy(&screen->window);

  info_print("Destroyed Screen");
}

void sdl_drivers_quit()
{
  Mix_CloseAudio();

  // SDL_WasInit
  SDL_Quit();

  IMG_Quit();

  TTF_Quit();

  Mix_Quit();

  info_print("Quitted SDL Drivers");
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

  info_print("Initiated SDL Drivers");

  return true;
}
