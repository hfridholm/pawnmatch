#include "../engine.h"
#include "../screen.h"

extern Square screen_pixels_square(SDL_Rect boardRect, int width, int height);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

extern SDL_Rect board_rect(int screenWidth, int screenHeight);
 
extern void screen_board_textures_create(ScreenBoardTextures* boardTextures, Screen screen, Position position);

void screen_board_piece_release(Screen* screen, Position position)
{
  screen->board.meta.holdingPiece = PIECE_NONE;

  pieces_texture_create(&screen->board.textures.pieces, screen->renderer, screen->board.rect.w, screen->board.rect.h, position, SQUARE_NONE);
}

void screen_board_square_mark(Screen* screen, Square square)
{
  if(BOARD_SQUARE_GET(screen->board.meta.markedSquaresBoard, square))
  {
    screen->board.meta.markedSquaresBoard = BOARD_SQUARE_POP(screen->board.meta.markedSquaresBoard, square);
  }
  else screen->board.meta.markedSquaresBoard = BOARD_SQUARE_SET(screen->board.meta.markedSquaresBoard, square);

  marks_texture_create(&screen->board.textures.marks, screen->renderer, screen->board.rect.w, screen->board.rect.h, screen->board.meta.markedSquaresBoard);
}

void screen_board_arrow_create(Screen* screen, Square source, Square target)
{
  if(source < 0 || source >= BOARD_SQUARES) return;
  if(target < 0 || target >= BOARD_SQUARES) return;

  if(BOARD_SQUARE_GET(screen->board.meta.arrows[source], target))
  {
    screen->board.meta.arrows[source] = BOARD_SQUARE_POP(screen->board.meta.arrows[source], target);
  }
  else screen->board.meta.arrows[source] = BOARD_SQUARE_SET(screen->board.meta.arrows[source], target);

  arrows_texture_create(&screen->board.textures.arrows, screen->renderer, screen->board.rect.w, screen->board.rect.h, screen->board.meta.arrows);
}

void screen_board_piece_ungrab(Screen* screen)
{
  screen->board.meta.markedSquare = SQUARE_NONE;
  screen->board.meta.grabbedSquare = SQUARE_NONE;
  
  texture_destroy(&screen->board.textures.moves);
}

void screen_move_make(Screen* screen, Position* position, Move move)
{
  make_move(position, move);

  screen_board_piece_ungrab(screen);

  // Update the check square texture
  check_texture_create(&screen->board.textures.check, screen->renderer, screen->board.rect.w, screen->board.rect.h, *position);

  // Update the pieces texture
  pieces_texture_create(&screen->board.textures.pieces, screen->renderer, screen->board.rect.w, screen->board.rect.h, *position, SQUARE_NONE);
}

void screen_board_piece_grab(Screen* screen, Position position, Square square)
{
  Piece piece = boards_square_piece(position.boards, square);

  // Assign grabbed and holding piece
  screen->board.meta.grabbedSquare = square;
  screen->board.meta.holdingPiece = piece;

  // Update pieces texture to remove the holding piece
  pieces_texture_create(&screen->board.textures.pieces, screen->renderer, screen->board.rect.w, screen->board.rect.h, position, screen->board.meta.grabbedSquare);

  // Create the texture that displays valid moves
  moves_texture_create(&screen->board.textures.moves, screen->renderer, screen->board.rect.w, screen->board.rect.h, position, screen->board.meta.grabbedSquare);
}

bool pixels_inside_rect(SDL_Rect rect, int width, int height)
{
  return (width >= rect.x && width <= (rect.x + rect.w) && height >= rect.y && height <= (rect.y + rect.h));
}

void screen_board_up_right_handler(Screen* screen, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(screen->board.rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  if(screen->board.meta.rightHoldingSquare != SQUARE_NONE && square != SQUARE_NONE)
  {
    if(screen->board.meta.rightHoldingSquare == square)
    {
      screen_board_square_mark(screen, square);
    }
    else screen_board_arrow_create(screen, screen->board.meta.rightHoldingSquare, square);
  }

  screen->board.meta.rightHoldingSquare = SQUARE_NONE;
}

void screen_board_down_right_handler(Screen* screen, Position* position, SDL_Event event)
{
  screen_board_piece_ungrab(screen);

  Square square = screen_pixels_square(screen->board.rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  screen->board.meta.rightHoldingSquare = square;
}

void screen_board_up_left_handler(Screen* screen, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(screen->board.rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  if(screen->board.meta.grabbedSquare != SQUARE_NONE && square != screen->board.meta.grabbedSquare)
  {
    // Try to make the move (grabbedSquare -> square);
    Move move = create_move(position->boards, screen->board.meta.grabbedSquare, square);

    if(move_fully_legal(*position, move))
    {
      screen_move_make(screen, position, move);
    }
  }

  // If you are already holding a piece and release it on the same square,
  // stop holding it
  if(screen->board.meta.markedSquare != SQUARE_NONE && screen->board.meta.markedSquare == square)
  {
    screen_board_piece_ungrab(screen);
  }
  else if(square != screen->board.meta.markedSquare && screen->board.meta.grabbedSquare != SQUARE_NONE)
  {
    screen->board.meta.markedSquare = screen->board.meta.grabbedSquare;
  }

  if(screen->board.meta.holdingPiece != PIECE_NONE)
  {
    screen_board_piece_release(screen, *position);
  }
}

// When the mouse is clicked:
// - all marks disappear
void screen_board_down_left_handler(Screen* screen, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(screen->board.rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;


  screen->board.meta.markedSquaresBoard = 0ULL;

  texture_destroy(&screen->board.textures.marks);

  memset(screen->board.meta.arrows, 0, sizeof(screen->board.meta.arrows));

  texture_destroy(&screen->board.textures.arrows);


  // If a square is marked and the user clicks on another square, try to make that move
  if(screen->board.meta.markedSquare != SQUARE_NONE && screen->board.meta.markedSquare != square)
  {
    // Try to make the move (markedSquare -> square);
    Move move = create_move(position->boards, screen->board.meta.markedSquare, square);

    if(move_fully_legal(*position, move))
    {
      screen_move_make(screen, position, move);

      return;
    }
  }

  // Here: If the user cant make a move or it was an illegal move

  if(BOARD_SQUARE_GET(position->covers[SIDE_BOTH], square)) 
  {
    screen_board_piece_grab(screen, *position, square);
  }
  else screen_board_piece_ungrab(screen);
}

void screen_event_mouse_down_left_handler(Screen* screen, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(screen->board.rect, event.button.x, event.button.y))
  {
    screen_board_down_left_handler(screen, position, event);
  }
}

void screen_event_mouse_down_right_handler(Screen* screen, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(screen->board.rect, event.button.x, event.button.y))
  {
    screen_board_down_right_handler(screen, position, event);
  }
}

void screen_event_mouse_down_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.button.button)
  {
    case SDL_BUTTON_LEFT:
      screen_event_mouse_down_left_handler(screen, position, event);
      break;

    case SDL_BUTTON_RIGHT:
      screen_event_mouse_down_right_handler(screen, position, event);
      break;
  }
}

void screen_event_mouse_up_left_handler(Screen* screen, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(screen->board.rect, event.button.x, event.button.y))
  {
    screen_board_up_left_handler(screen, position, event);
  }
}

void screen_event_mouse_up_right_handler(Screen* screen, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(screen->board.rect, event.button.x, event.button.y))
  {
    screen_board_up_right_handler(screen, position, event);
  }
}

void screen_event_mouse_up_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.button.button)
  {
    case SDL_BUTTON_LEFT:
      screen_event_mouse_up_left_handler(screen, position, event);
      break;

    case SDL_BUTTON_RIGHT:
      screen_event_mouse_up_right_handler(screen, position, event);
      break;
  }
}

void music_toggle()
{
  fprintf(stdout, "Toggled music on or off\n");
}

void screen_event_key_down_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.key.keysym.sym)
  {
    case SDLK_SPACE:
      music_toggle();
      break;
  }
}

void screen_event_mouse_motion_handler(Screen* screen, Position* position, SDL_Event event)
{
  screen->mouseX = event.motion.x;
  screen->mouseY = event.motion.y;
}

// When the window resizes:
// - change screen dimensions
// - update screen rects
// - update every texture
void screen_event_window_resize_handler(Screen* screen, Position* position, SDL_Event event)
{
  screen->width = event.window.data1;
  screen->height = event.window.data2;

  // SDL_SetWindowSize(screen->window, screen->width, screen->height);

  screen->board.rect = board_rect(screen->width, screen->height);

  screen_board_textures_create(&screen->board.textures, *screen, *position);

  background_texture_create(&screen->backgroundTexture, screen->renderer, screen->width, screen->height);
}

void screen_event_window_handler(Screen* screen, Position* position, SDL_Event event)
{
  if(event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
  {
    screen_event_window_resize_handler(screen, position, event);
  }
}

void screen_event_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.type)
  {
    case SDL_MOUSEBUTTONDOWN:
      screen_event_mouse_down_handler(screen, position, event);
      break;

    case SDL_MOUSEBUTTONUP:
      screen_event_mouse_up_handler(screen, position, event);
      break;

    case SDL_KEYDOWN:
      screen_event_key_down_handler(screen, position, event);
      break;

    case SDL_MOUSEMOTION:
      screen_event_mouse_motion_handler(screen, position, event);
      break;

    case SDL_WINDOWEVENT:
      screen_event_window_handler(screen, position, event);
      break;
  }
}
