//
//demon string.h
//

#ifndef STRING_H
#define STRING_H

#include "bitboard.h"

//function headers
int alpha_numeric_to_int( char c, char n );
void get( char str [], int n );
void parse_position( char string [] );
void parse_option( char string [] );
int str_to_move(const char* str);
bool string_starts( char s1 [], char s2 [] );
char *to_short_algebraic( bit_board *b, unsigned int m );
char *to_long_algebraic( bit_board *b, unsigned int m );

#endif