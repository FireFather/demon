//
//demon eval.h
//

#ifndef EVAL_H
#define EVAL_H

#define WHITE 0
#define BLACK 1

//function headers

int eval( int side, int alpha, int beta );
int white_pawn( int sq );
int black_pawn( int sq );
int white_king( int sq );
int black_king( int sq );
int white_pawn_shield( int file );
int black_pawn_shield( int file );
int knight_mobility( const int sq );
int bishop_mobility( const int sq );
int rook_mobility( const int sq );
int queen_mobility( const int sq );
int w_dev_penalty();
int b_dev_penalty();
int is_draw();
#endif
