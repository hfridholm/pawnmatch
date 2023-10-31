#include "../screen.h"

extern bool render_target_texture_setup(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);

extern bool render_target_texture_render(SDL_Texture** texture, SDL_Renderer* renderer);

extern bool texture_pixels_render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);

const float BACKGROUND_SCALE_FACTOR = 10.0;

int BACKGROUND_SQUARE_WIDTH;
int BACKGROUND_SQUARE_HEIGHT;

SDL_Texture* BACKGROUND_SQUARE_TEXTURES[9];

void background_base_textures_destroy(void)
{
  for(int index = 0; index < 9; index++)
  {
    texture_destroy(&BACKGROUND_SQUARE_TEXTURES[index]);
  }
  info_print("Destroyed background base textures");
}

bool background_base_textures_load(SDL_Renderer* renderer)
{
  SDL_Texture* backgroundTexture;

  if(!image_texture_load(&backgroundTexture, renderer, "../source/screen/images/background.png")) return false;

  int width = 0, height = 0;

  if(!texture_width_height(backgroundTexture, &width, &height)) return false;

  int squareWidth = (width / 3);
  int squareHeight = (height / 3);

  for(int index = 0; index < 9; index++)
  {
    SDL_Rect srcrect = { (index % 3) * squareWidth, (index / 3) * squareHeight, squareWidth, squareHeight};

    texture_rect_texture(&BACKGROUND_SQUARE_TEXTURES[index], renderer, backgroundTexture, srcrect);
  }

  texture_destroy(&backgroundTexture);

  BACKGROUND_SQUARE_WIDTH = squareWidth * BACKGROUND_SCALE_FACTOR;
  BACKGROUND_SQUARE_HEIGHT = squareHeight * BACKGROUND_SCALE_FACTOR;

  info_print("Created background base textures");

  return true;
}

bool background_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height)
{
  if(!render_target_texture_setup(texture, renderer, width, height)) return false;

  for(int textureHeight = BACKGROUND_SQUARE_HEIGHT; textureHeight <= (height - BACKGROUND_SQUARE_HEIGHT); textureHeight += BACKGROUND_SQUARE_HEIGHT)
  {
    for(int textureWidth = BACKGROUND_SQUARE_WIDTH; textureWidth <= (width - BACKGROUND_SQUARE_WIDTH); textureWidth += BACKGROUND_SQUARE_WIDTH)
    {
      texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[4], textureWidth, textureHeight, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);
    }
  }

  for(int textureHeight = BACKGROUND_SQUARE_HEIGHT; textureHeight <= (height - BACKGROUND_SQUARE_HEIGHT); textureHeight += BACKGROUND_SQUARE_HEIGHT)
  {
    texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[3], 0, textureHeight, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);
    
    texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[5], width - BACKGROUND_SQUARE_WIDTH, textureHeight, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);
  }

  for(int textureWidth = BACKGROUND_SQUARE_WIDTH; textureWidth <= (width - BACKGROUND_SQUARE_WIDTH); textureWidth += BACKGROUND_SQUARE_WIDTH)
  {
    texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[1], textureWidth, 0, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);
    
    texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[7], textureWidth, height - BACKGROUND_SQUARE_HEIGHT, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);
  }
  
  texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[0], 0, 0, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);

  texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[2], width - BACKGROUND_SQUARE_WIDTH, 0, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);

  texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[6], 0, height - BACKGROUND_SQUARE_HEIGHT, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);

  texture_pixels_render(renderer, BACKGROUND_SQUARE_TEXTURES[8], width - BACKGROUND_SQUARE_WIDTH, height - BACKGROUND_SQUARE_HEIGHT, BACKGROUND_SQUARE_WIDTH, BACKGROUND_SQUARE_HEIGHT);

  return render_target_texture_render(texture, renderer);
}
