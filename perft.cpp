#include <iostream>

using namespace std;

#include "bitboard.h"
#include "define.h"
#include "check.h"
#include "draw.h"
#include "eval.h"
#include "hash.h"
#include "init.h"
#include "legal.h"
#include "search.h"
#include "sort.h"
#include "move.h"
#include "movegen.h"
#include "options.h"
#include "protocol.h"
#include "pv.h"
#include "see.h"
#include "string.h"
#include "util.h"
#include "perft.h"

bitboard perft_nodes = 0;

void do_perft(int depth)
{
	depth--;
	unsigned int *move_list = (unsigned int *)malloc(MAX_MOVES * sizeof(unsigned int));
	int num = gen_all(move_list);

	for (int i = 0; i < num; i++)
	{
		make_move(move_list[i], 0);
		if (depth)
			do_perft(depth);
		else perft_nodes++;
		un_make_move(move_list[i], depth);
	}
	free(move_list);
}

void perft(int depth)
{
	perft_nodes = 0;
	start_time = clock();
	do_perft(depth);
	double time = ((float)(clock() - start_time));
	cout << "depth: " << int(depth) << "\n";
	cout << "nodes: ";
	cout << perft_nodes << "\n";
	cout << "time : ";
	cout.setf(ios::fixed);
	cout.precision(2);
	cout << time / 1000.0 << "\n";
	if (time)
		cout << "speed: " << int(perft_nodes * 1000.0 / time) / 1000 << " kN/s";
	cout << endl;
}

void divide(int depth) {
	unsigned int *move_list = (unsigned int *)malloc(MAX_MOVES * sizeof(unsigned int));
	int num = gen_all(move_list);

	cout << "divide depth " << depth << "\n";
	cout << "move: ";
	cout << "nodes";
	cout << "\n";

	perft_nodes = 0;
	unsigned __int64 start_nodes = perft_nodes;
	start_time = clock();

	for (int i = 0; i < num; i++)
	{
		start_nodes = perft_nodes;
		unsigned int m = move_list[i];
		make_move(move_list[i], 0);
		if (depth-1)
			do_perft(depth-1);
		else perft_nodes++;
		un_make_move(move_list[i], 0);
		char *coord_move = (char *)malloc(6);
		coord_move = to_long_algebraic(board, m);
		cout << coord_move;
		cout << ": ";
		cout << perft_nodes - start_nodes << "\n";
	}
	double time = ((float)(clock() - start_time));
	cout << "-------------\n";
	cout << "total nodes: " << perft_nodes << "\n";
	cout << "total time : ";

	cout.width(4);
	cout.setf(ios::fixed);
	cout.precision(2);
	if (time)
	{
		cout << time / 1000.0 << "\n";
		cout << "total speed: ";
		cout << int(perft_nodes * 1000.0 / time) / 1000 << " kN/s" << "\n";
	}
	cout << endl;
}

