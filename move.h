//
//demon move.h
//

#ifndef MOVE_H
#define MOVE_H

//function headers
void make_move( unsigned int move, int ply );
void un_make_move( unsigned int move, int ply );
unsigned int next_move( int ply, int side );

//extern
extern int promote;
extern int piece_num[15];
extern int piece_values[15];
extern int promote_values[5];
extern unsigned int killer[2][MAX_DEPTH];
extern bool white_has_castled;
extern bool black_has_castled;
extern bool White_KR_has_moved;
extern bool White_QR_has_moved;
extern bool Black_KR_has_moved;
extern bool Black_QR_has_moved;

#endif
