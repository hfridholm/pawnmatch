#include "../logic.h"
#include "../screen.h"

extern Square screen_pixels_square(SDL_Rect boardRect, int width, int height);

extern Move create_move(U64 boards[12], Square sourceSquare, Square targetSquare);

extern SDL_Rect board_rect(int screenWidth, int screenHeight);
 
extern void screen_board_textures_create(ScreenBoard* board, SDL_Renderer* renderer, Position position);

extern void screen_board_square_mark(ScreenBoard* board, SDL_Renderer* renderer, Square square);

extern void screen_board_arrow_create(ScreenBoard* board, SDL_Renderer* renderer, Square source, Square target);

extern void screen_board_piece_ungrab(ScreenBoard* board);

extern void screen_board_move_make(ScreenBoard* board, SDL_Renderer* renderer, Position* position, Move move);

extern void screen_board_piece_grab(ScreenBoard* board, SDL_Renderer* renderer, Position position, Square square);

extern void screen_board_piece_release(ScreenBoard* board, SDL_Renderer* renderer, Position position);

bool pixels_inside_rect(SDL_Rect rect, int width, int height)
{
  return (width >= rect.x && width <= (rect.x + rect.w) && height >= rect.y && height <= (rect.y + rect.h));
}

void screen_board_up_right_handler(ScreenBoard* board, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(board->rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  if(board->meta.rightHoldingSquare != SQUARE_NONE && square != SQUARE_NONE)
  {
    if(board->meta.rightHoldingSquare == square)
    {
      screen_board_square_mark(board, renderer, square);
    }
    else screen_board_arrow_create(board, renderer, board->meta.rightHoldingSquare, square);
  }

  board->meta.rightHoldingSquare = SQUARE_NONE;
}

void screen_board_down_right_handler(ScreenBoard* board, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  screen_board_piece_ungrab(board);

  Square square = screen_pixels_square(board->rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  board->meta.rightHoldingSquare = square;
}

void screen_board_up_left_handler(ScreenBoard* board, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(board->rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;

  if(board->meta.grabbedSquare != SQUARE_NONE && square != board->meta.grabbedSquare)
  {
    // Try to make the move (grabbedSquare -> square);
    Move move = create_move(position->boards, board->meta.grabbedSquare, square);

    if(move_fully_legal(*position, move))
    {
      screen_board_move_make(board, renderer, position, move);
    }
  }

  // If you are already holding a piece and release it on the same square,
  // stop holding it
  if(board->meta.markedSquare != SQUARE_NONE && board->meta.markedSquare == square)
  {
    screen_board_piece_ungrab(board);
  }
  else if(square != board->meta.markedSquare && board->meta.grabbedSquare != SQUARE_NONE)
  {
    board->meta.markedSquare = board->meta.grabbedSquare;
  }

  if(board->meta.holdingPiece != PIECE_NONE)
  {
    screen_board_piece_release(board, renderer, *position);
  }
}

// When the mouse is clicked:
// - all marks disappear
void screen_board_down_left_handler(ScreenBoard* board, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  Square square = screen_pixels_square(board->rect, event.button.x, event.button.y);

  // If the user has clicked outside of the board, quit
  if(square == SQUARE_NONE) return;


  board->meta.markedSquaresBoard = 0ULL;

  texture_destroy(&board->textures.marks);

  memset(board->meta.arrows, 0, sizeof(board->meta.arrows));

  texture_destroy(&board->textures.arrows);


  // If a square is marked and the user clicks on another square, try to make that move
  if(board->meta.markedSquare != SQUARE_NONE && board->meta.markedSquare != square)
  {
    // Try to make the move (markedSquare -> square);
    Move move = create_move(position->boards, board->meta.markedSquare, square);

    if(move_fully_legal(*position, move))
    {
      screen_board_move_make(board, renderer, position, move);

      return;
    }
  }

  // Here: If the user cant make a move or it was an illegal move

  if(BOARD_SQUARE_GET(position->covers[SIDE_BOTH], square)) 
  {
    screen_board_piece_grab(board, renderer, *position, square);
  }
  else screen_board_piece_ungrab(board);
}

void screen_menu_board_event_mouse_down_left_handler(ScreenMenuBoard* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(menu->board.rect, event.button.x, event.button.y))
  {
    screen_board_down_left_handler(&menu->board, renderer, position, event);
  }
}

void screen_menu_event_mouse_down_left_handler(ScreenMenu* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  switch(menu->type)
  {
    case SMT_MAIN:
      break;

    case SMT_ENGINE:
      break;

    case SMT_PLAYER:
      break;

    case SMT_BOARD:
      screen_menu_board_event_mouse_down_left_handler((ScreenMenuBoard*) menu, renderer, position, event);
      break;
  }
}

void screen_menu_board_event_mouse_down_right_handler(ScreenMenuBoard* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(menu->board.rect, event.button.x, event.button.y))
  {
    screen_board_down_right_handler(&menu->board, renderer, position, event);
  }
}

void screen_menu_event_mouse_down_right_handler(ScreenMenu* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  switch(menu->type)
  {
    case SMT_MAIN:
      break;

    case SMT_ENGINE:
      break;

    case SMT_PLAYER:
      break;

    case SMT_BOARD:
      screen_menu_board_event_mouse_down_right_handler((ScreenMenuBoard*) menu, renderer, position, event);
      break;
  }
}

void screen_event_mouse_down_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.button.button)
  {
    case SDL_BUTTON_LEFT:
      screen_menu_event_mouse_down_left_handler(screen->menu, screen->renderer, position, event);
      break;

    case SDL_BUTTON_RIGHT:
      screen_menu_event_mouse_down_right_handler(screen->menu, screen->renderer, position, event);
      break;
  }
}

void screen_menu_board_event_mouse_up_left_handler(ScreenMenuBoard* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(menu->board.rect, event.button.x, event.button.y))
  {
    screen_board_up_left_handler(&menu->board, renderer, position, event);
  }
}

void screen_menu_event_mouse_up_left_handler(ScreenMenu* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  switch(menu->type)
  {
    case SMT_MAIN:
      break;

    case SMT_ENGINE:
      break;

    case SMT_PLAYER:
      break;

    case SMT_BOARD:
      screen_menu_board_event_mouse_up_left_handler((ScreenMenuBoard*) menu, renderer, position, event);
      break;
  }
}

void screen_menu_board_event_mouse_up_right_handler(ScreenMenuBoard* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  if(pixels_inside_rect(menu->board.rect, event.button.x, event.button.y))
  {
    screen_board_up_right_handler(&menu->board, renderer, position, event);
  }
}

void screen_menu_event_mouse_up_right_handler(ScreenMenu* menu, SDL_Renderer* renderer, Position* position, SDL_Event event)
{
  switch(menu->type)
  {
    case SMT_MAIN:
      break;

    case SMT_ENGINE:
      break;

    case SMT_PLAYER:
      break;

    case SMT_BOARD:
      screen_menu_board_event_mouse_up_right_handler((ScreenMenuBoard*) menu, renderer, position, event);
      break;
  }
}

void screen_event_mouse_up_handler(Screen* screen, Position* position, SDL_Event event)
{
  switch(event.button.button)
  {
    case SDL_BUTTON_LEFT:
      screen_menu_event_mouse_up_left_handler(screen->menu, screen->renderer, position, event);
      break;

    case SDL_BUTTON_RIGHT:
      screen_menu_event_mouse_up_right_handler(screen->menu, screen->renderer, position, event);
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
  screen->mouse.x = event.motion.x;
  screen->mouse.y = event.motion.y;
}

// Updates the textures for the board menu (over the board)
void screen_menu_board_update(Screen* screen, Position position)
{
  ScreenMenuBoard* menu = (ScreenMenuBoard*) screen->menu;

  menu->board.rect = board_rect(screen->width, screen->height);

  screen_board_textures_create(&menu->board, screen->renderer, position);

  background_texture_create(&menu->menu.background, screen->renderer, screen->width, screen->height);
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

  // Update menu
  screen_menu_board_update(screen, *position);
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
