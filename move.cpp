//
//demon move.cpp
//

#include "bitboard.h"
#include "define.h"
#include "hash.h"
#include "legal.h"
#include "movegen.h"
#include "options.h"
#include "search.h"
#include "see.h"

//vars

int promote = 4;
bool white_has_castled = false;
bool black_has_castled = false;
bool WHITE_KR_has_moved = false;
bool WHITE_QR_has_moved = false;
bool BLACK_KR_has_moved = false;
bool BLACK_QR_has_moved = false;
unsigned int killer[2][MAX_DEPTH];

int piece_num[15] =
    {
    0, 0, 1, 2, 3, 4, 5, 0, 0, 6, 7, 8, 9, 10, 11
    };

int promote_values[5] =
    {
    0, knight_value, bishop_value, rook_value, queen_value
    };

int piece_values[15] =
    {
    0, rook_value, bishop_value, queen_value, knight_value, pawn_value, 5000, 0, 0, rook_value, bishop_value,
        queen_value, knight_value, pawn_value, 5000
    };

void make_move( unsigned int move, int ply )
    {
    int piece = PIECE(move);
    int from = FROM(move);
    int to = TO(move);
    int captured = CAPTURED(move);
    int special = SPECIAL(move);

    ply_info[ply].key = board->key;
    ply_info[ply].score = board->score;
    ply_info[ply].castle = board->castle;
    ply_info[ply].ep = board->ep;
    board->key ^= hash_data[from][piece_num[piece]];
    board->key ^= hash_data[to][piece_num[piece]];

    if( captured )
        board->key ^= hash_data[to][piece_num[captured]];
    board->square[from] = EMPTY;
    board->square[to] = piece;
    bitboard temp;
    temp = mask[from] | mask[to];
    board->rotate_90 &= not_mask[rotate_90[from]];
    board->rotate_90 |= mask[rotate_90[to]];
    board->rotate_135 &= not_mask[rotate_135[from]];
    board->rotate_135 |= mask[rotate_135[to]];
    board->rotate_45 &= not_mask[rotate_45[from]];
    board->rotate_45 |= mask[rotate_45[to]];
    board->key ^= ep_modifier[board->ep];
    board->ep = 0;

    if( WHITE_PIECE(piece) )
        {
        board->white_pieces ^= temp;
        board->black_pieces &= not_mask[to];

        switch( piece )
            {
            case WHITE_KNIGHT:
                board->white_knights ^= temp;
                break;

            case WHITE_BISHOP:
                board->white_bishops ^= temp;
                break;

            case WHITE_ROOK:
                board->white_rooks ^= temp;
                if( from == H1 && (board->castle & 4) )
                    {
                    board->castle &= 11 + 48;
                    board->key ^= castle_modifier[2];
                    }
                else if( from == A1 && (board->castle & 8) )
                    {
                    board->castle &= 7 + 48;
                    board->key ^= castle_modifier[3];
                    }
                if( from == H1 )
                    WHITE_KR_has_moved = true;
                if( from == A1 )
                    WHITE_QR_has_moved = true;
                break;

            case WHITE_QUEEN:
                board->white_queens ^= temp;
                break;

            case WHITE_PAWN:
                if( to >= H8 )
                    {
                    switch( promote )
                        {
                        case 1:
                            board->white_knights |= mask[to];
                            board->key ^= hash_data[to][piece_num[WHITE_KNIGHT]];
                            board->square[to] = WHITE_KNIGHT;
                            break;

                        case 2:
                            board->white_bishops |= mask[to];
                            board->key ^= hash_data[to][piece_num[WHITE_BISHOP]];
                            board->square[to] = WHITE_BISHOP;
                            break;

                        case 3:
                            board->white_rooks |= mask[to];
                            board->key ^= hash_data[to][piece_num[WHITE_ROOK]];
                            board->square[to] = WHITE_ROOK;
                            break;

                        case 4:
                            board->white_queens |= mask[to];
                            board->key ^= hash_data[to][piece_num[WHITE_QUEEN]];
                            board->square[to] = WHITE_QUEEN;
                            break;
                        }
                    board->white_pawns &= not_mask[from];
                    board->key ^= hash_data[to][piece_num[WHITE_PAWN]];
                    board->score -= promote_values[promote] - pawn_value;
                    }
                else
                    {
                    board->white_pawns ^= temp;

                    if( (to - from) == 16 )
                        {
                        if( (FILE(to) > 0 && board->square[to - 1] == BLACK_PAWN)
                            || (FILE(to) < 7 && board->square[to + 1] == BLACK_PAWN) )
                            {
                            board->ep = from + 8;
                            board->key ^= ep_modifier[board->ep];
                            }
                        }
                    }
                break;

            case WHITE_KING:
                if( special == 1 )
                    {
                    board->castle |= 32;
                    board->key ^= hash_data[H1][piece_num[WHITE_ROOK]];
                    board->key ^= hash_data[F1][piece_num[WHITE_ROOK]];
                    board->square[H1] = EMPTY;
                    board->square[F1] = WHITE_ROOK;
                    temp = mask[H1] | mask[F1];
                    board->white_rooks ^= temp;
                    board->white_pieces ^= temp;
                    board->rotate_90 &= not_mask[rotate_90[H1]];
                    board->rotate_90 |= mask[rotate_90[F1]];
                    board->rotate_135 &= not_mask[rotate_135[H1]];
                    board->rotate_135 |= mask[rotate_135[F1]];
                    board->rotate_45 &= not_mask[rotate_45[H1]];
                    board->rotate_45 |= mask[rotate_45[F1]];
                    white_has_castled = true;
                    }
                else if( special == 2 )
                    {
                    board->castle |= 32;
                    board->key ^= hash_data[A1][piece_num[WHITE_ROOK]];
                    board->key ^= hash_data[D1][piece_num[WHITE_ROOK]];
                    board->square[A1] = EMPTY;
                    board->square[D1] = WHITE_ROOK;
                    temp = mask[A1] | mask[D1];
                    board->white_rooks ^= temp;
                    board->white_pieces ^= temp;
                    board->rotate_90 &= not_mask[rotate_90[A1]];
                    board->rotate_90 |= mask[rotate_90[D1]];
                    board->rotate_135 &= not_mask[rotate_135[A1]];
                    board->rotate_135 |= mask[rotate_135[D1]];
                    board->rotate_45 &= not_mask[rotate_45[A1]];
                    board->rotate_45 |= mask[rotate_45[D1]];
                    white_has_castled = true;
                    }
                board->white_king = to;
                if( board->castle & 4 )
                    {
                    board->key ^= castle_modifier[2];
                    }
                if( board->castle & 8 )
                    {
                    board->key ^= castle_modifier[3];
                    }
                board->castle &= 3 + 48;
                break;
            }

        switch( captured )
            {
            case BLACK_KNIGHT:
                board->score -= knight_value;
                board->black_knights &= not_mask[to];
                break;

            case BLACK_BISHOP:
                board->score -= bishop_value;
                board->black_bishops &= not_mask[to];
                break;

            case BLACK_ROOK:
                board->score -= rook_value;
                board->black_rooks &= not_mask[to];
                if( from == H8 && (board->castle & 1) )
                    {
                    board->castle &= 14 + 48;
                    board->key ^= castle_modifier[0];
                    }
                else if( from == A8 && (board->castle & 2) )
                    {
                    board->castle &= 13 + 48;
                    board->key ^= castle_modifier[1];
                    }
                if( from == H8 )
                    BLACK_KR_has_moved = true;
                if( from == A8 )
                    BLACK_QR_has_moved = true;
                break;

            case BLACK_QUEEN:
                board->score -= queen_value;
                board->black_queens &= not_mask[to];
                break;

            case BLACK_PAWN:
                if( special == 4 )
                    {
                    int capSQ = to - 8;
                    board->key ^= hash_data[to][piece_num[captured]];
                    board->key ^= hash_data[capSQ][piece_num[captured]];
                    board->square[capSQ] = EMPTY;
                    board->black_pawns &= not_mask[capSQ];
                    board->black_pieces &= not_mask[capSQ];
                    board->rotate_90 &= not_mask[rotate_90[capSQ]];
                    board->rotate_135 &= not_mask[rotate_135[capSQ]];
                    board->rotate_45 &= not_mask[rotate_45[capSQ]];
                    }
                else
                    {
                    board->black_pawns &= not_mask[to];
                    }
                board->score -= pawn_value;
                break;
            }
        }
    else
        {
        board->black_pieces ^= temp;
        board->white_pieces &= not_mask[to];

        switch( piece )
            {
            case BLACK_KNIGHT:
                board->black_knights ^= temp;
                break;

            case BLACK_BISHOP:
                board->black_bishops ^= temp;
                break;

            case BLACK_ROOK:
                board->black_rooks ^= temp;
                if( from == H8 && (board->castle & 1) )
                    {
                    board->castle &= 14 + 48;
                    board->key ^= castle_modifier[0];
                    }
                else if( from == A8 && (board->castle & 2) )
                    {
                    board->castle &= 13 + 48;
                    board->key ^= castle_modifier[1];
                    }
                if( from == H8 )
                    BLACK_KR_has_moved = true;
                if( from == A8 )
                    BLACK_QR_has_moved = true;
                break;

            case BLACK_QUEEN:
                board->black_queens ^= temp;
                break;

            case BLACK_PAWN:
                if( to <= A1 )
                    {
                    switch( promote )
                        {
                        case 1:
                            board->black_knights |= mask[to];
                            board->key ^= hash_data[to][piece_num[BLACK_KNIGHT]];
                            board->square[to] = BLACK_KNIGHT;
                            break;

                        case 2:
                            board->black_bishops |= mask[to];
                            board->key ^= hash_data[to][piece_num[BLACK_BISHOP]];
                            board->square[to] = BLACK_BISHOP;
                            break;

                        case 3:
                            board->black_rooks |= mask[to];
                            board->key ^= hash_data[to][piece_num[BLACK_ROOK]];
                            board->square[to] = BLACK_ROOK;
                            break;

                        case 4:
                            board->black_queens |= mask[to];
                            board->key ^= hash_data[to][piece_num[BLACK_QUEEN]];
                            board->square[to] = BLACK_QUEEN;
                            break;
                        }
                    board->black_pawns &= not_mask[from];
                    board->key ^= hash_data[to][piece_num[BLACK_PAWN]];
                    board->score += promote_values[promote] - pawn_value;
                    }
                else
                    {
                    board->black_pawns ^= temp;
                    }
                if( (from - to) == 16 )
                    {
                    if( (FILE(to) > 0 && board->square[to - 1] == WHITE_PAWN)
                        || (FILE(to) < 7 && board->square[to + 1] == WHITE_PAWN) )
                        {
                        board->ep = from - 8;
                        board->key ^= ep_modifier[board->ep];
                        }
                    }
                break;

            case BLACK_KING:
                if( special == 1 )
                    {
                    board->castle |= 16;
                    board->key ^= hash_data[H8][piece_num[BLACK_ROOK]];
                    board->key ^= hash_data[F8][piece_num[BLACK_ROOK]];
                    board->square[H8] = EMPTY;
                    board->square[F8] = BLACK_ROOK;
                    temp = mask[H8] | mask[F8];
                    board->black_rooks ^= temp;
                    board->black_pieces ^= temp;
                    board->rotate_90 &= not_mask[rotate_90[H8]];
                    board->rotate_90 |= mask[rotate_90[F8]];
                    board->rotate_135 &= not_mask[rotate_135[H8]];
                    board->rotate_135 |= mask[rotate_135[F8]];
                    board->rotate_45 &= not_mask[rotate_45[H8]];
                    board->rotate_45 |= mask[rotate_45[F8]];
                    black_has_castled = true;
                    }
                else if( special == 2 )
                    {
                    board->castle |= 16;
                    board->key ^= hash_data[A8][piece_num[BLACK_ROOK]];
                    board->key ^= hash_data[D8][piece_num[BLACK_ROOK]];
                    board->square[A8] = EMPTY;
                    board->square[D8] = BLACK_ROOK;
                    temp = mask[A8] | mask[D8];
                    board->black_rooks ^= temp;
                    board->black_pieces ^= temp;
                    board->rotate_90 &= not_mask[rotate_90[A8]];
                    board->rotate_90 |= mask[rotate_90[D8]];
                    board->rotate_135 &= not_mask[rotate_135[A8]];
                    board->rotate_135 |= mask[rotate_135[D8]];
                    board->rotate_45 &= not_mask[rotate_45[A8]];
                    board->rotate_45 |= mask[rotate_45[D8]];
                    black_has_castled = true;
                    }
                board->black_king = to;
                if( board->castle & 1 )
                    {
                    board->key ^= castle_modifier[0];
                    }
                if( board->castle & 2 )
                    {
                    board->key ^= castle_modifier[1];
                    }
                board->castle &= 12 + 48;
                break;
            }

        switch( captured )
            {
            case WHITE_KNIGHT:
                board->score += knight_value;
                board->white_knights &= not_mask[to];
                break;

            case WHITE_BISHOP:
                board->score += bishop_value;
                board->white_bishops &= not_mask[to];
                break;

            case WHITE_ROOK:
                board->score += rook_value;
                board->white_rooks &= not_mask[to];
                if( from == H1 && (board->castle & 4) )
                    {
                    board->castle &= 11 + 48;
                    board->key ^= castle_modifier[2];
                    }
                else if( from == A1 && (board->castle & 8) )
                    {
                    board->castle &= 7 + 48;
                    board->key ^= castle_modifier[3];
                    }
                if( from == H1 )
                    WHITE_KR_has_moved = true;
                if( from == A1 )
                    WHITE_QR_has_moved = true;
                break;

            case WHITE_QUEEN:
                board->score += queen_value;
                board->white_queens &= not_mask[to];
                break;

            case WHITE_PAWN:
                if( special == 4 )
                    {
                    int capSQ = to + 8;
                    board->key ^= hash_data[to][piece_num[captured]];
                    board->key ^= hash_data[capSQ][piece_num[captured]];
                    board->square[capSQ] = EMPTY;
                    board->white_pawns &= not_mask[capSQ];
                    board->white_pieces &= not_mask[capSQ];
                    board->rotate_90 &= not_mask[rotate_90[capSQ]];
                    board->rotate_135 &= not_mask[rotate_135[capSQ]];
                    board->rotate_45 &= not_mask[rotate_45[capSQ]];
                    }
                else
                    {
                    board->white_pawns &= not_mask[to];
                    }
                board->score += pawn_value;
                break;
            }
        }
    board->turn = SWITCH_TURN(board->turn);
    }

void un_make_move( unsigned int move, int ply )
    {
    int piece = PIECE(move);
    int from = FROM(move);
    int to = TO(move);
    int captured = CAPTURED(move);
    int special = SPECIAL(move);
    int promote = 4;

    board->key = ply_info[ply].key;
    board->score = ply_info[ply].score;
    board->castle = ply_info[ply].castle;
    board->ep = ply_info[ply].ep;
    board->square[from] = piece;
    board->square[to] = captured;
    bitboard temp;
    temp = mask[from] | mask[to];
    board->rotate_90 |= mask[rotate_90[from]];
    board->rotate_135 |= mask[rotate_135[from]];
    board->rotate_45 |= mask[rotate_45[from]];

    if( !captured )
        {
        board->rotate_90 &= not_mask[rotate_90[to]];
        board->rotate_135 &= not_mask[rotate_135[to]];
        board->rotate_45 &= not_mask[rotate_45[to]];
        }

    if( WHITE_PIECE(piece) )
        {
        board->white_pieces ^= temp;

        if( captured )
            board->black_pieces |= mask[to];

        switch( piece )
            {
            case WHITE_KNIGHT:
                board->white_knights ^= temp;
                break;

            case WHITE_BISHOP:
                board->white_bishops ^= temp;
                break;

            case WHITE_ROOK:
                board->white_rooks ^= temp;
                if( to == H1 )
                    WHITE_KR_has_moved = false;
                if( to == A1 )
                    WHITE_QR_has_moved = false;
                break;

            case WHITE_QUEEN:
                board->white_queens ^= temp;
                break;

            case WHITE_PAWN:
                if( to >= H8 )
                    {
                    switch( promote )
                        {
                        case 1:
                            board->white_knights &= not_mask[to];
                            break;

                        case 2:
                            board->white_bishops &= not_mask[to];
                            break;

                        case 3:
                            board->white_rooks &= not_mask[to];
                            break;

                        case 4:
                            board->white_queens &= not_mask[to];
                            break;
                        }
                    board->white_pawns |= mask[from];
                    }
                else
                    {
                    board->white_pawns ^= temp;
                    }
                break;

            case WHITE_KING:
                if( special == 1 )
                    {
                    board->square[H1] = WHITE_ROOK;
                    board->square[F1] = EMPTY;
                    temp = mask[H1] | mask[F1];
                    board->white_rooks ^= temp;
                    board->white_pieces ^= temp;
                    board->rotate_90 |= mask[rotate_90[H1]];
                    board->rotate_90 &= not_mask[rotate_90[F1]];
                    board->rotate_135 |= mask[rotate_135[H1]];
                    board->rotate_135 &= not_mask[rotate_135[F1]];
                    board->rotate_45 |= mask[rotate_45[H1]];
                    board->rotate_45 &= not_mask[rotate_45[F1]];
                    white_has_castled = false;
                    }
                else if( special == 2 )
                    {
                    board->square[A1] = WHITE_ROOK;
                    board->square[D1] = EMPTY;
                    temp = mask[A1] | mask[D1];
                    board->white_rooks ^= temp;
                    board->white_pieces ^= temp;
                    board->rotate_90 |= mask[rotate_90[A1]];
                    board->rotate_90 &= not_mask[rotate_90[D1]];
                    board->rotate_135 |= mask[rotate_135[A1]];
                    board->rotate_135 &= not_mask[rotate_135[D1]];
                    board->rotate_45 |= mask[rotate_45[A1]];
                    board->rotate_45 &= not_mask[rotate_45[D1]];
                    white_has_castled = false;
                    }
                board->white_king = from;
                break;
            }

        switch( captured )
            {
            case BLACK_KNIGHT:
                board->black_knights |= mask[to];
                break;

            case BLACK_BISHOP:
                board->black_bishops |= mask[to];
                break;

            case BLACK_ROOK:
                board->black_rooks |= mask[to];
                if( to == H8 )
                    BLACK_KR_has_moved = false;
                if( to == A8 )
                    BLACK_QR_has_moved = false;
                break;

            case BLACK_QUEEN:
                board->black_queens |= mask[to];
                break;

            case BLACK_PAWN:
                if( special == 4 )
                    {
                    int capSQ = to - 8;
                    board->rotate_90 &= not_mask[rotate_90[to]];
                    board->rotate_135 &= not_mask[rotate_135[to]];
                    board->rotate_45 &= not_mask[rotate_45[to]];
                    board->square[to] = EMPTY;
                    board->black_pieces &= not_mask[to];
                    board->black_pawns |= mask[capSQ];
                    board->square[capSQ] = BLACK_PAWN;
                    board->black_pieces |= mask[capSQ];
                    board->rotate_90 |= mask[rotate_90[capSQ]];
                    board->rotate_135 |= mask[rotate_135[capSQ]];
                    board->rotate_45 |= mask[rotate_45[capSQ]];
                    }
                else
                    {
                    board->black_pawns |= mask[to];
                    }
                break;
            }
        }
    else
        {
        board->black_pieces ^= temp;

        if( captured )
            board->white_pieces |= mask[to];

        switch( piece )
            {
            case BLACK_KNIGHT:
                board->black_knights ^= temp;
                break;

            case BLACK_BISHOP:
                board->black_bishops ^= temp;
                break;

            case BLACK_ROOK:
                board->black_rooks ^= temp;
                if( to == H8 )
                    BLACK_KR_has_moved = false;
                if( to == A8 )
                    BLACK_QR_has_moved = false;
                break;

            case BLACK_QUEEN:
                board->black_queens ^= temp;
                break;

            case BLACK_PAWN:
                if( to <= A1 )
                    {
                    switch( promote )
                        {
                        case 1:
                            board->black_knights &= not_mask[to];
                            break;

                        case 2:
                            board->black_bishops &= not_mask[to];
                            break;

                        case 3:
                            board->black_rooks &= not_mask[to];
                            break;

                        case 4:
                            board->black_queens &= not_mask[to];
                            break;
                        }
                    board->black_pawns |= mask[from];
                    }
                else
                    {
                    board->black_pawns ^= temp;
                    }
                break;

            case BLACK_KING:
                if( special == 1 )
                    {
                    board->square[H8] = BLACK_ROOK;
                    board->square[F8] = EMPTY;
                    temp = mask[H8] | mask[F8];
                    board->black_rooks ^= temp;
                    board->black_pieces ^= temp;
                    board->rotate_90 |= mask[rotate_90[H8]];
                    board->rotate_90 &= not_mask[rotate_90[F8]];
                    board->rotate_135 |= mask[rotate_135[H8]];
                    board->rotate_135 &= not_mask[rotate_135[F8]];
                    board->rotate_45 |= mask[rotate_45[H8]];
                    board->rotate_45 &= not_mask[rotate_45[F8]];
                    black_has_castled = false;
                    }
                else if( special == 2 )
                    {
                    board->square[A8] = BLACK_ROOK;
                    board->square[D8] = EMPTY;
                    temp = mask[A8] | mask[D8];
                    board->black_rooks ^= temp;
                    board->black_pieces ^= temp;
                    board->rotate_90 |= mask[rotate_90[A8]];
                    board->rotate_90 &= not_mask[rotate_90[D8]];
                    board->rotate_135 |= mask[rotate_135[A8]];
                    board->rotate_135 &= not_mask[rotate_135[D8]];
                    board->rotate_45 |= mask[rotate_45[A8]];
                    board->rotate_45 &= not_mask[rotate_45[D8]];
                    black_has_castled = false;
                    }
                board->black_king = from;
                break;
            }

        switch( captured )
            {
            case WHITE_KNIGHT:
                board->white_knights |= mask[to];
                break;

            case WHITE_BISHOP:
                board->white_bishops |= mask[to];
                break;

            case WHITE_ROOK:
                board->white_rooks |= mask[to];
                if( to == H1 )
                    WHITE_KR_has_moved = false;
                if( to == A1 )
                    WHITE_QR_has_moved = false;
                break;

            case WHITE_QUEEN:
                board->white_queens |= mask[to];
                break;

            case WHITE_PAWN:
                if( special == 4 )
                    {
                    int capSQ = to + 8;
                    board->rotate_90 &= not_mask[rotate_90[to]];
                    board->rotate_135 &= not_mask[rotate_135[to]];
                    board->rotate_45 &= not_mask[rotate_45[to]];
                    board->square[to] = EMPTY;
                    board->white_pieces &= not_mask[to];
                    board->square[capSQ] = WHITE_PAWN;
                    board->white_pawns |= mask[capSQ];
                    board->white_pieces |= mask[capSQ];
                    board->rotate_90 |= mask[rotate_90[capSQ]];
                    board->rotate_135 |= mask[rotate_135[capSQ]];
                    board->rotate_45 |= mask[rotate_45[capSQ]];
                    }
                else
                    {
                    board->white_pawns |= mask[to];
                    }
                break;
            }
        }
    board->turn = SWITCH_TURN(board->turn);
    }

unsigned int next_move( int ply, int side )
    {
    unsigned int ret_move;
    int high;
    int i;
    int h_index = 0;
    int temp;
    int spot;

    switch( ply_info[ply].gen_stage )
        {
        case 0:
            ply_info[ply].gen_stage = 1;
            if( ply_info[ply].hash_move )
                return ply_info[ply].hash_move;

        case 1:
            ply_info[ply].gen_stage = 2;
            ply_info[ply].num = gen_captures(ply_info[ply].move_list);
            for ( i = 0; i < ply_info[ply].num; i++ )
                if( ply_info[ply].move_list[i] == ply_info[ply].hash_move )
                    {
                    ply_info[ply].move_list[i] = ply_info[ply].move_list[--ply_info[ply].num];
                    break;
                    }
            ply_info[ply].num_lose = 0;
            for ( i = ply_info[ply].num - 1; i >= 0; i-- )
                {
                sort[i] = see(FROM(ply_info[ply].move_list[i]), TO(ply_info[ply].move_list[i]));

                if( SPECIAL(ply_info[ply].move_list[i]) == 4 )
                    sort[i] += pawn_value;
                sort[i] += promote_values[PROMOTE(ply_info[ply].move_list[i])];

                if( sort[i] < 0 )
                    {
                    ply_info[ply].lose_list[ply_info[ply].num_lose++] = ply_info[ply].move_list[i];
                    ply_info[ply].move_list[i] = ply_info[ply].move_list[--ply_info[ply].num];
                    sort[i] = sort[ply_info[ply].num];
                    }
                }
            spot = ply_info[ply].num;
            while( 1 )
                {
                high = -1;

                for ( i = --spot; i >= 0; i-- )
                    {
                    if( sort[i] > high )
                        {
                        high = sort[i];
                        h_index = i;
                        }
                    }

                if( high == -1 )
                    break;
                ret_move = ply_info[ply].move_list[spot];
                ply_info[ply].move_list[spot] = ply_info[ply].move_list[h_index];
                ply_info[ply].move_list[h_index] = ret_move;

                temp = sort[spot];
                sort[spot] = sort[h_index];
                sort[h_index] = temp;
                }

        case 2:
            if( ply_info[ply].num )
                return ply_info[ply].move_list[--ply_info[ply].num];
            ply_info[ply].gen_stage = 3;

        case 3:
            ply_info[ply].gen_stage = 4;
            if( killer[0][ply] != ply_info[ply].hash_move && legal(killer[0][ply]) )
                {
                ply_info[ply].killer_1 = killer[0][ply];
                return killer[0][ply];
                }

        case 4:
            ply_info[ply].gen_stage = 5;
            if( killer[1][ply] != ply_info[ply].hash_move && legal(killer[1][ply]) )
                {
                ply_info[ply].killer_2 = killer[1][ply];
                return killer[1][ply];
                }

        case 5:
            ply_info[ply].gen_stage = 6;
            ply_info[ply].num_history = 0;
            ply_info[ply].num = gen_moves(ply_info[ply].move_list);
            for ( i = 0; i < ply_info[ply].num; i++ )
                if( ply_info[ply].move_list[i]
                    && (ply_info[ply].move_list[i] == ply_info[ply].hash_move
                        || ply_info[ply].move_list[i] == ply_info[ply].killer_1
                        || ply_info[ply].move_list[i] == ply_info[ply].killer_2) )
                    ply_info[ply].move_list[i] = 0;

        case 6:
            ply_info[ply].num_history++;
            high = -1;
            for ( i = 0; i < ply_info[ply].num; i++ )
                if( ply_info[ply].move_list[i] )
                    {
                    temp =
                        side
                            > 0
                                ? history_black[FROM(ply_info[ply].move_list[i])][TO(ply_info[ply].move_list[i])]
                                : history_white[FROM(ply_info[ply].move_list[i])][TO(ply_info[ply].move_list[i])];

                    if( temp > high )
                        {
                        high = temp;
                        h_index = i;
                        }
                    }
            if( high == -1 )
                {
                ply_info[ply].gen_stage = 8;
                return 0;
                }
            if( ply_info[ply].num_history >= HISTORY_SCANS )
                ply_info[ply].gen_stage = 7;
            ret_move = ply_info[ply].move_list[h_index];
            ply_info[ply].move_list[h_index] = 0;
            return ret_move;

        case 7:
            while( --ply_info[ply].num >= 0 && !ply_info[ply].move_list[ply_info[ply].num] );
            if( ply_info[ply].num >= 0 )
                return ply_info[ply].move_list[ply_info[ply].num];
            ply_info[ply].gen_stage = 8;

        case 8:
            if( ply_info[ply].num_lose )
                return ply_info[ply].lose_list[--ply_info[ply].num_lose];
            ply_info[ply].gen_stage = 9;
        }
    return 0;
    }
