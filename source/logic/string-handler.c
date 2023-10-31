#include "../logic.h"

bool split_string_delim(char (*stringArray)[128], const char string[], int length, const char delim[], int amount)
{
  if(amount < 1) return false;

  char stringCopy[length + 1]; 
  strcpy(stringCopy, string);

  char* stringToken = NULL;
  
  if((stringToken = strtok(stringCopy, delim)) == NULL) return false;

  for(int index = 0; index < amount; index += 1)
  {
    strcpy(stringArray[index], stringToken);

    if(index == (amount - 1)) stringToken = strtok(NULL, delim);

    else if((stringToken = strtok(NULL, delim)) == NULL) return false;
  }
  return true;
}

bool merge_string_delim(char* string, char* stringArray[], int amount, const char delim[])
{
  if(amount < 1) return false;

  for(int index = 0; index < amount; index += 1)
  {
    strcat(string, stringArray[index]);

    if(index < (amount - 1)) strcat(string, delim);
  }
  return true;
}

int string_symbol_index(const char string[], int length, char symbol)
{
  for(int index = 0; index < length; index += 1)
  {
    if(symbol == string[index]) return index;
  }
  return -1;
}

bool parse_token_quotes(char* result, const char string[], int length, const char token[], const char quotes[])
{
  char stringCopy[length + 1]; 
  strcpy(stringCopy, string);

  char* stringPoint;
  if(!(stringPoint = strstr(stringCopy, token))) return false;

  if(strtok(stringPoint, quotes) == NULL) return false;

  char* stringToken;
  if((stringToken = strtok(NULL, quotes)) == NULL) return false;

  strcpy(result, stringToken); 

  return true;
}
