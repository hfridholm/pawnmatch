#ifndef CONSOLE_H
#define CONSOLE_h

#include "logic.h"
#include "debug.h"

#include <ncurses.h>

extern void position_print(Position position);

extern bool stdin_string(char* string, const char prompt[]);

#endif
