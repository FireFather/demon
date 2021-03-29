//
//demon protocol.h
//

#ifndef INTERFACE_H
#define INTERFACE_H

#include <time.h>

//function headers

void process_input();
void new_game();

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

extern double search_time;

extern double move_time_in_cps;

extern int nodes_per_check;

extern clock_t start_time;
#endif
