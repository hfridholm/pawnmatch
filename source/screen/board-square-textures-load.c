#include "../screen.h"

extern bool image_texture_load(SDL_Texture** texture, SDL_Renderer* renderer, const char filePath[]);

extern SDL_Rect board_square_rect(SDL_Rect boardRect, Square square);


SDL_Texture* WHITE_SQUARE_TEXTURE;
SDL_Texture* BLACK_SQUARE_TEXTURE;

SDL_Texture* PIECE_TEXTURES[12];

SDL_Texture* MOVED_SQUARE_TEXTURE;
SDL_Texture* MARK_SQUARE_TEXTURE;
SDL_Texture* CHECK_SQUARE_TEXTURE;
SDL_Texture* MOVE_SQUARE_TEXTURE;
SDL_Texture* HOVER_SQUARE_TEXTURE;

SDL_Texture* ARROW_HEAD_TEXTURE;
SDL_Texture* ARROW_BODY_TEXTURE;


const char* PIECE_TYPE_STRINGS[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};


bool piece_textures_load(SDL_Renderer* renderer)
{
  char filePath[128];

  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_WHITE_KING; piece++)
  {
    sprintf(filePath, "../source/screen/images/white-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  for(Piece piece = PIECE_BLACK_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    sprintf(filePath, "../source/screen/images/black-%s.png", PIECE_TYPE_STRINGS[piece % 6]);

    if(image_texture_load(&PIECE_TEXTURES[piece], renderer, filePath));
  }

  return true;
}

bool square_textures_load(SDL_Renderer* renderer)
{
  if(image_texture_load(&WHITE_SQUARE_TEXTURE, renderer, "../source/screen/images/white-square.png"));

  if(image_texture_load(&BLACK_SQUARE_TEXTURE, renderer, "../source/screen/images/black-square.png"));

  return true;
}

bool other_textures_load(SDL_Renderer* renderer)
{
  image_texture_load(&MOVED_SQUARE_TEXTURE, renderer, "../source/screen/images/moved-square.png");
  
  image_texture_load(&MARK_SQUARE_TEXTURE, renderer, "../source/screen/images/mark-square.png");
  
  image_texture_load(&MOVE_SQUARE_TEXTURE, renderer, "../source/screen/images/move-square.png");
  
  image_texture_load(&CHECK_SQUARE_TEXTURE, renderer, "../source/screen/images/check-square.png");
  
  image_texture_load(&HOVER_SQUARE_TEXTURE, renderer, "../source/screen/images/hover-square.png");

  /*
  image_texture_load(&ARROW_HEAD_TEXTURE, renderer, "../source/screen/images/check-square.png");
  
  image_texture_load(&ARROW_BODY_TEXTURE, renderer, "../source/screen/images/check-square.png");
  */

  return true;
}

void other_textures_destroy(void)
{
  texture_destroy(&MOVED_SQUARE_TEXTURE);

  texture_destroy(&MARK_SQUARE_TEXTURE);

  texture_destroy(&MOVE_SQUARE_TEXTURE);

  texture_destroy(&CHECK_SQUARE_TEXTURE);

  texture_destroy(&HOVER_SQUARE_TEXTURE);

  texture_destroy(&ARROW_HEAD_TEXTURE);

  texture_destroy(&ARROW_BODY_TEXTURE);
}

void square_textures_destroy()
{
  texture_destroy(&WHITE_SQUARE_TEXTURE);

  texture_destroy(&BLACK_SQUARE_TEXTURE);
}

void piece_textures_destroy()
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    texture_destroy(&PIECE_TEXTURES[piece]);
  }
}

bool board_square_textures_load(SDL_Renderer* renderer)
{
  square_textures_load(renderer);

  piece_textures_load(renderer);

  other_textures_load(renderer);

  info_print("Loaded board square textures");

  return true;
}

void board_square_textures_destroy()
{
  square_textures_destroy();

  piece_textures_destroy();

  other_textures_destroy();

  info_print("Destroyed board square textures");
}
