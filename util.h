//
//demon util.h
//

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

//function headers
unsigned __int64 get_random_64();
void show_version( void );
void open_log( void );
void close_log( void );
void output( const char format [], ... );
int InputAvailable();
//extern
extern FILE *log_file;

#endif
