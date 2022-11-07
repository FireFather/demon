//
//demon string.cpp
//

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstring>

#include "bitops.h"
#include "bitboard.h"
#include "define.h"
#include "draw.h"
#include "init.h"
#include "move.h"
#include "protocol.h"

int alpha_numeric_to_int(const char c, const char n)
{
	return 7 - (c - 97) + (n - 49) * 8;
}

int str_to_move(const char* str)
{
	if (isalpha(str[0]) && isdigit(str[1]) && isalpha(str[2])
		&& isdigit(str[3]))
	{
		unsigned int move =
			alpha_numeric_to_int(str[0], str[1]) | board->square[alpha_numeric_to_int(str[0], str[1])] << 12
			| alpha_numeric_to_int(str[2], str[3]) << 6
			| board->square[alpha_numeric_to_int(str[2], str[3])] << 16;

		if (isalpha(static_cast<int>(str[4]) == 'q') || isalpha(static_cast<int>(str[4]) == 'Q'))
			promote = 4;

		else if (isalpha(static_cast<int>(str[4]) == 'r') || isalpha(static_cast<int>(str[4]) == 'R'))
			promote = 3;

		else if (isalpha(static_cast<int>(str[4]) == 'b') || isalpha(static_cast<int>(str[4]) == 'B'))
			promote = 2;

		else if (isalpha(static_cast<int>(str[4]) == 'n') || isalpha(static_cast<int>(str[4]) == 'N'))
			promote = 1;

		if ((PIECE(move) & 7) == 6)
		{
			if ((TO(move) & 7) - (FROM(move) & 7) == 2)
				move |= 2 << 20;

			else if ((TO(move) & 7) - (FROM(move) & 7) == -2)
				move |= 1 << 20;
		}

		if ((PIECE(move) & 7) == 5 && CAPTURED(move) == 0 && (FROM(move) & 7) != (TO(move) & 7))
		{
			if (PIECE(move) == 5)
			{
				move =
					alpha_numeric_to_int(str[0], str[1]) | board->square[alpha_numeric_to_int(str[0], str[1])] << 12
					| alpha_numeric_to_int(str[2], str[3]) << 6 | WHITE_PAWN << 16 | 4 << 20;
			}
			else
			{
				move =
					alpha_numeric_to_int(str[0], str[1]) | board->square[alpha_numeric_to_int(str[0], str[1])] << 12
					| alpha_numeric_to_int(str[2], str[3]) << 6 | BLACK_PAWN << 16 | 4 << 20;
			}
		}
		return move;
	}
	return 0;
}

char* to_long_algebraic(bit_board* b, const unsigned int m)
{
	const auto coord_move = static_cast<char*>(malloc(6));

	sprintf(coord_move, "%c%d%c%d", 7 - FROM(m) % 8 + 97, FROM(m) / 8 + 1, 7 - TO(m) % 8 + 97,
	        TO(m) / 8 + 1);
	return coord_move;
}

char* to_short_algebraic(const bit_board* b, const unsigned int m)
{
	const auto alg = static_cast<char*>(malloc(7));
	int at = 1;
	int sq;
	bitboard hold;

	switch (PIECE(m) & 7)
	{
	case BLACK_PAWN:
		{
			if (CAPTURED(m) == 0)
			{
				alg[0] = static_cast<char>(104 - TO(m) % 8);
				alg[1] = static_cast<char>((TO(m) / 8 + 49));
				alg[2] = 0;
			}
			else
			{
				alg[0] = static_cast<char>(104 - FROM(m) % 8);
				alg[1] = 'x';
				alg[2] = static_cast<char>(104 - TO(m) % 8);
				alg[3] = static_cast<char>((TO(m) / 8 + 49));
				alg[4] = 0;
			}
			return alg;
		}

	case BLACK_ROOK:
		{
			alg[0] = 'R';

			if (b->turn)
				hold = b->black_rooks;
			else
				hold = b->white_rooks;
			sq = TO(m);

			if (pop_count((rook_attack_horiz[sq][(board->white_pieces | board->black_pieces)
						>> horiz_shift[sq] & 0xff]
					| rook_attack_vert[sq][board->rotate_90 >> vert_shift[sq] & 0xff]) & hold)
				> 1)
			{
				alg[1] = static_cast<char>(104 - FROM(m) % 8);
				at = 2;

				if (pop_count((rook_attack_horiz[sq][(board->white_pieces | board->black_pieces)
							>> horiz_shift[sq] & 0xff]
						| rook_attack_vert[sq][board->rotate_90 >> vert_shift[sq] & 0xff]) & hold & file[FROM(m) % 8])
					> 1)
				{
					alg[2] = static_cast<char>((FROM(m) / 8 + 49));
					at = 3;
				}
			}
			break;
		}

	case BLACK_KNIGHT:
		{
			alg[0] = 'N';

			if (b->turn)
				hold = b->black_knights;
			else
				hold = b->white_knights;
			sq = TO(m);

			if (pop_count(knight_moves[sq] & hold) > 1)
			{
				alg[1] = static_cast<char>(104 - FROM(m) % 8);
				at = 2;

				if (pop_count(knight_moves[sq] & hold & file[FROM(m) % 8]) > 1)
				{
					alg[2] = static_cast<char>((FROM(m) / 8 + 49));
					at = 3;
				}
			}
			break;
		}

	case BLACK_BISHOP:
		{
			alg[0] = 'B';

			if (b->turn)
				hold = b->black_bishops;
			else
				hold = b->white_bishops;
			sq = TO(m);

			if (pop_count((attack_rotate_135[sq][board->rotate_135 >> rotate_135_shift[sq] & rotate_135_mask[sq]]
					| attack_rotate_45[sq][board->rotate_45 >> rotate_45_shift[sq] & rotate_45_mask[sq]]) & hold)
				> 1)
			{
				alg[1] = static_cast<char>(104 - FROM(m) % 8);
				at = 2;

				if (pop_count((attack_rotate_135[sq][board->rotate_135 >> rotate_135_shift[sq] & rotate_135_mask[sq]]
						| attack_rotate_45[sq][board->rotate_45 >> rotate_45_shift[sq] & rotate_45_mask[sq]]) & hold & file[FROM(m)
						% 8])
					> 1)
				{
					alg[2] = static_cast<char>((FROM(m) / 8 + 49));
					at = 3;
				}
			}
			break;
		}

	case BLACK_QUEEN:
		{
			alg[0] = 'Q';

			if (b->turn)
				hold = b->black_queens;
			else
				hold = b->white_queens;
			sq = TO(m);

			if (pop_count((attack_rotate_135[sq][board->rotate_135 >> rotate_135_shift[sq] & rotate_135_mask[sq]]
					| attack_rotate_45[sq][board->rotate_45 >> rotate_45_shift[sq] & rotate_45_mask[sq]]) & hold)
				> 1)
			{
				alg[1] = static_cast<char>(104 - FROM(m) % 8);
				at = 2;

				if (pop_count((attack_rotate_135[sq][board->rotate_135 >> rotate_135_shift[sq] & rotate_135_mask[sq]]
						| attack_rotate_45[sq][board->rotate_45 >> rotate_45_shift[sq] & rotate_45_mask[sq]]) & hold & file[FROM(m)
						% 8])
					> 1)
				{
					alg[2] = static_cast<char>((FROM(m) / 8 + 49));
					at = 3;
				}
			}
			break;
		}

	case BLACK_KING:
		{
			if (SPECIAL(m) == 1)
			{
				alg[0] = 'O';
				alg[1] = '-';
				alg[2] = 'O';
				alg[3] = 0;
				return alg;
			}
			if (SPECIAL(m) == 2)
			{
				alg[0] = 'O';
				alg[1] = '-';
				alg[2] = 'O';
				alg[3] = '-';
				alg[4] = 'O';
				alg[5] = 0;
				return alg;
			}
			alg[0] = 'K';
			break;
		}
	}

	if (CAPTURED(m) == 0)
	{
		alg[at] = static_cast<char>(104 - TO(m) % 8);
		alg[at + 1] = static_cast<char>((TO(m) / 8 + 49));
		alg[at + 2] = 0;
	}
	else
	{
		alg[at] = 'x';
		alg[at + 1] = static_cast<char>(104 - TO(m) % 8);
		alg[at + 2] = static_cast<char>((TO(m) / 8 + 49));
		alg[at + 3] = 0;
	}
	return alg;
}

void parse_position(char string[])
{
	move_number = 0;
	char* moves = strstr(string, "moves ");

	if (const auto fen = ""; fen != nullptr)
	{
		if (moves != nullptr)
		{
			moves[-1] = '\0';
		}
		set_start_position();
	}
	else
		set_start_position();

	if (moves != nullptr)
	{
		char move_string[256];
		const char* ptr = moves + 6;

		while (*ptr != '\0')
		{
			move_string[0] = *ptr++;
			move_string[1] = *ptr++;
			move_string[2] = *ptr++;
			move_string[3] = *ptr++;

			if (*ptr == '\0' || *ptr == ' ')
			{
				move_string[4] = '\0';
			}
			else
			{
				move_string[4] = *ptr++;
				move_string[5] = '\0';
			}
			const int move = str_to_move(move_string);

			make_move(move, 0);
			move_number++;
			position_list[move_number] = board->key;

			while (*ptr == ' ')
				ptr++;
		}
	}
}

void get(char str[], const int n)
{
	char c;
	int i = 0;

	while ((c = getchar()) != '\n')
	{
		if (i < n)
		{
			str[i++] = c;
		}
	}
	str[i] = '\0';
}

bool string_starts(const char s1[], const char s2[])
{
	return strstr(s1, s2) == s1;
}
