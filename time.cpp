//
//demon time.cpp
//

#include "bitboard.h"
#include "define.h"
#include "protocol.h"

double get_move_time()
    {
    double inc = 0;
    double tpm = 0;
    double move_time = 0;
    double time_left = 0;
    double num_moves = 0;

    if( search_time > 0 )
        {
        move_time = search_time;
        return move_time;
        }

    if( WHITE_TURN(board->turn) )
        {
        time_left = wtime;
        inc = winc;
        }
    else
        {
        time_left = btime;
        inc = binc;
        }

    if( moves_to_go == 0 )
        {
        if( move_number <= 60 )
            num_moves = (100 - move_number) / 2;
        else
            num_moves = 20;

        tpm = (time_left) / num_moves;

        move_time = tpm / 2 + inc / 2;
        }
    else
        {
        move_time = (time_left / moves_to_go) * 3 / 4 + inc * 3 / 4;
        }

    return move_time;
    }
