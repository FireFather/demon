//
//demon see.cpp
//

#include "bitboard.h"
#include "define.h"
#include "move.h"

int see( int from_sq, int to_sq )
    {
    bitboard attack;
    int score[32];
    int next_value = piece_values[board->square[from_sq]];
    int count = 1;
    int turn = SWITCH_TURN(board->turn);
    score[0] = piece_values[board->square[to_sq]];

    attack =
        ((attack_rotate_135[to_sq][(board->rotate_135 >> rotate_135_shift[to_sq]) & rotate_135_mask[to_sq]]
            | attack_rotate_45[to_sq][(board->rotate_45 >> rotate_45_shift[to_sq]) & rotate_45_mask[to_sq]]) & (board->
            white_bishops
            | board->black_bishops | board->white_queens | board->black_queens))
            | ((rook_attack_horiz[to_sq][((board->white_pieces | board->black_pieces) >> horiz_shift[to_sq]) & 0xff]
                | rook_attack_vert[to_sq][(board->rotate_90 >> vert_shift[to_sq]) & 0xff]) & (board->white_rooks
                | board->black_rooks | board->white_queens | board->black_queens))
                | (knight_moves[to_sq]&(board->white_knights | board->black_knights))
                | (pawn_attack_white[to_sq]&board->white_pawns) | (pawn_attack_black[to_sq]&board->black_pawns)
                | (king_moves[to_sq]&(mask[board->black_king] | mask[board->white_king]));

    attack &= not_mask[from_sq];

    while( attack )
        {
        if( turn )
            {
            if( board->black_pawns & attack )
                from_sq = lsb(board->black_pawns & attack);

            else if( board->black_bishops & attack )
                from_sq = lsb(board->black_bishops & attack);

            else if( board->black_knights & attack )
                from_sq = lsb(board->black_knights & attack);

            else if( board->black_rooks & attack )
                from_sq = lsb(board->black_rooks & attack);

            else if( board->black_queens & attack )
                from_sq = lsb(board->black_queens & attack);

            else if( mask[board->black_king] & attack )
                from_sq = board->black_king;

            else
                break;
            }
        else
            {
            if( board->white_pawns & attack )
                from_sq = lsb(board->white_pawns & attack);

            else if( board->white_bishops & attack )
                from_sq = lsb(board->white_bishops & attack);

            else if( board->white_knights & attack )
                from_sq = lsb(board->white_knights & attack);

            else if( board->white_rooks & attack )
                from_sq = lsb(board->white_rooks & attack);

            else if( board->white_queens & attack )
                from_sq = lsb(board->white_queens & attack);

            else if( mask[board->white_king] & attack )
                from_sq = board->white_king;

            else
                break;
            }
        attack &= not_mask[from_sq];
        score[count] = -score[count - 1] + next_value;
        next_value = piece_values[board->square[from_sq]];
        turn = SWITCH_TURN(turn);
        count++;
        }

    while( --count )
        if( score[count - 1] > -score[count] )
            score[count - 1] = -score[count];
    return score[0];
    }
