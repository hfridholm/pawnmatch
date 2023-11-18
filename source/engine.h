#ifndef ENGINE_H
#define ENGINE_H

#include "logic.h"
#include "debug.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

extern Move uci_bestmove_parse(char bestmoveString[]);

extern bool engine_setup(int* sockfd, const char address[], int port);

extern void engine_close(int* sockfd);

extern bool engine_read(int sockfd, char* string, size_t size);

extern bool engine_write(int sockfd, const char string[]);

extern char* uci_position_string(char* positionString, Position position);

extern Move uci_bestmove_parse(char bestmoveString[]);

extern char* move_string(char* moveString, Move move);

#endif // ENGINE_H
