//
//demon sort.cpp
//

#include "define.h"
#include "hash.h"
#include "move.h"
#include "options.h"
#include "search.h"
#include "see.h"

int sort_list(unsigned int* move_list, const int side, const int num)
{
	const unsigned int* mv = move_list;
	unsigned int temp;
	int i;
	int spot = 0;
	const unsigned int hash_move = get_move(side);

	for (i = 0; i < num; i++)
		if (move_list[i] == hash_move)
		{
			temp = move_list[0];
			move_list[0] = move_list[i];
			move_list[i] = temp;
			spot++;
		}

	int high;
	int h_index = 0;
	int sc;

	while (true)
	{
		high = -LIMIT;

		for (i = spot; i < num; i++)
			if (PROMOTE(move_list[i])
				&& (sc = promote_values[PROMOTE(move_list[i])] + see(FROM(move_list[i]), TO(move_list[i]))) > high)
			{
				high = sc;
				h_index = i;
			}
			else if (CAPTURED(move_list[i])
				&& (sc = (SPECIAL(move_list[i]) == 4 ? pawn_value : 0) + see(FROM(move_list[i]), TO(move_list[i]))) > high)
			{
				high = sc;
				h_index = i;
			}

		if (high == -LIMIT)
			break;
		temp = move_list[spot];
		move_list[spot] = move_list[h_index];
		move_list[h_index] = temp;
		spot++;
	}

	while (true)
	{
		high = -LIMIT;

		for (i = spot; i < num; i++)
			if ((BLACK_SIDE(side)
				     ? history_black[FROM(move_list[i])][TO(move_list[i])]
				     : history_white[FROM(move_list[i])][TO(move_list[i])])
				> high)
			{
				high =
					BLACK_SIDE(side)
						? history_black[FROM(move_list[i])][TO(move_list[i])]
						: history_white[FROM(move_list[i])][TO(move_list[i])];
				h_index = i;
			}

		if (high == -LIMIT)
			break;
		temp = move_list[spot];
		move_list[spot] = move_list[h_index];
		move_list[h_index] = temp;
		spot++;
	}
	return mv - move_list;
}

void do_history(const int ply, const int depth, const int side, const unsigned int move)
{
	if (CAPTURED(move) || PROMOTE(move))
		return;

	if (BLACK_SIDE(side))
		history_black[FROM(move)][TO(move)] += depth * depth;
	else
		history_white[FROM(move)][TO(move)] += depth * depth;

	if (killer[0][ply] != move)
	{
		killer[1][ply] = killer[0][ply];
		killer[0][ply] = move;
	}
}
