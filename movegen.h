//
//demon movegen.h
//

#ifndef MOVEGEN_H
#define MOVEGEN_H

//function headers
int gen_all( unsigned int *move_list );
int gen_moves( unsigned int *move_list );
int gen_captures( unsigned int *move_list );

void gen_white_pawn_moves();
void gen_black_pawn_moves();
void gen_white_knight_moves();
void gen_black_knight_moves();
void gen_white_bishop_queen_moves();
void gen_black_bishop_queen_moves();
void gen_white_rook_queen_moves();
void gen_black_rook_queen_moves();
void gen_white_king_moves();
void gen_black_king_moves();

void gen_white_pawn_captures();
void gen_black_pawn_captures();
void gen_white_knight_captures();
void gen_black_knight_captures();
void gen_white_bishop_queen_captures();
void gen_black_bishop_queen_captures();
void gen_white_rook_queen_captures();
void gen_black_rook_queen_captures();
void gen_white_king_captures();
void gen_black_king_captures();

#endif
