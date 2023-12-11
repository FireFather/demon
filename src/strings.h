//
//demon string.h
//

#pragma once

#include "bitboard.h"

//function headers
int alpha_numeric_to_int(char c, char n);
void get(char str[], int n);
void parse_position(char string[]);
int str_to_move(const char* str);
bool string_starts(const char s1[], const char s2[]);
char* to_short_algebraic(const position* b, unsigned int m);
char* to_long_algebraic(position* b, unsigned int m);
