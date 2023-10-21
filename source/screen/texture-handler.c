#include "../screen.h"

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
