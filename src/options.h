//
//demon options.h
//

#pragma once

//function headers
void define_option(const char* name, const char* default_value, int type, int min, int max, ...);

//extern
extern bool use_log;

inline int pawn_value = 100;
inline int knight_value = 320;
inline int bishop_value = 330;
inline int rook_value = 500;
inline int queen_value = 1000;

extern int total_value;

extern void init_options();
extern void get_options();
extern void print_options();
extern int get_option_bool(const char* option_name);
extern int get_option_int(const char* option_name);
extern char* get_option_string(const char* option_name, char* str);
extern void set_option(const char* option_name, const char* value);
