#include "header.h"

/*
engine vs engine
engine vs player
player vs engine
player vs player
*/

typedef struct
{
  char name[64];
  char author[64];
  int stdin;
  int stdout;
} Engine;

Move MOVE_NONE = 0;

U64 BOARD_LOOKUP_LINES[BOARD_SQUARES][BOARD_SQUARES];

U64 create_board_line(Square source, Square target)
{
  U64 board = 0ULL;

  int sourceRank = (source / BOARD_FILES);
  int sourceFile = (source % BOARD_FILES);

  int targetRank = (target / BOARD_FILES);
  int targetFile = (target % BOARD_FILES);

  int rankOffset = (targetRank - sourceRank);
  int fileOffset = (targetFile - sourceFile);

  int rankFactor = (rankOffset > 0) ? +1 : -1;
  int fileFactor = (fileOffset > 0) ? +1 : -1;

  int absRankOffset = (rankOffset * rankFactor);
  int absFileOffset = (fileOffset * fileFactor);

  // If the move is not diagonal nor straight, return empty board;
  if(!(absRankOffset == absFileOffset) && !((absRankOffset == 0) ^ (absFileOffset == 0))) return 0ULL;

  int rankScalor = (rankOffset == 0) ? 0 : rankFactor;
  int fileScalor = (fileOffset == 0) ? 0 : fileFactor;

  for(int rank = sourceRank, file = sourceFile; (rank != targetRank || file != targetFile); rank += rankScalor, file += fileScalor)
  {
    Square square = (rank * BOARD_FILES) + file;

    if(square == source || square == target) continue;

    board = BOARD_SQUARE_SET(board, square);
  }
  return board;
}

void init_board_lookup_lines()
{
  for(Square sourceSquare = A8; sourceSquare <= H1; sourceSquare++)
  {
    for(Square targetSquare = A8; targetSquare <= H1; targetSquare++)
    {
      U64 boardLines = create_board_line(sourceSquare, targetSquare);

      BOARD_LOOKUP_LINES[sourceSquare][targetSquare] = boardLines;
    }
  }
}

bool stdin_string(char* string, const char prompt[])
{
  fflush(stdin);
  printf("%s", prompt);
  char buffer[1024];
  if(fgets(buffer, sizeof(buffer), stdin) == NULL) return false;
  return sscanf(buffer, "%[^\n]%*c", string);
}

Square parse_square(const char squareString[])
{
  int file = squareString[0] - 'a';
  int rank = BOARD_RANKS - (squareString[1] - '0');

  if(!(file >= 0 && file < BOARD_FILES) || !(rank >= 0 && rank < BOARD_RANKS)) return SQUARE_NONE;

  return (rank * BOARD_FILES) + file;
}

Move parse_move(const char moveString[])
{
  Move parseMove = 0;

  Square sourceSquare = parse_square(moveString += 0);
  if(sourceSquare == SQUARE_NONE) return 0;

  Square targetSquare = parse_square(moveString += 2);
  if(targetSquare == SQUARE_NONE) return 0;

  parseMove |= MOVE_SET_SOURCE(sourceSquare);
  parseMove |= MOVE_SET_TARGET(targetSquare);

  Piece promotePiece = SYMBOL_PIECES[(unsigned char) *moveString];

  if(promotePiece != PIECE_WHITE_PAWN) parseMove |= MOVE_SET_PROMOTE(promotePiece);

  return parseMove;
}

char* move_string(char* moveString, Move move)
{
  const char* sourceString = SQUARE_STRINGS[MOVE_GET_SOURCE(move)];
  const char* targetString = SQUARE_STRINGS[MOVE_GET_TARGET(move)];

  Piece promotePiece = MOVE_GET_PROMOTE(move);

  if(promotePiece == PIECE_WHITE_PAWN) sprintf(moveString, "%s%s", sourceString, targetString);

  else sprintf(moveString, "%s%s%c", sourceString, targetString, PIECE_SYMBOLS[promotePiece]);

  return moveString;
}

void init_all()
{
  init_piece_lookup_masks();

  init_bishop_rook_relevant_bits();

  init_piece_lookup_attacks();

  init_board_lookup_lines();
}

const char FEN_START[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void position_string(char* positionString, const char fenString[], MoveArray moveArray)
{
  sprintf(positionString, "position fen %s", fenString);

  if(moveArray.amount > 0)
  {
    strcat(positionString, " moves");
  }

  for(int index = 0; index < moveArray.amount; index++)
  {
    char moveString[8];
    move_string(moveString, moveArray.moves[index]);

    strcat(positionString, " ");

    strcat(positionString, moveString);
  }
}

bool write_engine(Engine engine, const char string[])
{
  char writeChar;
  for(int index = 0; index < strlen(string); index++)
  {
    writeChar = string[index];

    if(writeChar == '\n' || writeChar == '\0') break;

    if(write(engine.stdin, &writeChar, sizeof(writeChar)) < 1) return false;
  }

  writeChar = '\n';
  if(write(engine.stdin, &writeChar, sizeof(writeChar)) < 1) return false;

  return true;
}

bool read_engine(char* string, Engine engine)
{
  char readChar;
  int index = 0;
  int readReturn;

  while((readReturn = read(engine.stdout, &readChar, sizeof(readChar))) == 1)
  {
    if(readChar == '\n' || readChar == '\0') break;

    string[index++] = readChar;
  }
  return (readReturn != -1 && index > 0);
}

Move move_engine(Engine engine, const char fenString[], MoveArray moveArray)
{
  if(!write_engine(engine, "isready"))
  {
    printf("Could not write to engine\n");
    return MOVE_NONE;
  }

  char readString[1024];
  
  do 
  {
    memset(readString, 0, sizeof(readString));

    if(!read_engine(readString, engine))
    {
      printf("Can not read from engine\n");
      return MOVE_NONE;
    }
    
    printf("Engine: (%s)\n", readString);
  }
  while(strcmp(readString, "readyok"));


  char positionString[1024];
  memset(positionString, 0, sizeof(positionString));

  position_string(positionString, fenString, moveArray);

  if(!write_engine(engine, positionString))
  {
    printf("Could not write to engine\n");
    return MOVE_NONE;
  }

  if(!write_engine(engine, "go"))
  {
    printf("Could not write to engine\n");
    return MOVE_NONE;
  }


  do 
  {
    memset(readString, 0, sizeof(readString));

    if(!read_engine(readString, engine))
    {
      printf("Can not read from engine\n");
      return MOVE_NONE;
    }
    
    printf("Engine: (%s)\n", readString);
  }
  while(strncmp(readString, "bestmove", 8));


  return parse_move(readString + 9);
}

Move move_player(Position position)
{
  /*
  print the position

  while input is something and not legal move:
    input string move from user
  */
  return MOVE_NONE;
}

bool position_alive(Position position)
{
  return true;
}

void player_vs_player(void)
{
  /*
  while game still going on:
    if side is white:
      input move from player white

    else:
      input move from player black
  */
}

void player_vs_engine(void)
{
  Engine engine = {"Engine Name", "Engine Authour", 1, 0};

  if(!write_engine(engine, "uci"))
  {
    printf("Could not write to engine\n");
    return;
  }

  char readString[1024];
  
  do 
  {
    memset(readString, 0, sizeof(readString));

    if(!read_engine(readString, engine))
    {
      printf("Can not read from engine\n");
      return;
    }
    
    printf("Engine: (%s)\n", readString);
  }
  while(strcmp(readString, "uciok"));


  if(!write_engine(engine, "ucinewgame"))
  {
    printf("Could not write to engine\n");
    return;
  }



  

  Position position;
  parse_fen(&position, FEN_START);

  MoveArray moveArray;

  moveArray.amount = 0;
  moveArray.moves[0] = 0;


  parse_fen(&position, FEN_START);

  while(position_alive(position))
  {
    if(position.side == SIDE_WHITE)
    {
      Move move = move_player(position);

      if(move == MOVE_NONE) // The player stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
    else
    {
      Move move = move_engine(engine, FEN_START, moveArray);

      if(move == MOVE_NONE) // The engine stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
  }



  if(!write_engine(engine, "quit"))
  {
    printf("Could not write to engine\n");
    return;
  }
}

bool engine_connect(Engine* engine)
{
  if(!write_engine(*engine, "uci"))
  {
    printf("Could not write to engine\n");
    return false;
  }

  char readString[1024];
  
  do 
  {
    memset(readString, 0, sizeof(readString));

    if(!read_engine(readString, *engine))
    {
      printf("Can not read from engine\n");
      return false;
    }
    
    printf("Engine: (%s)\n", readString);
  }
  while(strcmp(readString, "uciok"));

  if(!write_engine(*engine, "ucinewgame"))
  {
    printf("Could not write to engine\n");
    return false;
  }

  return true;
}

void engine_vs_player(void)
{
  Engine engine = {"Engine Name", "Engine Authour", 1, 0};

  if(!engine_connect(&engine))
  {
    printf("Engine could not connect\n");
    return;
  }

  Position position;
  parse_fen(&position, FEN_START);

  MoveArray moveArray;

  moveArray.amount = 0;
  moveArray.moves[0] = 0;


  parse_fen(&position, FEN_START);

  while(position_alive(position))
  {
    if(position.side == SIDE_WHITE)
    {
      Move move = move_engine(engine, FEN_START, moveArray);

      if(move == MOVE_NONE) // The engine stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
    else
    {
      Move move = move_player(position);

      if(move == MOVE_NONE) // The player stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
  }



  if(!write_engine(engine, "quit"))
  {
    printf("Could not write to engine\n");
    return;
  }
}

void engine_close(Engine engine)
{
  close(engine.stdin);
  close(engine.stdout);
}

bool engine1_open(Engine* engine1)
{
  engine1->stdin = open("engine1-stdin", O_WRONLY);

  if(engine1->stdin == -1)
  {
    printf("Could not open engine1-stdin\n");

    return false;
  }

  engine1->stdout = open("engine1-stdout", O_RDONLY);

  if(engine1->stdout == -1)
  {
    printf("Could not open engine1-stdout\n");

    close(engine1->stdin);

    return false;
  }
  return true;
}

bool engine2_open(Engine* engine2)
{
  engine2->stdin = open("engine2-stdin", O_WRONLY);

  if(engine2->stdin == -1)
  {
    printf("Could not open engine2-stdin\n");

    return false;
  }

  engine2->stdout = open("engine2-stdout", O_RDONLY);

  if(engine2->stdout == -1)
  {
    printf("Could not open engine2-stdout\n");

    close(engine2->stdin);

    return false;
  }
  return true;
}

void engine_vs_engine(void)
{
  Engine engine1;
  if(!engine1_open(&engine1))
  {
    printf("Could not open engine1\n");
    return;
  }

  if(!engine_connect(&engine1))
  {
    printf("Engine1 could not connect\n");

    engine_close(engine1);
    return;
  }

  Engine engine2;
  if(!engine1_open(&engine2))
  {
    printf("Could not open engine2\n");

    engine_close(engine1);
    return;
  }

  if(!engine_connect(&engine2))
  {
    printf("Engine2 could not connect\n");

    engine_close(engine1);
    engine_close(engine2);
    return;
  }


  Position position;
  parse_fen(&position, FEN_START);

  MoveArray moveArray;

  moveArray.amount = 0;
  moveArray.moves[0] = 0;


  parse_fen(&position, FEN_START);

  while(position_alive(position))
  {
    if(position.side == SIDE_WHITE)
    {
      Move move = move_engine(engine1, FEN_START, moveArray);

      if(move == MOVE_NONE) // Engine1 stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
    else
    {
      Move move = move_engine(engine2, FEN_START, moveArray);

      if(move == MOVE_NONE) // Engine2 stopped playing
      {
        break;
      }

      make_move(&position, move);
    }
  }

  write_engine(engine1, "quit");
  write_engine(engine2, "quit");

  engine_close(engine1);
  engine_close(engine2);
}

int main(int argc, char* argv[])
{
  player_vs_player();
  /*
  const char matchString[] = "player vs player";

  player_vs_player();

  printf("Opening engine1-stdout...\n");

  int engine1Stdout = open("engine1-stdout", O_RDONLY);
  if(engine1Stdout == -1)
  {
    printf("Could not open engine1Stdout\n");
    return 1;
  }

  printf("Opened engine1-stdout\n");

  printf("Opening engine1-stdin...\n");

  int engine1Stdin = open("engine1-stdin", O_WRONLY);
  if(engine1Stdout == -1)
  {
    printf("Could not open engine1-stdin\n");

    close(engine1Stdout);

    return 2;
  }

  printf("Opened engine1-stdin\n");

  char inputBuffer[64];
  char outputBuffer[64];
  
  while(1)
  {
    memset(inputBuffer, 0, sizeof(inputBuffer));

    printf("To engine1-stdin: ");
    if(scanf("%[^\n]%*c", inputBuffer) < 1) break;

    printf("Writing to engine1-stdin:\n");

    char writeChar;

    for(int index = 0; index < sizeof(inputBuffer); index++)
    {
      writeChar = inputBuffer[index];

      // if(writeChar == '\n' || writeChar == '\0') break;

      if(writeChar == '\0') break;

      // printf("%c", writeChar);

      if(write(engine1Stdin, &writeChar, sizeof(writeChar)) < 1) break;
    }

    writeChar = '\n';

    printf("Writing a backslash\n");
    if(write(engine1Stdin, &writeChar, sizeof(writeChar)) < 1) break;

    printf("Written to engine1-stdin\n");

    memset(outputBuffer, 0, sizeof(outputBuffer));

    printf("Reading from engine1-stdout\n");

    char readChar;
    for(int index = 0; read(engine1Stdout, &readChar, sizeof(readChar)) > 0; index++)
    {
      if(readChar == '\n' || readChar == '\0') break;

      outputBuffer[index] = readChar;
    }

    printf("Read from engine1-stdout\n");

    printf("From engine1-stdout: (%s)\n", outputBuffer);
  }

  printf("Closing pipes\n");

  close(engine1Stdout);
  close(engine1Stdin);
  */

  return 0;
}