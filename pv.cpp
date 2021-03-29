//
//demon pv.cpp
//

#include "bitboard.h"
#include "hash.h"
#include "legal.h"
#include "search.h"
#include "move.h"
#include "protocol.h"
#include "pv.h"
#include "string.h"
#include "util.h"

void fill_pv( int ply, int depth, int side )
    {
    if( ply == pv_length[0] )
        {
        return;
        }

    if( legal(pv[0][ply]) )
        {
        set_hash(0, ply, depth, NIX, pv[0][ply], side);
        make_move(pv[0][ply], ply);
        fill_pv(ply + 1, depth - ONE_PLY, CHANGE_SIDE(side));
        un_make_move(pv[0][ply], ply);
        }
    }

void print_pv( int score, int depth, int side )
    {
    int i;
    int nps;
    int time_used;

    secs = ((float)(clock() - start_time)) / CLOCKS_PER_SEC;

    if( secs == 0 )
        secs = 1;
    time_used = (int)(secs * 1000);
    nps = nodes / time_used * 1000;

    output("info depth %d score cp %d time %d nodes %d nps %d %s ", depth, score, time_used, nodes, nps, "pv");

    for ( i = 0; i < pv_length[0]; i++ )
        output("%s ", to_long_algebraic(board, pv[0][i]));
    output("\n");
    }
