//
//demon pv.cpp
//

#include "pv.h"

#include "bitboard.h"
#include "define.h"
#include "hash.h"
#include "legal.h"
#include "move.h"
#include "protocol.h"
#include "search.h"
#include "strings.h"
#include "util.h"

void fill_pv(const int ply, const int depth, const int side)
{
	if (ply == pv_length[0])
	{
		return;
	}

	if (legal(pv[0][ply]))
	{
		set_hash(0, ply, depth, NIX, pv[0][ply], side);
		make_move(pv[0][ply], ply);
		fill_pv(ply + 1, depth - ONE_PLY, CHANGE_SIDE(side));
		un_make_move(pv[0][ply], ply);
	}
}

void print_pv(const int score, const int depth, int side)
{
	secs = static_cast<float>(clock() - start_time) / CLOCKS_PER_SEC;
	if (secs == 0)
		secs = 1;
	const int time_used = static_cast<int>(secs * 1000);
	const int nps = nodes / time_used * 1000;

	output("info depth %d score cp %d time %d nodes %d nps %d %s ", depth, score, time_used, nodes, nps, "pv");

	for (int i = 0; i < pv_length[0]; i++)
		output("%s ", to_long_algebraic(board, pv[0][i]));
	output("\n");
}
