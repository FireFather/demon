//
//demon eval.cpp
//

#include "eval.h"
#include "bitboard.h"
#include "bitops.h"
#include "define.h"
#include "move.h"
#include "options.h"
#include "protocol.h"
#include "psq.h"

int pawn_mat[2];
int piece_mat[2];
int pawn_rank[2][10];

int n_mobility = 3;
int b_mobility = 4;
int r_mobility = 2;
int q_mobility = 1;

int bp_bonus = 50;

int dp_penalty = 20;
int ip_penalty = 30;
int bp_penalty = 10;
int pp_bonus = 50;

int rsof_bonus = 10;
int rof_bonus = 15;
int ro7r_bonus = 20;

int dev_penalty = 20;
int bcp_penalty = 20;
int eqm_penalty = 20;
int kof_penalty = 20;
int cas_bonus = 100;

int wp = 0;
int bp = 0;
int wn = 0;
int bn = 0;
int wb = 0;
int bb = 0;
int wr = 0;
int br = 0;
int wq = 0;
int bq = 0;

int twp = 0;
int tbp = 0;

int eval(const int side, const int alpha, const int beta)
{
	int sq;
	int ofile;
	int result;
	int score[2]{};

	wp = pop_count(board->white_pawns);
	wn = pop_count(board->white_knights);
	wb = pop_count(board->white_bishops);
	wr = pop_count(board->white_rooks);
	wq = pop_count(board->white_queens);
	bp = pop_count(board->black_pawns);
	bn = pop_count(board->black_knights);
	bb = pop_count(board->black_bishops);
	br = pop_count(board->black_rooks);
	bq = pop_count(board->black_queens);

	twp = wn + wb + wr + wq;
	tbp = bn + bb + br + bq;

	if (twp >= 3 && tbp >= 3)
	{
		int lazy_eval = board->score;

		if (WHITE_TURN(board->turn))
			lazy_eval = -lazy_eval;

		if (lazy_eval - 200 >= beta)
			return lazy_eval;

		if (lazy_eval + 200 <= alpha)
			return lazy_eval;
	}

	for (sq = 0; sq < 10; ++sq)
	{
		pawn_rank[white][sq] = 0;
		pawn_rank[black][sq] = 7;
	}

	piece_mat[white] = 0;
	piece_mat[black] = 0;
	pawn_mat[white] = 0;
	pawn_mat[black] = 0;

	for (sq = 0; sq < 64; sq++)
		switch (board->square[sq])
		{
		case WHITE_PAWN:
			pawn_mat[white] += pawn_value;
			ofile = FILE(sq) + 1;

			if (RANK(sq) < pawn_rank[white][ofile])
				pawn_rank[white][ofile] = RANK(sq);
			break;

		case WHITE_KNIGHT:
			piece_mat[white] += knight_value;
			break;

		case WHITE_BISHOP:
			piece_mat[white] += bishop_value;
			break;

		case WHITE_ROOK:
			piece_mat[white] += rook_value;
			break;

		case WHITE_QUEEN:
			piece_mat[white] += queen_value;
			break;

		case WHITE_KING:
			break;

		case BLACK_PAWN:
			pawn_mat[black] += pawn_value;
			ofile = FILE(sq) + 1;

			if (RANK(sq) > pawn_rank[black][ofile])
				pawn_rank[black][ofile] = RANK(sq);
			break;

		case BLACK_KNIGHT:
			piece_mat[black] += knight_value;
			break;

		case BLACK_BISHOP:
			piece_mat[black] += bishop_value;
			break;

		case BLACK_ROOK:
			piece_mat[black] += rook_value;
			break;

		case BLACK_QUEEN:
			piece_mat[black] += queen_value;
			break;

		case BLACK_KING:
			break;
		default: ;
		}

	score[white] = piece_mat[white] + pawn_mat[white];
	score[black] = piece_mat[black] + pawn_mat[black];

	if (pawn_mat[white] == 0 && pawn_mat[black] == 0)
	{
		if (is_draw() == 1)
			return 0;
	}

	for (sq = 0; sq < 64; sq++)
		switch (board->square[sq])
		{
		case WHITE_PAWN:
			score[white] += pawn_psq[sq];
			score[white] += white_pawn(sq);

			if (board->square[sq] == D2 && board->square[D3] != EMPTY
				|| board->square[sq] == E2 && board->square[E3] != EMPTY)
				score[white] -= bcp_penalty;
			break;

		case WHITE_KNIGHT:
			score[white] += knight_mobility(sq);
			score[white] += knight_psq[sq];
			score[white] -= distance[sq][board->black_king];
			break;

		case WHITE_BISHOP:
			score[white] += bishop_mobility(sq);
			score[white] += bishop_psq[sq];
			score[white] -= distance[sq][board->black_king];
			break;

		case WHITE_ROOK:
			score[white] += rook_mobility(sq);
			score[white] += rook_psq[sq];
			score[white] -= distance[sq][board->black_king];

			if (pawn_rank[white][FILE(sq) + 1] == 0)
			{
				if (pawn_rank[black][FILE(sq) + 1] == 7)
					score[white] += rof_bonus;
				else
					score[white] += rsof_bonus;
			}

			if (RANK(sq) == 6)
				score[white] += ro7r_bonus;
			break;

		case WHITE_QUEEN:
			score[white] += queen_mobility(sq);
			score[white] -= distance[sq][board->black_king];

			if (board->square[sq] != D1 && move_number < 16)
				score[white] -= eqm_penalty * move_number;
			break;

		case WHITE_KING:
			score[white] += white_king(sq);

			if (piece_mat[black] <= total_value / 2 / 3)
				score[white] += king_endgame_psq[sq];
			else
			{
				score[white] += king_psq[sq];
			}
			break;

		case BLACK_PAWN:
			score[black] += pawn_psq[flip[sq]];
			score[black] += black_pawn(sq);

			if (board->square[sq] == D7 && board->square[D6] != EMPTY
				|| board->square[sq] == E7 && board->square[E6] != EMPTY)
				score[black] -= bcp_penalty;
			break;

		case BLACK_KNIGHT:
			score[black] += knight_mobility(sq);
			score[black] += knight_psq[flip[sq]];
			score[black] -= distance[sq][board->white_king];
			break;

		case BLACK_BISHOP:
			score[black] += bishop_mobility(sq);
			score[black] += bishop_psq[flip[sq]];
			score[black] -= distance[sq][board->white_king];
			break;

		case BLACK_ROOK:
			score[black] += rook_mobility(sq);
			score[black] += rook_psq[flip[sq]];
			score[black] -= distance[sq][board->white_king];

			if (pawn_rank[black][FILE(sq) + 1] == 7)
			{
				if (pawn_rank[white][FILE(sq) + 1] == 0)
					score[black] += rof_bonus;
				else
					score[black] += rsof_bonus;
			}

			if (RANK(sq) == 1)
				score[black] += ro7r_bonus;
			break;

		case BLACK_QUEEN:
			score[black] += queen_mobility(sq);
			score[black] -= distance[sq][board->white_king];

			if (board->square[sq] != D8 && move_number < 16)
				score[black] -= eqm_penalty * move_number;
			break;

		case BLACK_KING:
			score[black] += black_king(sq);

			if (piece_mat[white] <= total_value / 2 / 3)
				score[black] += king_endgame_psq[flip[sq]];
			else
			{
				score[black] += king_psq[flip[sq]];
			}
			break;
		default: ;
		}

	if (wb >= 2)
		score[white] += bp_bonus;

	if (bb >= 2)
		score[black] += bp_bonus;

	if (white_has_castled)
		score[white] += cas_bonus;

	if (black_has_castled)
		score[black] += cas_bonus;

	score[white] -= w_dev_penalty();
	score[black] -= b_dev_penalty();

	if (side == -1)
		result = score[white] - score[black];
	else
		result = score[black] - score[white];

	return result;
}

int white_pawn(const int sq)
{
	int score = 0;
	const int ofile = FILE(sq) + 1;

	if (RANK(sq) < pawn_rank[white][ofile])
		score -= dp_penalty;

	if (pawn_rank[white][ofile - 1] == 0 && pawn_rank[white][ofile + 1] == 0)
		score -= ip_penalty;

	else if (RANK(sq) < pawn_rank[white][ofile - 1] && RANK(sq) < pawn_rank[white][ofile + 1])
		score -= bp_penalty;

	if (RANK(sq) >= pawn_rank[black][ofile - 1] && RANK(sq) >= pawn_rank[black][ofile]
		&& RANK(sq) >= pawn_rank[black][ofile + 1])
		score += pp_bonus * RANK(sq);

	return score;
}

int black_pawn(const int sq)
{
	int score = 0;
	const int ofile = FILE(sq) + 1;

	if (RANK(sq) > pawn_rank[black][ofile])
		score -= dp_penalty;

	if (pawn_rank[black][ofile - 1] == 7 && pawn_rank[black][ofile + 1] == 7)
		score -= ip_penalty;

	else if (RANK(sq) > pawn_rank[black][ofile - 1] && RANK(sq) > pawn_rank[black][ofile + 1])
		score -= bp_penalty;

	if (RANK(sq) <= pawn_rank[white][ofile - 1] && RANK(sq) <= pawn_rank[white][ofile]
		&& RANK(sq) <= pawn_rank[white][ofile + 1])
		score += pp_bonus * (7 - RANK(sq));

	return score;
}

int white_king(const int sq)
{
	int score = 0;

	if (FILE(sq) < 3)
	{
		score += white_pawn_shield(1);
		score += white_pawn_shield(2);
		score += white_pawn_shield(3) / 2;
	}
	else if (FILE(sq) > 4)
	{
		score += white_pawn_shield(8);
		score += white_pawn_shield(7);
		score += white_pawn_shield(6) / 2;
	}
	else
	{
		for (int i = FILE(sq); i <= FILE(sq) + 2; ++i)
			if (pawn_rank[white][i] == 0 && pawn_rank[black][i] == 7)
				score -= kof_penalty;
	}
	score *= piece_mat[black];
	score /= 3200;

	return score;
}

int black_king(const int sq)
{
	int score = 0;

	if (FILE(sq) < 3)
	{
		score += black_pawn_shield(1);
		score += black_pawn_shield(2);
		score += black_pawn_shield(3) / 2;
	}
	else if (FILE(sq) > 4)
	{
		score += black_pawn_shield(8);
		score += black_pawn_shield(7);
		score += black_pawn_shield(6) / 2;
	}
	else
	{
		for (int i = FILE(sq); i <= FILE(sq) + 2; ++i)
			if (pawn_rank[white][i] == 0 && pawn_rank[black][i] == 7)
				score -= kof_penalty;
	}
	score *= piece_mat[white];
	score /= 3200;

	return score;
}

int white_pawn_shield(const int occfile)
{
	int score = 0;

	if (pawn_rank[white][occfile] == 1)
		{}
	else if (pawn_rank[white][occfile] == 2)
		score -= 10;

	else if (pawn_rank[white][occfile] != 0)
		score -= 20;

	else
		score -= 25;

	if (pawn_rank[black][occfile] == 7)
		score -= 15;

	else if (pawn_rank[black][occfile] == 2)
		score -= 10;

	else if (pawn_rank[black][occfile] == 3)
		score -= 5;

	return score;
}

int black_pawn_shield(const int ofile)
{
	int score = 0;

	if (pawn_rank[black][ofile] == 6)
		{}

	else if (pawn_rank[black][ofile] == 5)
		score -= 10;

	else if (pawn_rank[black][ofile] != 7)
		score -= 20;

	else
		score -= 25;

	if (pawn_rank[white][ofile] == 0)
		score -= 15;

	else if (pawn_rank[white][ofile] == 5)
		score -= 10;

	else if (pawn_rank[white][ofile] == 4)
		score -= 5;

	return score;
}

int knight_mobility(const int sq)
{
	int i;
	int score = 0;

	for (i = sq - 6; ; i -= 6)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq + 6; ; i += 6)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq - 10; ; i -= 10)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq + 10; ; i += 10)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq - 15; ; i -= 15)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq + 15; ; i += 15)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq - 17; ; i -= 17)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}

	for (i = sq + 17; ; i += 17)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += n_mobility;
	}
	return score;
}

int bishop_mobility(const int sq)
{
	int i;
	int score = 0;

	for (i = sq - 7; ; i -= 7)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += b_mobility;
	}

	for (i = sq + 7; ; i += 7)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += b_mobility;
	}

	for (i = sq - 9; ; i -= 9)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += b_mobility;
	}

	for (i = sq + 9; ; i += 9)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += b_mobility;
	}
	return score;
}

int rook_mobility(const int sq)
{
	int i;
	int score = 0;

	for (i = sq - 1; ; i -= 1)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += r_mobility;
	}

	for (i = sq + 1; ; i += 1)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += r_mobility;
	}

	for (i = sq - 8; ; i -= 8)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += r_mobility;
	}

	for (i = sq + 8; ; i += 8)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += r_mobility;
	}
	return score;
}

int queen_mobility(const int sq)
{
	int i;
	int score = 0;

	for (i = sq - 1; ; i -= 1)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq + 1; ; i += 1)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq - 7; ; i -= 7)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq + 7; ; i += 7)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq - 8; ; i -= 8)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq + 8; ; i += 8)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq - 9; ; i -= 9)
	{
		if (i < 0)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}

	for (i = sq + 9; ; i += 9)
	{
		if (i > 63)
			break;

		if (board->square[i] != EMPTY)
		{
			break;
		}
		score += q_mobility;
	}
	return score;
}

int w_dev_penalty()
{
	int score = 0;

	if (board->square[C1] == WHITE_BISHOP)
	{
		score += dev_penalty;
	}

	if (board->square[B1] == WHITE_KNIGHT)
	{
		score += dev_penalty;
	}

	if (board->square[F1] == WHITE_BISHOP)
	{
		score += dev_penalty;
	}

	if (board->square[G1] == WHITE_KNIGHT)
	{
		score += dev_penalty;
	}

	if ((board->square[F1] == WHITE_KING || board->square[G1] == WHITE_KING) &&
		(board->square[G1] == WHITE_ROOK || board->square[H1] == WHITE_ROOK))
	{
		score += dev_penalty;
	}
	return score;
}

int b_dev_penalty()
{
	int score = 0;

	if (board->square[C8] == BLACK_BISHOP)
	{
		score += dev_penalty;
	}

	if (board->square[B8] == BLACK_KNIGHT)
	{
		score += dev_penalty;
	}

	if (board->square[F8] == BLACK_BISHOP)
	{
		score += dev_penalty;
	}

	if (board->square[G8] == BLACK_KNIGHT)
	{
		score += dev_penalty;
	}

	if ((board->square[F8] == BLACK_KING || board->square[G8] == BLACK_KING) &&
		(board->square[G8] == BLACK_ROOK || board->square[H8] == BLACK_ROOK))
	{
		score += dev_penalty;
	}
	return score;
}

int is_draw()
{
	if (!wq && !bq)
	{
		if (wr == 1 && br == 1)
		{
			if (wn + wb <= 1 && bn + bb <= 1)
			{
				return 1;
			}
		}
		else if (wr == 1 && !br)
		{
			if (wn + wb == 0 && (bn + bb == 1 || bn + bb == 2))
			{
				return 1;
			}
		}
		else if (br == 1 && !wr)
		{
			if (bn + bb == 0 && (wn + wb == 1 || wn + wb == 2))
			{
				return 1;
			}
		}
	}
	else if (!wq && !bq && !wr && !br)
	{
		if (!bb && !wb)
		{
			if (wn <= 2 && bn <= 2)
			{
				return 1;
			}
		}
		else if (!wn && !bn)
		{
			if (ABS(wb - bb) <= 1)
			{
				return 1;
			}
		}
		else if (wn <= 2 && !wb || wb == 1 && !wn)
		{
			if (bn <= 2 && !bb || bb == 1 && !bn)
			{
				return 1;
			}
		}
		else if (!wb && !bn)
		{
			if (wn <= 1 && bb <= 1)
			{
				return 1;
			}
		}
		else if (!bb && !wn)
		{
			if (bn <= 1 && wb <= 1)
			{
				return 1;
			}
		}
	}
	else if (!wn && !wb && !bn && !bb)
	{
		if (!wq && bq)
		{
			if (bq == 1 && wr == 2)
			{
				return 1;
			}
		}
		else if (!bq && wq)
		{
			if (wq == 1 && br == 2)
			{
				return 1;
			}
		}
	}
	else if (wq && !wr && !wb && !wn && !bq && !bb)
	{
		if (br == 1 && bn == 2)
		{
			return 1;
		}
	}
	else if (bq && !br && !bb && !bn && !wq && !wb)
	{
		if (wr == 1 && wn == 2)
		{
			return 1;
		}
	}
	else if (wq && !wr && !wb && !wn && !bq && !bn)
	{
		if (br == 1 && bb == 2)
		{
			return 1;
		}
	}
	else if (bq && !br && !bb && !bn && !wq && !wn)
	{
		if (wr == 1 && wb == 2)
		{
			return 1;
		}
	}
	else if (wq && !wr && !wb && !bq && !bb && !bn)
	{
		if (wn == 1 && br == 2)
		{
			return 1;
		}
	}
	else if (bq && !br && !bb && !wq && !wb && !wn)
	{
		if (bn == 1 && wr == 2)
		{
			return 1;
		}
	}
	else if (wq && !wr && !wb && !wn && bq && !br && !bb)
	{
		if (bn == 1)
		{
			return 1;
		}
	}
	else if (bq && !br && !bb && !bn && wq && !wr && !wb)
	{
		if (wn == 1)
		{
			return 1;
		}
	}
	return 0;
}
