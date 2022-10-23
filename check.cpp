//
//demon check.cpp
//

#include <stdlib.h>

#include "define.h"
#include "bitops.h"
#include "bitboard.h"
#include "move.h"
#include "movegen.h"
#include "move.h"

int check( int side )
    {
    if (side > 0)
        {
        int king = board->black_king;
        return pop_count(((attack_rotate_135[king][(board->rotate_135 >> rotate_135_shift[king]) & rotate_135_mask[king]]
            | attack_rotate_45[king][(board->rotate_45 >> rotate_45_shift[king]) & rotate_45_mask[king]]) & (board->white_bishops
            | board->white_queens))
            | ((rook_attack_horiz[king][((board->white_pieces | board->black_pieces) >> horiz_shift[king]) & 0xff]
            | rook_attack_vert[king][(board->rotate_90 >> vert_shift[king]) & 0xff]) & (board->white_rooks
            | board->white_queens))
            | (knight_moves[king]&board->white_knights) | (pawn_attack_white[king]&board->white_pawns)
            | (king_moves[king]&(mask[board->white_king])));
        }
    else
        {
        int king = board->white_king;
        return pop_count(((attack_rotate_135[king][(board->rotate_135 >> rotate_135_shift[king]) & rotate_135_mask[king]]
            | attack_rotate_45[king][(board->rotate_45 >> rotate_45_shift[king]) & rotate_45_mask[king]]) & (board->black_bishops
            | board->black_queens))
            | ((rook_attack_horiz[king][((board->white_pieces | board->black_pieces) >> horiz_shift[king]) & 0xff]
            | rook_attack_vert[king][(board->rotate_90 >> vert_shift[king]) & 0xff]) & (board->black_rooks
            | board->black_queens))
            | (knight_moves[king]&board->black_knights) | (pawn_attack_black[king]&board->black_pawns)
            | (king_moves[king]&(mask[board->black_king])));
        }
    }

int attack( int side, int sq )
    {
    if (side < 0)
        {
        return pop_count(((attack_rotate_135[sq][(board->rotate_135 >> rotate_135_shift[sq]) & rotate_135_mask[sq]]
            | attack_rotate_45[sq][(board->rotate_45 >> rotate_45_shift[sq]) & rotate_45_mask[sq]]) & (board->white_bishops
            | board->white_queens))
            | ((rook_attack_horiz[sq][((board->white_pieces | board->black_pieces)
              >> horiz_shift[sq]) & 0xff] | rook_attack_vert[sq][(board->rotate_90
              >> vert_shift[sq]) & 0xff]) & (board->white_rooks
            | board->white_queens))
            | (knight_moves[sq]&board->white_knights)
            | (((((mask[sq]) &zero_left) >> 7) | (((mask[sq]) &zero_right) >> 9)) & board->white_pawns)
            | (king_moves[sq]&(mask[board->white_king])));
        }
    else
        {
        return pop_count(((attack_rotate_135[sq][(board->rotate_135 >> rotate_135_shift[sq]) & rotate_135_mask[sq]]
            | attack_rotate_45[sq][(board->rotate_45 >> rotate_45_shift[sq]) & rotate_45_mask[sq]]) & (board->black_bishops
            | board->black_queens))
            | ((rook_attack_horiz[sq][((board->white_pieces | board->black_pieces)
              >> horiz_shift[sq]) & 0xff] | rook_attack_vert[sq][(board->rotate_90
              >> vert_shift[sq]) & 0xff]) & (board->black_rooks
            | board->black_queens))
            | (knight_moves[sq]&board->black_knights)
            | (((((mask[sq]) &zero_right) << 7) | (((mask[sq]) &zero_left) << 9)) & board->black_pawns)
            | (king_moves[sq]&(mask[board->black_king])));
        }
    }

int check_mate( int side, int ply )
    {
    if (side > 0)
        {
        unsigned int *move_list = (unsigned int *)malloc(MAX_MOVES * sizeof(unsigned int));
        int num = gen_all(move_list);
        unsigned int *temp = move_list;

        for ( int i = 0; i < num; i++ )
            {
            make_move(*temp, ply);

            if (!check(1))
                {
                un_make_move(*temp, ply);
                free(move_list);
                return 0;
                }
            else
                un_make_move(*temp, ply);
            temp++;
            }
        free(move_list);
        return 1;
        }
    else
        {
        unsigned int *move_list = (unsigned int *)malloc(MAX_MOVES * sizeof(unsigned int));
        int num = gen_all(move_list);
        unsigned int *temp = move_list;

        for ( int i = 0; i < num; i++ )
            {
            make_move(*temp, ply);

            if (!check(-1))
                {
                un_make_move(*temp, ply);
                free(move_list);
                return 0;
                }
            else
                un_make_move(*temp, ply);
            temp++;
            }
        free(move_list);
        return 1;
        }
    }
