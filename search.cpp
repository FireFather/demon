//
//demon search.cpp
//
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "bitops.h"
#include "bitboard.h"
#include "define.h"
#include "check.h"
#include "draw.h"
#include "eval.h"
#include "hash.h"
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

#define WINDOW 400

using namespace std;

//vars
int nodes;
double secs;
int last_nodes;
int curr_depth;
int initial_depth;
int sort[MAX_MOVES];
unsigned int best_move;
int pv_length[MAX_DEPTH];
int history_white[64][64];
int history_black[64][64];
plyinfo ply_info[MAX_DEPTH];
unsigned int pv[MAX_DEPTH][MAX_DEPTH];

//iterate
void iterate( int side, int depth )
    {
    int i;
    int j;
    int alpha = 0;
    int beta = 0;
    int score = 0;
	unsigned int temp;
    bitboard temp_count;
    int start_depth = 1;
    bitboard big_num = 1;
    unsigned int prev_best = pv[0][0];
    bitboard *node_count = (bitboard *)malloc(MAX_MOVES * sizeof(bitboard));

    nodes = 0;
    last_nodes = 0;
	secs = 0;
	big_num <<= 63;

    if (draw_by_rep(0))
        {
        output("1/2-1/2 {draw by repetition}\n");
        return;
        }

    ply_info[0].in_check = 0;

	search_clear();

    unsigned int *move_list = (unsigned int *) &ply_info[0].move_list;
    int num = gen_all(move_list);

    for ( i = 0; i < num; i++ )
        {
        make_move(move_list[i], 0);

        if (check(side))
            {
            un_make_move(move_list[i], 0);
            move_list[i--] = move_list[--num];
            }
        else
            un_make_move(move_list[i], 0);
        }

    if (num == 0)
        {
        if (check(side))
            {
            if (side == 1)
                {
                output("1-0 {white mates}\n");
                }
            else
                {
                output("0-1 {black mates}\n");
                }
            }
        else
            {
            output("1/2-1/2 {stalemate}\n");
            }
        return;
        }
    else if (num == 1)
        {
        best_move = move_list[0];
        return;
        }

	sort_list( move_list, side, num );

    for ( curr_depth = start_depth; curr_depth <= depth; curr_depth++ )
        {
        initial_depth = curr_depth;

        if (curr_depth > start_depth)
            {
            alpha = score - WINDOW;
            beta = score + WINDOW;
            }
        else
            {
            alpha = -LIMIT;
            beta = LIMIT;
            }

        do
            {
            score = root_search(side, curr_depth * ONE_PLY, move_list, node_count, num, alpha, beta);

            if (score == -MAX)
                break;

            if (score <= alpha)
                {
                beta = alpha + 1;
                alpha = -LIMIT;
                score = root_search(side, curr_depth * ONE_PLY, move_list, node_count, num, alpha, beta);

                if (score == -MAX)
                    break;

                if (score >= beta)
                    {
                    beta = LIMIT;
                    score = root_search(side, curr_depth * ONE_PLY, move_list, node_count, num, alpha, beta);
                    }
                }
            else if (score >= beta)
                {
                alpha = beta - 1;
                beta = LIMIT;
                score = root_search(side, curr_depth * ONE_PLY, move_list, node_count, num, alpha, beta);

                if (score == -MAX)
                    break;

                if (score <= alpha)
                    {
                    alpha = -LIMIT;
                    score = root_search(side, curr_depth * ONE_PLY, move_list, node_count, num, alpha, beta);
                    }
                }
            } while ( 0 );

        if (score != -MAX)
            {

            for ( i = 0; i < num; i++ )
                if (move_list[i] == pv[0][0])
                    node_count[i] = big_num;

            for ( i = 0; i < num; i++ )
                for ( j = i + 1; j < num; j++ )
                    if (node_count[i] < node_count[j])
                        {
                        temp = move_list[i];
                        move_list[i] = move_list[j];
                        move_list[j] = temp;
                        temp_count = node_count[i];
                        node_count[i] = node_count[j];
                        node_count[j] = temp_count;
                        }

            fill_pv(0, curr_depth * ONE_PLY, side);
			print_pv( score, curr_depth, side); 

            prev_best = best_move;

            if (score > NEAR_MATE)
                {
                secs = ((float)(clock() - start_time)) / CLOCKS_PER_SEC;

                if (secs > 1)
                    {
                    nodes_per_check = nodes;
                    nodes_per_check = (int)(nodes_per_check / secs);
                    }
                free(node_count);
				return;
                }
            }
        else
            {
            best_move = prev_best;
            secs = ((float)(clock() - start_time)) / CLOCKS_PER_SEC;

            if (secs > 1)
                {
                nodes_per_check = nodes;
                nodes_per_check = (int)(nodes_per_check / secs);
                }

            free(node_count);
			return;
            }
        }

	return;
    }

int search( int ply, int depth, int side, int alpha, int beta, int null_move )
    {
    nodes++;
	while (InputAvailable())
	{
		char str[128];
		cin.getline(str, 128);
		if (!*str) break;

		if (!strcmp(str, "isready"))
		{
			cout << "readyok\n";
			cout.flush();
		}
		else if (!strcmp(str, "stop"))
			return -MAX;
		else if (!strcmp(str, "ponderhit"))
		{
		}
	}
    if ((nodes - last_nodes) > nodes_per_check)
        {
        if (((clock() - start_time) >= move_time_in_cps))
            {
            return -MAX;
            }
        else
            last_nodes = nodes;
        }
    position_list[move_number + ply] = board->key;
    pv_length[ply] = ply;

    if (ply >= MAX_DEPTH - 1)
        return beta;

    if (null_move && is_repeated(ply))
        {
        return 0;
        }

    int i;
    char null_fails = 0;

    switch( get_hash(ply, depth, &alpha, &beta, side) )
        {
        case EQUAL_TRUE:

        case ABOVE_TRUE:
            return alpha;

        case BELOW_TRUE:
            return beta;

        case NULL_FAILS:
            null_fails = 1;
        }

    int value;
    int null_threat = 0;

    if (null_move && !null_fails && !ply_info[ply].in_check)
        {
        int zugzwang = 1;

        if (BLACK_SIDE(side))
            {
            if (board->black_queens || pop_count(board->black_rooks | board->black_bishops | board->black_knights) > 1)
                zugzwang = 0;
            }
        else
            {
            if (board->white_queens || pop_count(board->white_rooks | board->white_bishops | board->white_knights) > 1)

                zugzwang = 0;
            }

        if (!zugzwang)
            {
            ply_info[ply + 1].in_check = 0;
            ply_info[ply].move_played = 0;
            board->key ^= ep_modifier[board->ep];
            ply_info[ply].ep = board->ep;
            board->ep = 0;
            board->turn = SWITCH_TURN(board->turn);
            int r = NULL_REDUCTION(depth);

            if (depth - r >= ONE_PLY)
                {
                value = -search(ply + 1, depth - r, CHANGE_SIDE(side), -beta, 1 - beta, 0);

                if (value == MAX)
                    {
                    board->turn = SWITCH_TURN(board->turn);
                    board->ep = ply_info[ply].ep;
                    board->key ^= ep_modifier[board->ep];
                    return -MAX;
                    }
                }
            else
                value = -quiescient(ply + 1, CHANGE_SIDE(side), -beta, 1 - beta);

            if (value >= beta)
                {
                board->turn = SWITCH_TURN(board->turn);
                board->ep = ply_info[ply].ep;
                board->key ^= ep_modifier[board->ep];
                set_hash(value, ply, depth, BELOW_TRUE, 0, side);
                return value;
                }
            board->turn = SWITCH_TURN(board->turn);
            board->ep = ply_info[ply].ep;
            board->key ^= ep_modifier[board->ep];

            if (value < -NEAR_MATE)
                null_threat = 1;
            }
        }

	else if(depth <= 3 && eval( side, alpha, beta ) < beta - 300)
		{
		int value = -quiescient(ply + 1, CHANGE_SIDE(side), -beta, -alpha);
		if(value < beta)
			return value;
		}

    int old_a = alpha;
    int num = 0;
    unsigned int move;
    int ext;
    int small_ext = ply > (initial_depth << 1);

    ply_info[ply].hash_move = get_move(side);

    if (!legal(ply_info[ply].hash_move))
        ply_info[ply].hash_move = 0;

    if (!ply_info[ply].hash_move && alpha + 1 != beta && depth >= 3 * ONE_PLY)
        {
        value = search(ply, depth - 2 * ONE_PLY, side, alpha, beta, 1);

        if (value == -MAX)
            return -MAX;

        if (value <= alpha)
            {
            value = search(ply, depth - 2 * ONE_PLY, side, -LIMIT, beta, 1);

            if (value == -MAX)
                return -MAX;
            }

        if (legal(pv[ply][ply]))
            {
            ply_info[ply].hash_move = pv[ply][ply];
            }
        }

    ply_info[ply].gen_stage = 0;

    while (ply_info[ply].gen_stage < 9)
        {
        if ((move = next_move(ply, side)))
            {
            make_move(move, ply);

            if (!check(side))
                {
                ply_info[ply].move_played = move;

                ext = -ONE_PLY;

                if (null_threat)
                    ext += small_ext ? NULL_EXT >> 1 : NULL_EXT;

                if ((ply_info[ply + 1].in_check = check(CHANGE_SIDE(side))))
                    {
                    ext += small_ext ? CHECK_EXT >> 1 : CHECK_EXT;
                    }

                if ((PIECE(move) == BLACK_PAWN && TO(move) <= A2) || (PIECE(move) == WHITE_PAWN && TO(move) >= H7))
                    {
                    ext += small_ext ? PAWN_PUSH >> 1 : PAWN_PUSH;
                    }

                if (CAPTURED(ply_info[ply - 1].move_played)
                    && piece_values[CAPTURED(ply_info[ply - 1].move_played)] == piece_values[CAPTURED(move)]
                        && TO(ply_info[ply - 1].move_played) == TO(move))
                    {
                    ext += small_ext ? RECAP_EXT >> 1 : RECAP_EXT;
                    }
                ext = ext > 0 ? 0 : ext;

                if (!num)
                    {
                    if (depth + ext >= ONE_PLY)
                        {
                        value = -search(ply + 1, depth + ext, CHANGE_SIDE(side), -beta, -alpha, 1);

                        if (value == MAX)
                            {
                            un_make_move(move, ply);
                            return -MAX;
                            }
                        }
                    else
                        {
                        value = -quiescient(ply + 1, CHANGE_SIDE(side), -beta, -alpha);
                        }
                    }
                else
                    {
                    if (depth + ext >= ONE_PLY)
                        {
                        value = -search(ply + 1, depth + ext, CHANGE_SIDE(side), -alpha - 1, -alpha, 1);

                        if (value == MAX)
                            {
                            un_make_move(move, ply);
                            return -MAX;
                            }
                        }
                    else
                        {
                        value = -quiescient(ply + 1, CHANGE_SIDE(side), -alpha - 1, -alpha);
                        }

                    if (value > alpha && value < beta)
                        {
                        if (depth + ext >= ONE_PLY)
                            {
                            value = -search(ply + 1, depth + ext, CHANGE_SIDE(side), -beta, -value, 1);

                            if (value == MAX)
                                {
                                un_make_move(move, ply);
                                return -MAX;
                                }
                            }
                        else
                            {
                            value = -quiescient(ply + 1, CHANGE_SIDE(side), -beta, -value);
                            }
                        }
                    }

                if (value > alpha)
                    {
                    pv[ply][ply] = move;

                    for ( i = ply + 1; i < pv_length[ply + 1]; i++ )
                        pv[ply][i] = pv[ply + 1][i];
                    pv_length[ply] = pv_length[ply + 1];

                    if (value >= beta)
                        {
                        do_history(ply, depth, side, move);
                        un_make_move(move, ply);
                        set_hash(value, ply, depth, BELOW_TRUE, move, side);
                        return value;
                        }
                    alpha = value;
                    }
                num++;
                }
            un_make_move(move, ply);
            }
        }

    if (!num)
        {
        if (ply_info[ply].in_check)
            {
            set_hash(-MATE + ply, ply, depth, EQUAL_TRUE, 0, side);
            return (-MATE + ply);
            }
        set_hash(0, ply, depth, EQUAL_TRUE, 0, side);
        return 0;
        }

    if (alpha == old_a)
        set_hash(alpha, ply, depth, ABOVE_TRUE, 0, side);
    else
        {
        do_history(ply, depth, side, pv[ply][ply]);
        set_hash(alpha, ply, depth, EQUAL_TRUE, pv[ply][ply], side);
        }

    return alpha;
    }

int root_search( int side, int depth, unsigned int* move_list, bitboard* node_count, int num_moves, int alpha, int beta )
    {
    int nodes_before;
    pv_length[0] = 0;
    int value;
    int i;
    int curr_depth;
    ply_info[1].in_check = 0;
    curr_depth = 0;
    int ext;

    while (curr_depth < num_moves)
        {
        ply_info[0].move_played = move_list[curr_depth];
        make_move(move_list[curr_depth], 0);
        ext = -ONE_PLY;

        if ((ply_info[1].in_check = check(-side)))
            {
            ext += CHECK_EXT;
            }

        if ((PIECE(move_list[curr_depth]) == BLACK_PAWN && TO(move_list[curr_depth]) <= A2)
            || (PIECE(move_list[curr_depth]) == WHITE_PAWN && TO(move_list[curr_depth]) >= H7))
            {
            ext += PAWN_PUSH;
            }
        ext = ext > 0 ? 0 : ext;
        nodes_before = nodes;

        if (!curr_depth)
            {
            value = -search(1, depth + ext, CHANGE_SIDE(side), -beta, -alpha, 1);

            if (value == MAX)
                {
                un_make_move(move_list[curr_depth], 0);
                return -MAX;
                }
            }
        else
            {
            value = -search(1, depth + ext, CHANGE_SIDE(side), -alpha - 1, -alpha, 1);

            if (value == MAX)
                {
                un_make_move(move_list[curr_depth], 0);
                return -MAX;
                }

            if (value > alpha && value < beta)
                {
                value = -search(1, depth + ext, CHANGE_SIDE(side), -beta, -value, 1);

                if (value == MAX)
                    {
                    un_make_move(move_list[curr_depth], 0);
                    return -MAX;
                    }
                }
            }

        if (value > alpha)
            {
            pv[0][0] = move_list[curr_depth];
            best_move = move_list[curr_depth];

            for ( i = 1; i < pv_length[1]; i++ )
                pv[0][i] = pv[1][i];
            pv_length[0] = pv_length[1];

            if (value >= beta)
                {
                un_make_move(move_list[curr_depth], 0);
                return value;
                }
            alpha = value;
            }
        un_make_move(move_list[curr_depth], 0);
        node_count[curr_depth] = nodes - nodes_before;
        curr_depth++;
        }
    set_hash(alpha, 0, depth, EQUAL_TRUE, pv[0][0], side);
    return alpha;
    }

int quiescient( int ply, int side, int alpha, int beta )
    {
    nodes++;
    pv_length[ply] = ply;

    if (ply >= MAX_DEPTH - 1)
        return beta;

    int h = eval( side, alpha, beta );

    if (h > alpha)
        {
        if (h >= beta)
            {
            return h;
            }
        alpha = h;
        }
    int i, curr_depth, k;
    int value = -LIMIT;
    int num = gen_captures(ply_info[ply].move_list);

    int delta = alpha - pawn_value - side * board->score;

    for ( i = 0; i < num; i++ )
        {
        if (piece_values[CAPTURED(ply_info[ply].move_list[i])] >= delta
            || promote_values[PROMOTE(ply_info[ply].move_list[i])] >= delta)
            {
            sort[i] = see(FROM(ply_info[ply].move_list[i]), TO(ply_info[ply].move_list[i]));

            if (SPECIAL(ply_info[ply].move_list[i]) == 4)
                sort[i] += pawn_value;
            sort[i] += promote_values[PROMOTE(ply_info[ply].move_list[i])];

            if (sort[i] < 0)
                ply_info[ply].move_list[i] = 0;
            }
        else
            ply_info[ply].move_list[i] = 0;
        }

    unsigned int temp;

    for ( k = 0; k < num; k++ )
        for ( curr_depth = k + 1; curr_depth < num; curr_depth++ )
            if (sort[k] > sort[curr_depth])
                {
                temp = ply_info[ply].move_list[k];
                ply_info[ply].move_list[k] = ply_info[ply].move_list[curr_depth];
                ply_info[ply].move_list[curr_depth] = temp;
                i = sort[k];
                sort[k] = sort[curr_depth];
                sort[curr_depth] = i;
                }

    while (num > 0)
        {
        num--;

        if (ply_info[ply].move_list[num])
            {
            make_move(ply_info[ply].move_list[num], ply);

            if (!check(side))
                {
                value = -quiescient(ply + 1, CHANGE_SIDE(side), -beta, -alpha);

                if (value > alpha)
                    {
                    pv[ply][ply] = ply_info[ply].move_list[num];

                    for ( i = ply + 1; i < pv_length[ply + 1]; i++ )
                        pv[ply][i] = pv[ply + 1][i];
                    pv_length[ply] = pv_length[ply + 1];

                    if (value >= beta)
                        {
                        un_make_move(ply_info[ply].move_list[num], ply);
                        return value;
                        }
                    alpha = value;
                    }
                }
            un_make_move(ply_info[ply].move_list[num], ply);
            }
        }

    if (value == -LIMIT)
        return h;
    return alpha;
    }

void search_clear()
    {
	int i;

    for ( i = 0; i < MAX_DEPTH; i++ )
        {
        pv_length[i] = 0;

        for ( curr_depth = 0; curr_depth < MAX_DEPTH; curr_depth++ )
            {
            pv[i][curr_depth] = 0;
            }
        }

    for ( i = 0; i < 64; i++ )
        for ( curr_depth = 0; curr_depth < 64; curr_depth++ )
            {
            history_white[i][curr_depth] = 0;
            history_black[i][curr_depth] = 0;
            }
    }
