//
//demon hash.cpp
//

#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"
#include "define.h"
#include "hash.h"
#include "move.h"
#include "options.h"
#include "protocol.h"
#include "util.h"

//vars

int hash_entries;
hash_entry *smart_hash;
hash_entry *basic_hash;
bitboard hash_data[64][12];
bitboard ep_modifier[64];
bitboard castle_modifier[4];

void init_hash()
    {
    int size = hash_size;

    if( size < 8 )
        size = 8;

    if( size > 1024 )
        size = 1024;

    size *= 1024 * 1024 / 2;

    smart_hash = (hash_entry *)malloc(size * sizeof(hash_entry) / 4);
    basic_hash = (hash_entry *)malloc(size * sizeof(hash_entry) / 4);

    hash_entries = size / sizeof(hash_entry);

    int q;
    int t;

    for ( q = 0; q < 64; q++ )
        for ( t = 0; t < 12; t++ )
            hash_data[q][t] = get_random_64();

    for ( q = 0; q < 64; q++ )
        ep_modifier[q] = 0;

    for ( q = H2; q <= A2; q++ )
        ep_modifier[q] = get_random_64();

    for ( q = H5; q <= A5; q++ )
        ep_modifier[q] = get_random_64();

    for ( q = 0; q < 4; q++ )
        castle_modifier[q] = get_random_64();

    for ( q = 0; q < hash_entries; q++ )
        {
        smart_hash[q].key = 0;
        basic_hash[q].key = 0;
        }
    }

void free_hash()
    {
    free(basic_hash);
    free(smart_hash);
    }

void clear_hash()
    {
    int q;
    int t;

    for ( q = 0; q < 64; q++ )
        for ( t = 0; t < 12; t++ )
            hash_data[q][t] = NULL;

    for ( q = 0; q < 64; q++ )
        ep_modifier[q] = NULL;

    for ( q = H2; q <= A2; q++ )
        ep_modifier[q] = NULL;

    for ( q = H5; q <= A5; q++ )
        ep_modifier[q] = NULL;

    for ( q = 0; q < 4; q++ )
        castle_modifier[q] = NULL;

    for ( q = 0; q < hash_entries; q++ )
        {
        smart_hash[q].key = 0;
        basic_hash[q].key = 0;
        }
    free_hash();
    }

bitboard hash_board( bit_board *brd )
    {
    int q;
    bitboard hash_val = 0;

    for ( q = 0; q < 64; q++ )
        if( brd->square[q] != EMPTY )
            hash_val ^= hash_data[q][piece_num[brd->square[q]]];
    hash_val ^= ep_modifier[brd->ep];

    if( !(board->castle & 1) )
        hash_val ^= castle_modifier[0];

    if( !(board->castle & 2) )
        hash_val ^= castle_modifier[1];

    if( !(board->castle & 4) )
        hash_val ^= castle_modifier[2];

    if( !(board->castle & 8) )
        hash_val ^= castle_modifier[3];
    return hash_val;
    }

void set_hash( int score, int ply, int depth, char tag, unsigned int move, int side )
    {
    switch( tag )
        {
        case EQUAL_TRUE:
            if( score > NEAR_MATE )
                score += ply;

            else if( score < -NEAR_MATE )
                score -= ply;
            break;

        case BELOW_TRUE:
            score = (score < NEAR_MATE) ? score : NEAR_MATE;
            break;

        case ABOVE_TRUE:
            score = (score > -NEAR_MATE) ? score : -NEAR_MATE;
        }
    bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;
    bitboard hash_index = key &(hash_entries - 1);

    if( smart_hash[hash_index].key != key || smart_hash[hash_index].old || smart_hash[hash_index].depth < depth
        || (smart_hash[hash_index].depth == depth
            && (tag == EQUAL_TRUE
                || (tag == BELOW_TRUE && smart_hash[hash_index].tag == BELOW_TRUE
                    && score > smart_hash[hash_index].score)
                || (tag == ABOVE_TRUE && smart_hash[hash_index].tag == ABOVE_TRUE
                    && score < smart_hash[hash_index].score))) )
        {
        basic_hash[hash_index] = smart_hash[hash_index];

        smart_hash[hash_index].key = key;
        smart_hash[hash_index].old = 0;
        smart_hash[hash_index].tag = tag;
        smart_hash[hash_index].depth = depth;
        smart_hash[hash_index].score = score;
        smart_hash[hash_index].move_to_try = move;
        }
    else
        {
        basic_hash[hash_index].key = key;
        basic_hash[hash_index].tag = tag;
        basic_hash[hash_index].depth = depth;
        basic_hash[hash_index].score = score;
        basic_hash[hash_index].move_to_try = move;
        }
    }

int get_hash( int ply, int depth, int *alpha, int *beta, int side )
    {
    bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;
    bitboard hash_index = key &(hash_entries - 1);
    int t1;
    char return_tag = NIX;

    if( smart_hash[hash_index].key == key )
        do
            {
            smart_hash[hash_index].old = 0;

            if( smart_hash[hash_index].tag == ABOVE_TRUE
                && smart_hash[hash_index].depth >= (depth - NULL_REDUCTION(depth))
                && smart_hash[hash_index].score < *beta )
                return_tag = NULL_FAILS;

            if( smart_hash[hash_index].depth < depth )
                break;

            switch( smart_hash[hash_index].tag )
                {
                case EQUAL_TRUE:
                    t1 = smart_hash[hash_index].score;
                    if( t1 > NEAR_MATE )
                        t1 -= ply;

                    else if( t1 < -NEAR_MATE )
                        t1 += ply;
                    *alpha = t1;
                    return EQUAL_TRUE;

                case ABOVE_TRUE:
                    if( smart_hash[hash_index].score <= *alpha )
                        {
                        *alpha = smart_hash[hash_index].score;
                        return ABOVE_TRUE;
                        }
                    break;

                case BELOW_TRUE:
                    if( smart_hash[hash_index].score >= *beta )
                        {
                        *beta = smart_hash[hash_index].score;
                        return BELOW_TRUE;
                        }
                }
            } while ( 0 );

    if( basic_hash[hash_index].key == key )
        do
            {
            if( basic_hash[hash_index].tag == ABOVE_TRUE
                && basic_hash[hash_index].depth >= (depth - NULL_REDUCTION(depth))
                && basic_hash[hash_index].score < *beta )
                return_tag = NULL_FAILS;

            if( basic_hash[hash_index].depth < depth )
                break;

            switch( basic_hash[hash_index].tag )
                {
                case EQUAL_TRUE:
                    t1 = basic_hash[hash_index].score;
                    if( t1 > NEAR_MATE )
                        t1 -= ply;

                    else if( t1 < -900000 )
                        t1 += ply;
                    *alpha = t1;
                    return EQUAL_TRUE;

                case ABOVE_TRUE:
                    if( basic_hash[hash_index].score <= *alpha )
                        {
                        *alpha = basic_hash[hash_index].score;
                        return ABOVE_TRUE;
                        }
                    break;

                case BELOW_TRUE:
                    if( basic_hash[hash_index].score >= *beta )
                        {
                        *beta = basic_hash[hash_index].score;
                        return BELOW_TRUE;
                        }
                }
            } while ( 0 );
    return return_tag;
    }

unsigned int get_move( int side )
    {
    bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;
    bitboard hash_index = key &(hash_entries - 1);

    if( smart_hash[hash_index].key == key )
        {
        smart_hash[hash_index].old = 0;
        return smart_hash[hash_index].move_to_try;
        }

    if( basic_hash[hash_index].key == key )
        {
        return basic_hash[hash_index].move_to_try;
        }
    return 0;
    }
