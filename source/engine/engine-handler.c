#include "../engine.h"

extern bool client_socket_create(int* sockfd, const char address[], int port);

extern bool socket_close(int* sockfd);

extern int socket_write(int sockfd, const char* buffer, size_t size);

extern int socket_read(int sockfd, char* buffer, size_t size);

bool engine_write(int sockfd, const char string[])
{
  char buffer[strlen(string) + 1];

  sprintf(buffer, "%s\n", string);

  return socket_write(sockfd, buffer, sizeof(buffer));
}

bool engine_read(int sockfd, char* string, size_t size)
{
  char buffer[size + 1];

  memset(buffer, '\0', sizeof(buffer));

  if(!socket_read(sockfd, buffer, sizeof(buffer))) return false;

  strncpy(string, buffer, strlen(buffer) - 1);

  return true;
}

bool engine_connect(int sockfd)
{
  if(!engine_write(sockfd, "uci")) return false;

  char buffer[1024];
  do 
  {
    memset(buffer, '\0', sizeof(buffer));

    if(!engine_read(sockfd, buffer, sizeof(buffer))) return false;
  }
  while(strcmp(buffer, "uciok"));

  if(!engine_write(sockfd, "ucinewgame")) return false;

  return true;
}

void engine_close(int* sockfd)
{
  engine_write(*sockfd, "quit");

  // Let the reciever get the message
  usleep(100000);

  socket_close(sockfd);
}

bool engine_setup(int* sockfd, const char address[], int port)
{
  if(!client_socket_create(sockfd, address, port)) return false;

  if(!engine_connect(*sockfd))
  {
    socket_close(sockfd);

    return false;
  }
  return true;
}
