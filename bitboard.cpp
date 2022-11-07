//
//demon bitboard.cpp
//

#include "define.h"
#include "bitboard.h"

//global bitboards
bitboard file[8];
bitboard rank[8];
bitboard mask[64];
bitboard not_mask[64];
bitboard zero_right;
bitboard zero_left;
bitboard king_moves[64];
bitboard knight_moves[64];
bitboard pawn_attack_black[64];
bitboard pawn_attack_white[64];
bitboard obstructed[64][64];
bitboard rook_attack_vert[64][256];
bitboard rook_attack_horiz[64][256];
bitboard rook_move_vert[64][256];
bitboard rook_move_horiz[64][256];
bitboard attack_rotate_135[64][256];
bitboard move_rotate_135[64][256];
bitboard attack_rotate_45[64][256];
bitboard move_rotate_45[64][256];
bitboard rank_block_mask[8][8];

//global vars
char ls_bits[65536];
unsigned int horiz_shift[64];
unsigned int vert_shift[64];
bit_board* board;
char distance[64][64];

int white_pawn_shift_left_12 = WHITE_PAWN << 12;
int black_pawn_shift_left_12 = BLACK_PAWN << 12;
int white_knight_shift_left_12 = WHITE_KNIGHT << 12;
int black_knight_shift_left_12 = BLACK_KNIGHT << 12;
int white_king_shift_left_12 = WHITE_KING << 12;
int black_king_shift_left_12 = BLACK_KING << 12;

/*
int pop_count( bitboard n )
    {
    int q = 0;

    while (n)
        {
        q++;
        n &= n - 1;
        }
    return q;
    }

int lsb( bitboard n )
    {
    int q = ls_bits[n & 0xffff];

    if (q == 20)
        {
        q = ls_bits[(n >> 16) & 0xffff];

        if (q == 20)
            {
            q = ls_bits[(n >> 32) & 0xffff];

            if (q == 20)
                {
                return 48 + ls_bits[n >> 48];
                }
            return q + 32;
            }
        return q + 16;
        }
    return q;
    }
*/
int normal[64] =
{
	H1, G1, F1, E1, D1, C1, B1, A1,
	H2, G2, F2, E2, D2, C2, B2, A2,
	H3, G3, F3, E3, D3, C3, B3, A3,
	H4, G4, F4, E4, D4, C4, B4, A4,
	H5, G5, F5, E5, D5, C5, B5, A5,
	H6, G6, F6, E6, D6, C6, B6, A6,
	H7, G7, F7, E7, D7, C7, B7, A7,
	H8, G8, F8, E8, D8, C8, B8, A8
};

int rotate_45[64] =
{
	H7, H6, H5, H4, H3, H2, H1, H8,
	G6, G5, G4, G3, G2, G1, G8, G7,
	F5, F4, F3, F2, F1, F8, F7, F6,
	E4, E3, E2, E1, E8, E7, E6, E5,
	D3, D2, D1, D8, D7, D6, D5, D4,
	C2, C1, C8, C7, C6, C5, C4, C3,
	B1, B8, B7, B6, B5, B4, B3, B2,
	A8, A7, A6, A5, A4, A3, A2, A1
};

int rotate_90[64] =
{
	A1, A2, A3, A4, A5, A6, A7, A8,
	B1, B2, B3, B4, B5, B6, B7, B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8
};

int rotate_135[64] =
{
	H1, H8, H7, H6, H5, H4, H3, H2,
	G2, G1, G8, G7, G6, G5, G4, G3,
	F3, F2, F1, F8, F7, F6, F5, F4,
	E4, E3, E2, E1, E8, E7, E6, E5,
	D5, D4, D3, D2, D1, D8, D7, D6,
	C6, C5, C4, C3, C2, C1, C8, C7,
	B7, B6, B5, B4, B3, B2, B1, B8,
	A8, A7, A6, A5, A4, A3, A2, A1
};

int rotate_45_shift[64] =
{
	48, 40, 32, 24, 16, 8, 0, 56,
	40, 32, 24, 16, 8, 0, 56, 49,
	32, 24, 16, 8, 0, 56, 49, 42,
	24, 16, 8, 0, 56, 49, 42, 35,
	16, 8, 0, 56, 49, 42, 35, 28,
	8, 0, 56, 49, 42, 35, 28, 21,
	0, 56, 49, 42, 35, 28, 21, 14,
	56, 49, 42, 35, 28, 21, 14, 7
};

int rotate_135_shift[64] =
{
	0, 56, 48, 40, 32, 24, 16, 8,
	9, 0, 56, 48, 40, 32, 24, 16,
	18, 9, 0, 56, 48, 40, 32, 24,
	27, 18, 9, 0, 56, 48, 40, 32,
	36, 27, 18, 9, 0, 56, 48, 40,
	45, 36, 27, 18, 9, 0, 56, 48,
	54, 45, 36, 27, 18, 9, 0, 56,
	63, 54, 45, 36, 27, 18, 9, 0
};

int rotate_45_mask[64] =
{
	1, 3, 7, 15, 31, 63, 127, 255,
	3, 7, 15, 31, 63, 127, 255, 127,
	7, 15, 31, 63, 127, 255, 127, 63,
	15, 31, 63, 127, 255, 127, 63, 31,
	31, 63, 127, 255, 127, 63, 31, 15,
	63, 127, 255, 127, 63, 31, 15, 7,
	127, 255, 127, 63, 31, 15, 7, 3,
	255, 127, 63, 31, 15, 7, 3, 1
};

int rotate_135_mask[64] =
{
	255, 127, 63, 31, 15, 7, 3, 1,
	127, 255, 127, 63, 31, 15, 7, 3,
	63, 127, 255, 127, 63, 31, 15, 7,
	31, 63, 127, 255, 127, 63, 31, 15,
	15, 31, 63, 127, 255, 127, 63, 31,
	7, 15, 31, 63, 127, 255, 127, 63,
	3, 7, 15, 31, 63, 127, 255, 127,
	1, 3, 7, 15, 31, 63, 127, 255
};

int length_rotate_45[64] =
{
	1, 2, 3, 4, 5, 6, 7, 8,
	2, 3, 4, 5, 6, 7, 8, 7,
	3, 4, 5, 6, 7, 8, 7, 6,
	4, 5, 6, 7, 8, 7, 6, 5,
	5, 6, 7, 8, 7, 6, 5, 4,
	6, 7, 8, 7, 6, 5, 4, 3,
	7, 8, 7, 6, 5, 4, 3, 2,
	8, 7, 6, 5, 4, 3, 2, 1
};

int length_rotate_135[64] =
{
	8, 7, 6, 5, 4, 3, 2, 1,
	7, 8, 7, 6, 5, 4, 3, 2,
	6, 7, 8, 7, 6, 5, 4, 3,
	5, 6, 7, 8, 7, 6, 5, 4,
	4, 5, 6, 7, 8, 7, 6, 5,
	3, 4, 5, 6, 7, 8, 7, 6,
	2, 3, 4, 5, 6, 7, 8, 7,
	1, 2, 3, 4, 5, 6, 7, 8
};

int down_right[64] =
{
	8, 7, 6, 5, 4, 3, 2, 1,
	7, 7, 6, 5, 4, 3, 2, 1,
	6, 6, 6, 5, 4, 3, 2, 1,
	5, 5, 5, 5, 4, 3, 2, 1,
	4, 4, 4, 4, 4, 3, 2, 1,
	3, 3, 3, 3, 3, 3, 2, 1,
	2, 2, 2, 2, 2, 2, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

int down_left[64] =
{
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 7,
	1, 2, 3, 4, 5, 6, 6, 6,
	1, 2, 3, 4, 5, 5, 5, 5,
	1, 2, 3, 4, 4, 4, 4, 4,
	1, 2, 3, 3, 3, 3, 3, 3,
	1, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1
};

int up_left[64] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 2, 2, 2, 2,
	1, 2, 3, 3, 3, 3, 3, 3,
	1, 2, 3, 4, 4, 4, 4, 4,
	1, 2, 3, 4, 5, 5, 5, 5,
	1, 2, 3, 4, 5, 6, 6, 6,
	1, 2, 3, 4, 5, 6, 7, 7,
	1, 2, 3, 4, 5, 6, 7, 8
};

int up_right[64] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 1,
	3, 3, 3, 3, 3, 3, 2, 1,
	4, 4, 4, 4, 4, 3, 2, 1,
	5, 5, 5, 5, 4, 3, 2, 1,
	6, 6, 6, 5, 4, 3, 2, 1,
	7, 7, 6, 5, 4, 3, 2, 1,
	8, 7, 6, 5, 4, 3, 2, 1
};
