//
//demon draw.cpp
//

#include "bitboard.h"
#include "define.h"
#include "protocol.h"
#include "search.h"

//vars
bitboard position_list[MAX_GAME];

int draw_by_rep( int ply )
    {
    int i;
    int reps = 1;

    for ( i = move_number + ply - 2; i >= 0; i -= 2 )
        if (position_list[move_number + ply] == position_list[i])
            {
            reps++;

            if (reps >= 3)
                return 1;
            }

    return 0;
    }

int is_repeated( int ply )
    {
    int i;

    for ( i = move_number + ply - 2; i >= 0; i -= 2 )
        if (position_list[move_number + ply] == position_list[i])
            return 1;
    return 0;
    }
