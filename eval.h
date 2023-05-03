//
//demon eval.h
//

#pragma once

constexpr auto white = 0;
constexpr auto black = 1;

//function headers
int eval(int side, int alpha, int beta);
int white_pawn(int sq);
int black_pawn(int sq);
int white_king(int sq);
int black_king(int sq);
int white_pawn_shield(int occfile);
int black_pawn_shield(int ofile);
int knight_mobility(int sq);
int bishop_mobility(int sq);
int rook_mobility(int sq);
int queen_mobility(int sq);
int w_dev_penalty();
int b_dev_penalty();
int is_draw();
