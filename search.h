//
//demon search.h
//

#ifndef SEARCH_H
#define SEARCH_H

typedef struct
    {
    char in_check;
    char last_ply_was_extended;
    char ep;
    bitboard key;
    char castle;
    int score;
    unsigned int move_list[MAX_MOVES];
    unsigned int lose_list[MAX_MOVES];
    unsigned int move_played;
    char num;
	char num_lose;
	char num_history;
    char gen_stage;
    unsigned int hash_move;
    unsigned int killer_1;
    unsigned int killer_2;
    } plyinfo;

//function headers
void iterate( int side, int depth );
int search( int ply, int depth, int side, int alpha, int beta, int null_move );
int root_search( int side, int depth, unsigned int *move_list, bitboard *node_count, int num_moves, int alpha, int beta );
int quiescient( int ply, int side, int alpha, int beta );
void search_clear();

//extern
extern int history_white[64][64];
extern int history_black[64][64];
extern unsigned int best_move;
extern int nodes;
extern int last_nodes;
extern double secs;

extern unsigned int pv[MAX_DEPTH][MAX_DEPTH];
extern int pv_length[MAX_DEPTH];
extern plyinfo ply_info[MAX_DEPTH];
extern int sort[MAX_MOVES];
extern int initial_depth;
extern int history_white[64][64];
extern int history_black[64][64];

extern int InputAvailable();
#endif
