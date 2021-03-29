//
//demon hash.h
//

#ifndef HASH_H
#define HASH_H

typedef struct
    {
    unsigned int move_to_try;
    int score;
    bitboard key;
    char old;
    char tag;
    int depth;
    } hash_entry;

//function headers
void init_hash();
void free_hash();
void clear_hash();
bitboard hash_board( bit_board *brd );
void set_hash( int score, int ply, int depth, char tag, unsigned int move, int side );
int get_hash( int ply, int depth, int *alpha, int *beta, int side );
unsigned int get_move( int side );

//extern
extern int hash_entries;

extern hash_entry *smart_hash;

extern hash_entry *basic_hash;

extern bitboard hash_data[64][12];

extern bitboard ep_modifier[64];

extern bitboard castle_modifier[4];
#endif
