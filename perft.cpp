#include <iostream>

#include "perft.h"
#include "bitboard.h"
#include "define.h"
#include "move.h"
#include "movegen.h"
#include "protocol.h"
#include "strings.h"

using namespace std;

bitboard perft_nodes = 0;

void do_perft(int depth)
{
	depth--;
	const auto move_list = static_cast<unsigned int*>(malloc(MAX_MOVES * sizeof(unsigned int)));
	const int num = gen_all(move_list);

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

void perft(const int depth)
{
	perft_nodes = 0;
	start_time = clock();
	do_perft(depth);
	const double time = clock() - start_time;
	cout << "depth: " << depth << "\n";
	cout << "nodes: ";
	cout << perft_nodes << "\n";
	cout << "time : ";
	cout.setf(ios::fixed);
	cout.precision(2);
	cout << time / 1000.0 << "\n";
	if (time)
		cout << "speed: " << static_cast<int>(perft_nodes * 1000.0 / time) / 1000 << " kN/s";
	cout << endl;
}

void divide(const int depth)
{
	const auto move_list = static_cast<unsigned int*>(malloc(MAX_MOVES * sizeof(unsigned int)));
	const int num = gen_all(move_list);

	cout << "divide depth " << depth << "\n";
	cout << "move: ";
	cout << "nodes";
	cout << "\n";

	perft_nodes = 0;
	start_time = clock();

	for (int i = 0; i < num; i++)
	{
		const uint64_t start_nodes = perft_nodes;
		const unsigned int m = move_list[i];
		make_move(move_list[i], 0);
		if (depth - 1)
			do_perft(depth - 1);
		else perft_nodes++;
		un_make_move(move_list[i], 0);
		const auto coord_move = to_long_algebraic(board, m);
		cout << coord_move;
		cout << ": ";
		cout << perft_nodes - start_nodes << "\n";
	}
	const double time = static_cast<float>(clock() - start_time);
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
		cout << static_cast<int>(perft_nodes * 1000.0 / time) / 1000 << " kN/s" << "\n";
	}
	cout << endl;
}
