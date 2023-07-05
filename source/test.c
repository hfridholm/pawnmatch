#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  char string[256];
  memset(string, 0, sizeof(string));

  while(scanf("%[^\n]%*c", string) == 1)
  {
    usleep(100000);

    printf("%s\n", string);
  }

  return 0;
}