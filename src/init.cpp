//
//demon init.cpp
//
#include <cstdlib>

#include "init.h"
#include "bitboard.h"
#include "define.h"
#include "hash.h"
#include "options.h"
#include "search.h"

void init()
{
	int i, j;
	board = static_cast<position*>(malloc(sizeof(position)));

	set_start_position();
	init_bitboards();
	init_options();
	get_options();
	init_hash();

	for (i = 0; i < 50; i++)
		for (j = 0; j < 50; j++)
			pv[i][j] = 0;

	for (i = 0; i < 50; i++)
		pv_length[i] = 0;

	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
		{
			history_white[i][j] = 0;
			history_black[i][j] = 0;
		}
}

void init_bitboards()
{
	int i, k, q, l;
	bitboard j = 1;
	zero_right = 1;
	zero_left = j << 63;

	for (i = 8; i < 57; i += 8)
	{
		zero_right |= j << i;
		zero_left |= j << (i - 1);
	}
	zero_right = ~zero_right;
	zero_left = ~zero_left;

	j = 1;

	for (i = 0; i < 64; i++)
	{
		mask[i] = j << i;
		not_mask[i] = ~mask[i];
		pawn_attack_black[i] = (mask[i] & zero_right) << 7 | (mask[i] & zero_left) << 9;
		pawn_attack_white[i] = (mask[i] & zero_left) >> 7 | (mask[i] & zero_right) >> 9;
	}
	int knight_inc[8] =
	{
		-17, 17, -15, 15, -10, 10, -6, 6
	};

	for (i = 0; i < 64; i++)
	{
		j = 0;

		for (k = 0; k < 8; k++)
			if (i + knight_inc[k] >= 0 && i + knight_inc[k] < 64)
				if (ABS((i + knight_inc[k]) % 8 - i % 8) + ABS((i + knight_inc[k]) / 8 - i / 8) == 3)
				{
					j |= mask[i + knight_inc[k]];
				}
		knight_moves[i] = j;
	}
	int king_inc[8] =
	{
		-1, 1, -8, 8, 7, -7, 9, -9
	};

	for (i = 0; i < 64; i++)
	{
		j = 0;

		for (k = 0; k < 8; k++)
			if (i + king_inc[k] >= 0 && i + king_inc[k] < 64)
				if (ABS((i + king_inc[k]) % 8 - i % 8) + ABS((i + king_inc[k]) / 8 - i / 8) < 3)
				{
					j |= mask[i + king_inc[k]];
				}
		king_moves[i] = j;
	}

	for (i = 0; i < 64; i++)
	{
		horiz_shift[i] = i >> 3 << 3;
		vert_shift[i] = i % 8 << 3;
	}

	for (i = 0; i < 64; i++)
		for (q = 0; q < 256; q++)
		{
			rook_move_horiz[i][q] = 0;
			rook_attack_horiz[i][q] = 0;
		}
	j = 1;

	for (i = 0; i < 64; i++)
	{
		for (q = 0; q < 256; q++)
		{
			for (k = i + 1; k < i + 8 - i % 8 && !(1 << k % 8 & q); k++)
			{
				rook_move_horiz[i][q] |= j << k;
			}

			if (k < i + 8 - i % 8)
				rook_attack_horiz[i][q] |= j << k;

			for (k = i - 1; k >= i - i % 8 && !(1 << k % 8 & q); k--)
			{
				rook_move_horiz[i][q] |= j << k;
			}

			if (k >= i - i % 8)
				rook_attack_horiz[i][q] |= j << k;
		}
	}

	for (i = 0; i < 64; i++)
		for (q = 0; q < 256; q++)
		{
			rook_move_vert[i][q] = 0;
			rook_attack_vert[i][q] = 0;
		}
	j = 1;

	for (i = 0; i < 64; i++)
	{
		for (q = 0; q < 256; q++)
		{
			for (k = i + 8; k < 64 && !((1 << (7 - k / 8)) & q); k += 8)
			{
				rook_move_vert[i][q] |= j << k;
			}

			if (k < 64)
				rook_attack_vert[i][q] |= j << k;

			for (k = i - 8; k >= 0 && !((1 << (7 - k / 8)) & q); k -= 8)
			{
				rook_move_vert[i][q] |= j << k;
			}

			if (k >= 0)
				rook_attack_vert[i][q] |= j << k;
		}
	}

	for (i = 0; i < 64; i++)
		for (q = 0; q < 256; q++)
		{
			move_rotate_135[i][q] = 0;
			attack_rotate_135[i][q] = 0;
		}
	j = 1;

	for (i = 0; i < 64; i++)
	{
		for (q = 0; q < 1 << length_rotate_135[i]; q++)
		{
			l = up_left[i];

			for (k = i + 9; l < length_rotate_135[i] && !(1 << l & q); k += 9)
			{
				move_rotate_135[i][q] |= j << k;
				l++;
			}

			if (l < length_rotate_135[i])
				attack_rotate_135[i][q] |= j << k;
			l = down_right[i];

			for (k = i - 9; l < length_rotate_135[i] && k >= 0 && !((1 << (length_rotate_135[i] - l - 1)) & q); k -= 9)
			{
				move_rotate_135[i][q] |= j << k;
				l++;
			}

			if (l < length_rotate_135[i] && k >= 0)
				attack_rotate_135[i][q] |= j << k;
		}
	}

	for (i = 0; i < 64; i++)
		for (q = 0; q < 256; q++)
		{
			move_rotate_45[i][q] = 0;
			attack_rotate_45[i][q] = 0;
		}
	j = 1;

	for (i = 0; i < 64; i++)
	{
		for (q = 0; q <= 1 << length_rotate_45[i]; q++)
		{
			l = up_right[i];

			for (k = i + 7; l < length_rotate_45[i] && !(1 << l & q); k += 7)
			{
				move_rotate_45[i][q] |= j << k;
				l++;
			}

			if (l < length_rotate_45[i])
				attack_rotate_45[i][q] |= j << k;
			l = down_left[i];

			for (k = i - 7; l < length_rotate_45[i] && k >= 0 && !((1 << (length_rotate_45[i] - l - 1)) & q); k -= 7)
			{
				move_rotate_45[i][q] |= j << k;
				l++;
			}

			if (l < length_rotate_45[i] && k >= 0)
				attack_rotate_45[i][q] |= j << k;
		}
	}

	j = 1;

	for (i = 0; i < 64; i++)
		for (q = 0; q < 64; q++)
		{
			obstructed[i][q] = 0;

			if (i / 8 == q / 8)
			{
				if (i > q)
				{
					for (k = q + 1; k < i; k++)
						obstructed[i][q] |= j << k;
				}
				else
				{
					for (k = i + 1; k < q; k++)
						obstructed[i][q] |= j << k;
				}
			}
			else if (i % 8 == q % 8)
			{
				if (i > q)
				{
					for (k = q + 8; k < i; k += 8)
						obstructed[i][q] |= j << k;
				}
				else
				{
					for (k = i + 8; k < q; k += 8)
						obstructed[i][q] |= j << k;
				}
			}
			else
			{
				if (i % 8 < q % 8 && i / 8 > q / 8)
				{
					for (k = q + 7; k < i; k += 7)
						obstructed[i][q] |= j << k;
				}
				else if (i % 8 > q % 8 && i / 8 > q / 8)
				{
					for (k = q + 9; k < i; k += 9)
						obstructed[i][q] |= j << k;
				}
				else if (i % 8 > q % 8 && i / 8 < q / 8)
				{
					for (k = q - 7; k > i; k -= 7)
						obstructed[i][q] |= j << k;
				}
				else if (i % 8 < q % 8 && i / 8 < q / 8)
				{
					for (k = q - 9; k > i; k -= 9)
						obstructed[i][q] |= j << k;
				}
			}
		}

	board->white_knights = 0;
	board->white_pieces = 0;
	board->black_knights = 0;
	board->black_pieces = 0;
	board->white_pawns = 0;
	board->black_pawns = 0;
	board->white_rooks = 0;
	board->white_bishops = 0;
	board->black_rooks = 0;
	board->black_bishops = 0;
	board->white_queens = 0;
	board->black_queens = 0;
	board->rotate_90 = 0;
	board->rotate_45 = 0;
	board->rotate_135 = 0;

	for (i = 0; i < 64; i++)
	{
		if (board->square[i] != EMPTY)
		{
			board->rotate_90 |= mask[rotate_90[i]];
			board->rotate_135 |= mask[rotate_135[i]];
			board->rotate_45 |= mask[rotate_45[i]];
		}

		switch (board->square[i])
		{
		case WHITE_KNIGHT:
			board->white_knights |= mask[i];
			board->white_pieces |= mask[i];
			break;

		case WHITE_BISHOP:
			board->white_bishops |= mask[i];
			board->white_pieces |= mask[i];
			break;

		case WHITE_ROOK:
			board->white_rooks |= mask[i];
			board->white_pieces |= mask[i];
			break;

		case WHITE_QUEEN:
			board->white_queens |= mask[i];
			board->white_pieces |= mask[i];
			break;

		case WHITE_PAWN:
			board->white_pawns |= mask[i];
			board->white_pieces |= mask[i];
			break;

		case WHITE_KING:
			board->white_king = i;
			board->white_pieces |= mask[i];
			break;

		case BLACK_KNIGHT:
			board->black_knights |= mask[i];
			board->black_pieces |= mask[i];
			break;

		case BLACK_BISHOP:
			board->black_bishops |= mask[i];
			board->black_pieces |= mask[i];
			break;

		case BLACK_ROOK:
			board->black_rooks |= mask[i];
			board->black_pieces |= mask[i];
			break;

		case BLACK_QUEEN:
			board->black_queens |= mask[i];
			board->black_pieces |= mask[i];
			break;

		case BLACK_PAWN:
			board->black_pawns |= mask[i];
			board->black_pieces |= mask[i];
			break;

		case BLACK_KING:
			board->black_king = i;
			board->black_pieces |= mask[i];
			break;
		default: ;
		}
	}

	ls_bits[0] = 20;

	for (i = 1; i < 65536; i++)
	{
		k = (i & -i) >> 1;
		ls_bits[i] = 0;

		while (k > 0)
		{
			ls_bits[i]++;
			k = k >> 1;
		}
	}

	for (i = 0; i < 8; i++)
		file[i] = 0;
	j = 1;

	for (i = 0; i < 8; i++)
	{
		for (k = 0; k < 8; k++)
			file[k] |= j << (k + i * 8);
	}

	for (i = 0; i < 8; i++)
		rank[i] = 0;
	j = 1;

	for (i = 0; i < 8; i++)
	{
		for (k = 0; k < 8; k++)
			rank[k] |= j << (k * 8 + i);
	}

	for (i = 0; i < 8; i++)
		for (l = 0; l < 8; l++)
		{
			rank_block_mask[i][l] = 0;

			if (i < l)
			{
				for (k = i; k <= l; k++)
					rank_block_mask[i][l] |= rank[k];
			}
			else
			{
				for (k = l; k <= i; k++)
					rank_block_mask[i][l] |= rank[k];
			}
		}

	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
		{
			distance[i][j] =
				ABS(FILE(i) - FILE(j)) > ABS(RANK(i) - RANK(j)) ? ABS(FILE(i) - FILE(j)) : ABS(RANK(i) - RANK(j));
		}
}

void set_start_position()
{
	board->castle = 1 | 2 | 4 | 8;
	board->square[0] = WHITE_ROOK;
	board->square[1] = WHITE_KNIGHT;
	board->square[2] = WHITE_BISHOP;
	board->square[3] = WHITE_KING;
	board->square[4] = WHITE_QUEEN;
	board->square[5] = WHITE_BISHOP;
	board->square[6] = WHITE_KNIGHT;
	board->square[7] = WHITE_ROOK;
	board->square[8] = WHITE_PAWN;
	board->square[9] = WHITE_PAWN;
	board->square[10] = WHITE_PAWN;
	board->square[11] = WHITE_PAWN;
	board->square[12] = WHITE_PAWN;
	board->square[13] = WHITE_PAWN;
	board->square[14] = WHITE_PAWN;
	board->square[15] = WHITE_PAWN;
	board->square[16] = EMPTY;
	board->square[17] = EMPTY;
	board->square[18] = EMPTY;
	board->square[19] = EMPTY;
	board->square[20] = EMPTY;
	board->square[21] = EMPTY;
	board->square[22] = EMPTY;
	board->square[23] = EMPTY;
	board->square[24] = EMPTY;
	board->square[25] = EMPTY;
	board->square[26] = EMPTY;
	board->square[27] = EMPTY;
	board->square[28] = EMPTY;
	board->square[29] = EMPTY;
	board->square[30] = EMPTY;
	board->square[31] = EMPTY;
	board->square[32] = EMPTY;
	board->square[33] = EMPTY;
	board->square[34] = EMPTY;
	board->square[35] = EMPTY;
	board->square[36] = EMPTY;
	board->square[37] = EMPTY;
	board->square[38] = EMPTY;
	board->square[39] = EMPTY;
	board->square[40] = EMPTY;
	board->square[41] = EMPTY;
	board->square[42] = EMPTY;
	board->square[43] = EMPTY;
	board->square[44] = EMPTY;
	board->square[45] = EMPTY;
	board->square[46] = EMPTY;
	board->square[47] = EMPTY;
	board->square[48] = BLACK_PAWN;
	board->square[49] = BLACK_PAWN;
	board->square[50] = BLACK_PAWN;
	board->square[51] = BLACK_PAWN;
	board->square[52] = BLACK_PAWN;
	board->square[53] = BLACK_PAWN;
	board->square[54] = BLACK_PAWN;
	board->square[55] = BLACK_PAWN;
	board->square[56] = BLACK_ROOK;
	board->square[57] = BLACK_KNIGHT;
	board->square[58] = BLACK_BISHOP;
	board->square[59] = BLACK_KING;
	board->square[60] = BLACK_QUEEN;
	board->square[61] = BLACK_BISHOP;
	board->square[62] = BLACK_KNIGHT;
	board->square[63] = BLACK_ROOK;
	init_bitboards();
	board->turn = 0;
	board->score = 0;
	board->ep = 0;
	board->key = hash_board(board);
}
