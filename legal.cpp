//
//demon legal.cpp
//

#include "define.h"
#include "check.h"
#include "bitboard.h"

int legal(const unsigned int move)
{
	if (move)
	{
		const int to = TO(move);
		const int from = FROM(move);
		const int piece = PIECE(move);
		const int special = SPECIAL(move);
		const int captured = CAPTURED(move);

		if (BLACK_TURN(board->turn))
		{
			if (piece > 8)
				return 0;
		}
		else
		{
			if (piece < 8)
				return 0;
		}

		switch (piece)
		{
		case EMPTY:
			return 0;

		case WHITE_KING:
			{
				if (from - to == 2)
				{
					if (special == 1 && to == G1 && board->square[H1] == WHITE_ROOK && board->castle & 4
						&& !board->square[F1] && !board->square[G1] && !attack(1, E1) && !attack(1, F1)
						&& !attack(1, G1))
						return 1;
					return 0;
				}
				if (to - from == 2)
				{
					if (special == 2 && to == C1 && board->square[A1] == WHITE_ROOK && board->castle & 8
						&& !board->square[D1] && !board->square[C1] && !board->square[B1] && !attack(1, E1)
						&& !attack(1, D1) && !attack(1, C1))
						return 1;
					return 0;
				}
			}
			break;

		case BLACK_KING:
			{
				if (from - to == 2)
				{
					if (special == 1 && to == G8 && board->square[H8] == BLACK_ROOK && board->castle & 1
						&& !board->square[F8] && !board->square[G8] && !attack(-1, E8) && !attack(-1, F8)
						&& !attack(-1, G8))
						return 1;
					return 0;
				}
				if (to - from == 2)
				{
					if (special == 2 && to == C8 && board->square[A8] == BLACK_ROOK && board->castle & 2
						&& !board->square[D8] && !board->square[C8] && !board->square[B8] && !attack(-1, E8)
						&& !attack(-1, D8) && !attack(-1, C8))
						return 1;
					return 0;
				}
			}
			break;

		case WHITE_PAWN:
			if (board->square[from] == piece)
			{
				if (to - from == 8)
				{
					if (board->square[to] == EMPTY)
						return 1;
					return 0;
				}

				if (to - from == 16)
				{
					if (board->square[to] == EMPTY && board->square[to - 8] == EMPTY)
						return 1;
					return 0;
				}

				if (captured)
				{
					if (special == 4)
					{
						if (board->square[to] == EMPTY && board->square[to - 8] == BLACK_PAWN && board->ep == to)
							return 1;
						return 0;
					}

					if (board->square[to] == captured && (captured & 7) != BLACK_KING)
						return 1;
				}
			}
			return 0;

		case BLACK_PAWN:
			if (board->square[from] == piece)
			{
				if (from - to == 8)
				{
					if (board->square[to] == EMPTY)
						return 1;
					return 0;
				}

				if (from - to == 16)
				{
					if (board->square[to] == EMPTY && board->square[to + 8] == EMPTY)
						return 1;
					return 0;
				}

				if (captured)
				{
					if (special == 4)
					{
						if (board->square[to] == EMPTY && board->square[to + 8] == WHITE_PAWN && board->ep == to)
							return 1;
						return 0;
					}

					if (board->square[to] == captured && (captured & 7) != BLACK_KING)
						return 1;
				}
			}
			return 0;

		case WHITE_KNIGHT:
			break;

		case BLACK_KNIGHT:
			break;

		case WHITE_BISHOP:
			break;

		case BLACK_BISHOP:
			break;

		case WHITE_ROOK:
			break;

		case BLACK_ROOK:
			break;

		case WHITE_QUEEN:
			if ((board->white_pieces | board->black_pieces) & obstructed[from][to])
				return 0;

		case BLACK_QUEEN:
			if ((board->white_pieces | board->black_pieces) & obstructed[from][to])
				return 0;
		}

		if (board->square[from] != piece || board->square[to] != captured || (captured & 7) == BLACK_KING)
			return 0;
		return 1;
	}
	return 0;
}
