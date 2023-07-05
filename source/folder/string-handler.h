#ifndef STRING_HANDLER_H
#define STRING_HANDLER_H

bool split_string_delim(char (*stringArray)[128], const char string[], int length, const char delim[], int amount);

bool merge_string_delim(char* string, char* stringArray[], int amount, const char delim[]);

int string_symbol_index(const char string[], int length, char symbol);

bool parse_token_quotes(char* result, const char string[], int length, const char token[], const char quotes[]);

#endif