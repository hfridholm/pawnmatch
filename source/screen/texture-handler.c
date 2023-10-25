#include "../screen.h"

void texture_destroy(SDL_Texture** texture)
{
  SDL_DestroyTexture(*texture);

  *texture = NULL;
}

// Clears the texture beforehand
bool render_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height)
{
  texture_destroy(texture);

  // https://stackoverflow.com/questions/24241974/sdl2-generate-fully-transparent-texture

  *texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, width, height);

  if(*texture == NULL)
  {
    error_print("SDL_CreateTexture: %s", SDL_GetError());

    return false;
  }

  SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);

  return true;
}

bool render_target_texture_setup(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height)
{
  if(!render_texture_create(texture, renderer, width, height)) return false;

  if(SDL_SetRenderTarget(renderer, *texture) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }
  return true;
}

bool render_target_texture_render(SDL_Texture** texture, SDL_Renderer* renderer)
{
  if(SDL_SetRenderTarget(renderer, NULL) != 0)
  {
    error_print("SDL_SetRenderTarget: %s", SDL_GetError());

    texture_destroy(texture);

    return false;
  }

  SDL_RenderPresent(renderer);

  return true;
}

bool texture_rect_render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect destRect)
{
  if(texture == NULL || renderer == NULL) return false;

  if(SDL_RenderCopy(renderer, texture, NULL, &destRect) != 0)
  {
    error_print("SDL_RenderCopy: %s\n", SDL_GetError());

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
    error_print("img_load: %s", IMG_GetError());

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
    error_print("SDL_CreateTextureFromSurface: %s", SDL_GetError());

    return false;
  }
  return true;
}
