//
//demon protocol.h
//

#pragma once

#include <ctime>

//function headers
int process_input();
void new_game();
void start_search();
void parse_option(char string[]);

//extern
extern bool ponder;
extern int hash_size;
extern int move_number;
extern int moves_to_go;
extern double winc;
extern double binc;
extern double wtime;
extern double btime;
extern double move_time;
extern int search_depth;
extern double move_time_in_cps;
extern int nodes_per_check;
extern clock_t start_time;
extern void perft(int);
extern void divide(int);
