//
//demon bitboard.h
//

#pragma once

#include <cstdint>

using bitboard = uint64_t;

using position = struct
{
	int ep;
	int turn;
	int score;
	char castle;
	bitboard key;
	int square[64];
	int white_king;
	int black_king;
	bitboard white_knights;
	bitboard black_knights;
	bitboard white_bishops;
	bitboard black_bishops;
	bitboard white_rooks;
	bitboard black_rooks;
	bitboard white_queens;
	bitboard black_queens;
	bitboard white_pawns;
	bitboard black_pawns;
	bitboard white_pieces;
	bitboard black_pieces;
	bitboard rotate_45;
	bitboard rotate_90;
	bitboard rotate_135;
};

//extern bitboards
extern bitboard file[8];
extern bitboard rank[8];
extern bitboard mask[64];
extern position* board;
extern bitboard zero_left;
extern bitboard zero_right;
extern bitboard not_mask[64];
extern bitboard knight_moves[64];
extern bitboard king_moves[64];
extern bitboard pawn_attack_black[64];
extern bitboard pawn_attack_white[64];
extern bitboard obstructed[64][64];
extern bitboard rook_move_horiz[64][256];
extern bitboard rook_attack_horiz[64][256];
extern bitboard rook_move_vert[64][256];
extern bitboard rook_attack_vert[64][256];
extern bitboard attack_rotate_135[64][256];
extern bitboard move_rotate_135[64][256];
extern bitboard attack_rotate_45[64][256];
extern bitboard move_rotate_45[64][256];
extern bitboard rank_block_mask[8][8];

//extern vars
extern char ls_bits[65536];
extern unsigned int horiz_shift[64];
extern unsigned int vert_shift[64];
extern int distance[64][64];

extern int white_pawn_shift_left_12;
extern int black_pawn_shift_left_12;
extern int white_knight_shift_left_12;
extern int black_knight_shift_left_12;
extern int white_king_shift_left_12;
extern int black_king_shift_left_12;

extern int normal[64];
extern int rotate_45[64];
extern int rotate_90[64];
extern int rotate_135[64];
extern int rotate_45_shift[64];
extern int rotate_135_shift[64];
extern int rotate_45_mask[64];
extern int rotate_135_mask[64];
extern int length_rotate_45[64];
extern int length_rotate_135[64];
extern int down_right[64];
extern int down_left[64];
extern int up_left[64];
extern int up_right[64];
