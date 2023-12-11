//
//demon time.cpp
//

#include <algorithm>

#include "bitboard.h"
#include "define.h"
#include "protocol.h"

double get_move_time()
{
	double inc;
	double movetime;
	double time_left;

	if (WHITE_TURN(board->turn))
	{
		time_left = wtime;
		inc = winc;
	}
	else
	{
		time_left = btime;
		inc = binc;
	}

	if (moves_to_go == 0)
	{
		double num_moves;
		if (move_number <= 60)
			num_moves = (static_cast<double>(100) - move_number) / 2;
		else
			num_moves = 20;

		const double tpm = time_left / num_moves;

		movetime = tpm / 2 + inc / 2;
	}
	else
	{
		const auto k = 1 + 60.0 * static_cast<double>(move_number) / (500.0 + static_cast<double>(move_number));
		const auto ratio = 0.17 * (k + 0.00 / time_left);
		movetime = static_cast<int>(std::min(1.0, ratio) * time_left);
	}

	return movetime;
}
