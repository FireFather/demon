//
//demon options.h
//

#ifndef OPTIONS_H
#define OPTIONS_H

//function headers
void define_option( char *name, char *default_value, int type, int min, int max, ... );

//extern
extern bool use_log;

extern int pawn_value;
extern int knight_value;
extern int bishop_value;
extern int rook_value;
extern int queen_value;
extern int total_value;

extern void init_options( void );
extern void get_options( void );
extern void print_options( void );
extern int get_option_bool( const char *option_name );
extern int get_option_int( const char *option_name );
extern char *get_option_string( const char *option_name, char *str );
extern void set_option( const char *option_name, const char *value );

#endif
