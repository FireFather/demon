//
//demon movegen.cpp
//

#include "define.h"
#include "check.h"
#include "bitboard.h"
#include "move.h"
#include "movegen.h"

//vars

bitboard from_map;
bitboard to_map;
bitboard all_pieces;
bitboard not_all_pieces;
char from_square;
char to_square;
unsigned int *ml;
unsigned int hold;

int gen_all( unsigned int *move_list )
    {
    ml = move_list;

    if( WHITE_TURN(board->turn) )
        {
        gen_white_pawn_moves();
        gen_white_knight_moves();
        gen_white_bishop_queen_moves();
        gen_white_rook_queen_moves();
        gen_white_king_moves();

        gen_white_pawn_captures();
        gen_white_knight_captures();
        gen_white_bishop_queen_captures();
        gen_white_rook_queen_captures();
        gen_white_king_captures();
        }
    else
        {
        gen_black_pawn_moves();
        gen_black_knight_moves();
        gen_black_bishop_queen_moves();
        gen_black_rook_queen_moves();
        gen_black_king_moves();

        gen_black_pawn_captures();
        gen_black_knight_captures();
        gen_black_bishop_queen_captures();
        gen_black_rook_queen_captures();
        gen_black_king_captures();
        }
    return ml - move_list;
    }

int gen_moves( unsigned int *move_list )
    {
    ml = move_list;

    if( WHITE_TURN(board->turn) )
        {
        gen_white_pawn_moves();
        gen_white_knight_moves();
        gen_white_bishop_queen_moves();
        gen_white_rook_queen_moves();
        gen_white_king_moves();
        }
    else
        {
        gen_black_pawn_moves();
        gen_black_knight_moves();
        gen_black_bishop_queen_moves();
        gen_black_rook_queen_moves();
        gen_black_king_moves();
        }
    return ml - move_list;
    }

int gen_captures( unsigned int *move_list )
    {
    ml = move_list;

    if( WHITE_TURN(board->turn) )
        {
        gen_white_pawn_captures();
        gen_white_knight_captures();
        gen_white_bishop_queen_captures();
        gen_white_rook_queen_captures();
        gen_white_king_captures();
        }
    else
        {
        gen_black_pawn_captures();
        gen_black_knight_captures();
        gen_black_bishop_queen_captures();
        gen_black_rook_queen_captures();
        gen_black_king_captures();
        }
    return ml - move_list;
    }

void gen_white_pawn_moves()
    {
    if( board->ep )
        {
        if( ((board->white_pawns &zero_left) << 9) & (mask[board->ep]) )
            {
            *(ml++) = (board->ep - 9) | white_pawn_shift_left_12 | (board->ep << 6) | (BLACK_PAWN << 16) | (4 << 20);
            }

        if( ((board->white_pawns &zero_right) << 7) & (mask[board->ep]) )
            {
            *(ml++) = (board->ep - 7) | white_pawn_shift_left_12 | (board->ep << 6) | (BLACK_PAWN << 16) | (4 << 20);
            }
        }
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    to_map = (board->white_pawns << 8) & not_all_pieces & ~rank[7];
    hold = (white_pawn_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = (to_square - 8) | hold | (to_square << 6);

        if( to_square < 24 && board->square[to_square + 8] == EMPTY )
            {
            *(ml++) = (to_square - 8) | hold | ((to_square + 8) << 6);
            }
        }
    }

void gen_black_pawn_moves()
    {
    if( board->ep )
        {
        if( ((board->black_pawns &zero_left) >> 7) & (mask[board->ep]) )
            {
            *(ml++) = (board->ep + 7) | black_pawn_shift_left_12 | (board->ep << 6) | (WHITE_PAWN << 16) | (4 << 20);
            }

        if( ((board->black_pawns &zero_right) >> 9) & (mask[board->ep]) )
            {
            *(ml++) = (board->ep + 9) | black_pawn_shift_left_12 | (board->ep << 6) | (WHITE_PAWN << 16) | (4 << 20);
            }
        }
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    to_map = (board->black_pawns >> 8) & not_all_pieces & ~rank[0];
    hold = (black_pawn_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = (to_square + 8) | hold | (to_square << 6);

        if( to_square > 39 && board->square[to_square - 8] == EMPTY )
            {
            *(ml++) = (to_square + 8) | hold | ((to_square - 8) << 6);
            }
        }
    }

void gen_white_knight_moves()
    {
    from_map = board->white_knights;
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map = knight_moves[from_square] & not_all_pieces;
        hold = from_square | (white_knight_shift_left_12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_black_knight_moves()
    {
    from_map = board->black_knights;
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map = knight_moves[from_square] & not_all_pieces;
        hold = from_square | (black_knight_shift_left_12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_white_bishop_queen_moves()
    {
    from_map = board->white_bishops | board->white_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            move_rotate_135[from_square][(board->rotate_135
                >> rotate_135_shift[from_square]) & rotate_135_mask[from_square]]
                | move_rotate_45[from_square][(board->rotate_45
                    >> rotate_45_shift[from_square]) & rotate_45_mask[from_square]];
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_black_bishop_queen_moves()
    {
    from_map = board->black_bishops | board->black_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            move_rotate_135[from_square][(board->rotate_135
                >> rotate_135_shift[from_square]) & rotate_135_mask[from_square]]
                | move_rotate_45[from_square][(board->rotate_45
                    >> rotate_45_shift[from_square]) & rotate_45_mask[from_square]];
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_white_rook_queen_moves()
    {
    from_map = board->white_rooks | board->white_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            rook_move_horiz[from_square][(all_pieces >> horiz_shift[from_square]) & 0xff]
                | rook_move_vert[from_square][(board->rotate_90 >> vert_shift[from_square]) & 0xff];
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_black_rook_queen_moves()
    {
    from_map = board->black_rooks | board->black_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            rook_move_horiz[from_square][(all_pieces >> horiz_shift[from_square]) & 0xff]
                | rook_move_vert[from_square][(board->rotate_90 >> vert_shift[from_square]) & 0xff];
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6);
            }
        }
    }

void gen_white_king_moves()
    {
    if( board->castle & 4 && !board->square[F1] && !board->square[G1] && board->square[H1] == WHITE_ROOK
        && !WHITE_KR_has_moved )
        if( !attack(1, E1) && !attack(1, F1) && !attack(1, G1) )
            {
            *(ml++) = E1 | (G1 << 6) | (white_king_shift_left_12) | (1 << 20);
            }

    if( board->castle & 8 && !board->square[D1] && !board->square[C1] && !board->square[B1]
        && board->square[A1] == WHITE_ROOK && !WHITE_QR_has_moved )
        if( !attack(1, E1) && !attack(1, D1) && !attack(1, C1) )
            {
            *(ml++) = E1 | (C1 << 6) | (white_king_shift_left_12) | (2 << 20);
            }

    from_square = board->white_king;
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    to_map = king_moves[from_square] & not_all_pieces;
    hold = from_square | (white_king_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = hold | (to_square << 6);
        }
    }

void gen_black_king_moves()
    {
    if( board->castle & 1 && !board->square[F8] && !board->square[G8] && board->square[H8] == BLACK_ROOK
        && !BLACK_KR_has_moved )
        if( !attack(-1, E8) && !attack(-1, F8) && !attack(-1, G8) )
            {
            *(ml++) = E8 | (G8 << 6) | (black_king_shift_left_12) | (1 << 20);
            }

    if( board->castle & 2 && !board->square[D8] && !board->square[C8] && !board->square[B8]
        && board->square[A8] == BLACK_ROOK && !BLACK_QR_has_moved )
        if( !attack(-1, E8) && !attack(-1, D8) && !attack(-1, C8) )
            {
            *(ml++) = E8 | (C8 << 6) | (black_king_shift_left_12) | (2 << 20);
            }
    from_square = board->black_king;
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    to_map = king_moves[from_square] & not_all_pieces;
    hold = from_square | (black_king_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = hold | (to_square << 6);
        }
    }

void gen_white_pawn_captures()
    {
    to_map = ((board->white_pawns &zero_left) << 9) & (board->black_pieces);
    hold = (white_pawn_shift_left_12);
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];

        if( to_square >= H8 )
            {
            *(ml++) = (to_square - 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (1 << 23);
            *(ml++) = (to_square - 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (2 << 23);
            *(ml++) = (to_square - 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (3 << 23);
            *(ml++) = (to_square - 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (4 << 23);
            }
        else
            *(ml++) = (to_square - 9) | hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    to_map = ((board->white_pawns &zero_right) << 7) & (board->black_pieces);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];

        if( to_square >= H8 )
            {
            *(ml++) = (to_square - 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (1 << 23);
            *(ml++) = (to_square - 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (2 << 23);
            *(ml++) = (to_square - 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (3 << 23);
            *(ml++) = (to_square - 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (4 << 23);
            }
        else
            *(ml++) = (to_square - 7) | hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    to_map = (board->white_pawns << 8) & not_all_pieces & rank[7];

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = (to_square - 8) | hold | (to_square << 6) | (1 << 23);
        *(ml++) = (to_square - 8) | hold | (to_square << 6) | (2 << 23);
        *(ml++) = (to_square - 8) | hold | (to_square << 6) | (3 << 23);
        *(ml++) = (to_square - 8) | hold | (to_square << 6) | (4 << 23);
        }
    }

void gen_black_pawn_captures()
    {
    to_map = ((board->black_pawns &zero_left) >> 7) & (board->white_pieces);
    hold = (black_pawn_shift_left_12);
    all_pieces = board->black_pieces | board->white_pieces;
    not_all_pieces = ~all_pieces;

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];

        if( to_square <= A1 )
            {
            *(ml++) = (to_square + 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (1 << 23);
            *(ml++) = (to_square + 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (2 << 23);
            *(ml++) = (to_square + 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (3 << 23);
            *(ml++) = (to_square + 7) | hold | (to_square << 6) | (board->square[to_square] << 16) | (4 << 23);
            }
        else
            *(ml++) = (to_square + 7) | hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    to_map = ((board->black_pawns &zero_right) >> 9) & (board->white_pieces);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];

        if( to_square <= A1 )
            {
            *(ml++) = (to_square + 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (1 << 23);
            *(ml++) = (to_square + 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (2 << 23);
            *(ml++) = (to_square + 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (3 << 23);
            *(ml++) = (to_square + 9) | hold | (to_square << 6) | (board->square[to_square] << 16) | (4 << 23);
            }
        else
            *(ml++) = (to_square + 9) | hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    to_map = (board->black_pawns >> 8) & not_all_pieces & rank[0];

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = (to_square + 8) | hold | (to_square << 6) | (1 << 23);
        *(ml++) = (to_square + 8) | hold | (to_square << 6) | (2 << 23);
        *(ml++) = (to_square + 8) | hold | (to_square << 6) | (3 << 23);
        *(ml++) = (to_square + 8) | hold | (to_square << 6) | (4 << 23);
        }
    }

void gen_white_knight_captures()
    {
    from_map = board->white_knights;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map = knight_moves[from_square]&(board->black_pieces);
        hold = from_square | (white_knight_shift_left_12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_black_knight_captures()
    {
    from_map = board->black_knights;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map = knight_moves[from_square]&(board->white_pieces);
        hold = from_square | (black_knight_shift_left_12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_white_bishop_queen_captures()
    {
    from_map = board->white_bishops | board->white_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            (attack_rotate_135[from_square][(board->rotate_135
                >> rotate_135_shift[from_square]) & rotate_135_mask[from_square]]
                | attack_rotate_45[from_square][(board->rotate_45
                    >> rotate_45_shift[from_square]) & rotate_45_mask[from_square]]) & board->black_pieces;
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_black_bishop_queen_captures()
    {
    from_map = board->black_bishops | board->black_queens;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            (attack_rotate_135[from_square][(board->rotate_135
                >> rotate_135_shift[from_square]) & rotate_135_mask[from_square]]
                | attack_rotate_45[from_square][(board->rotate_45
                    >> rotate_45_shift[from_square]) & rotate_45_mask[from_square]]) & board->white_pieces;
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_white_rook_queen_captures()
    {
    from_map = board->white_rooks | board->white_queens;
    all_pieces = board->black_pieces | board->white_pieces;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            (rook_attack_horiz[from_square][(all_pieces >> horiz_shift[from_square]) & 0xff]
                | rook_attack_vert[from_square][(board->rotate_90
                    >> vert_shift[from_square]) & 0xff]) & board->black_pieces;
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_black_rook_queen_captures()
    {
    from_map = board->black_rooks | board->black_queens;
    all_pieces = board->black_pieces | board->white_pieces;

    while( from_map )
        {
        from_square = lsb(from_map);
        from_map &= not_mask[from_square];
        to_map =
            (rook_attack_horiz[from_square][(all_pieces >> horiz_shift[from_square]) & 0xff]
                | rook_attack_vert[from_square][(board->rotate_90
                    >> vert_shift[from_square]) & 0xff]) & board->white_pieces;
        hold = from_square | (board->square[from_square] << 12);

        while( to_map )
            {
            to_square = lsb(to_map);
            to_map &= not_mask[to_square];
            *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
            }
        }
    }

void gen_white_king_captures()
    {
    from_square = board->white_king;
    to_map = king_moves[from_square]&(board->black_pieces);
    hold = from_square | (white_king_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    }

void gen_black_king_captures()
    {
    from_square = board->black_king;
    to_map = king_moves[from_square]&(board->white_pieces);
    hold = from_square | (black_king_shift_left_12);

    while( to_map )
        {
        to_square = lsb(to_map);
        to_map &= not_mask[to_square];
        *(ml++) = hold | (to_square << 6) | (board->square[to_square] << 16);
        }
    }
