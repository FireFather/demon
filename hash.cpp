//
//demon hash.cpp
//

#include "hash.h"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>

#include "bitboard.h"
#include "define.h"
#include "move.h"
#include "protocol.h"

//vars
int hash_entries;
hash_entry* smart_hash;
hash_entry* basic_hash;
bitboard hash_data[64][12];
bitboard ep_modifier[64];
bitboard castle_modifier[4];

void init_hash()
{
	int size = hash_size;

	if (size < 8) size = 8;
	if (size > 1024) size = 1024;

	size *= 1024 * 1024 / 2;

	smart_hash = static_cast<hash_entry*>(malloc(size * sizeof(hash_entry) / 4));
	basic_hash = static_cast<hash_entry*>(malloc(size * sizeof(hash_entry) / 4));

	hash_entries = size / static_cast<int>(sizeof(hash_entry));

	std::random_device rd;
	std::mt19937_64 e2(rd());
	std::uniform_int_distribution dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));

	int q;

	for (q = 0; q < 64; q++)
		for (int t = 0; t < 12; t++)
			hash_data[q][t] = dist(e2);

	for (q = 0; q < 64; q++)
		ep_modifier[q] = 0;

	for (q = H2; q <= A2; q++)
		ep_modifier[q] = dist(e2);

	for (q = H5; q <= A5; q++)
		ep_modifier[q] = dist(e2);

	for (q = 0; q < 4; q++)
		castle_modifier[q] = dist(e2);

	for (q = 0; q < hash_entries; q++)
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

	for (q = 0; q < 64; q++)
		for (int t = 0; t < 12; t++)
			hash_data[q][t] = NULL;

	for (q = 0; q < 64; q++)
		ep_modifier[q] = NULL;

	for (q = H2; q <= A2; q++)
		ep_modifier[q] = NULL;

	for (q = H5; q <= A5; q++)
		ep_modifier[q] = NULL;

	for (q = 0; q < 4; q++)
		castle_modifier[q] = NULL;

	for (q = 0; q < hash_entries; q++)
	{
		smart_hash[q].key = 0;
		basic_hash[q].key = 0;
	}
	free_hash();
}

bitboard hash_board(const position* brd)
{
	bitboard hash_val = 0;

	for (int q = 0; q < 64; q++)
		if (brd->square[q] != EMPTY)
			hash_val ^= hash_data[q][piece_num[brd->square[q]]];
	hash_val ^= ep_modifier[brd->ep];

	if (!(board->castle & 1))
		hash_val ^= castle_modifier[0];

	if (!(board->castle & 2))
		hash_val ^= castle_modifier[1];

	if (!(board->castle & 4))
		hash_val ^= castle_modifier[2];

	if (!(board->castle & 8))
		hash_val ^= castle_modifier[3];
	return hash_val;
}

void set_hash(int score, const int ply, const int depth, const char tag, const unsigned int move, const int side)
{
	switch (tag)
	{
	case EQUAL_TRUE:

		if (score > NEAR_MATE)
			score += ply;

		else if (score < -NEAR_MATE)
			score -= ply;
		break;

	case BELOW_TRUE:
		score = score < NEAR_MATE ? score : NEAR_MATE;
		break;

	case ABOVE_TRUE:
		score = score > -NEAR_MATE ? score : -NEAR_MATE;
		break;
	default: ;
	}
	const bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;

	if (const bitboard hash_index = key & hash_entries - 1; smart_hash[hash_index].key != key || smart_hash[hash_index].old
		|| smart_hash[hash_index].depth < depth
		|| smart_hash[hash_index].depth == depth
		&& (tag == EQUAL_TRUE
			|| tag == BELOW_TRUE && smart_hash[hash_index].tag == BELOW_TRUE
			&& score > smart_hash[hash_index].score
			|| tag == ABOVE_TRUE && smart_hash[hash_index].tag == ABOVE_TRUE
			&& score < smart_hash[hash_index].score))
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

int get_hash(const int ply, const int depth, int* alpha, int* beta, const int side)
{
	const bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;
	const bitboard hash_index = key & hash_entries - 1;
	int t1;
	char return_tag = NIX;

	if (smart_hash[hash_index].key == key)
		do
		{
			smart_hash[hash_index].old = 0;

			if (smart_hash[hash_index].tag == ABOVE_TRUE
				&& smart_hash[hash_index].depth >= depth - NULL_REDUCTION(depth)
				&& smart_hash[hash_index].score < *beta)
				return_tag = NULL_FAILS;

			if (smart_hash[hash_index].depth < depth)
				break;

			switch (smart_hash[hash_index].tag)
			{
			case EQUAL_TRUE:
				t1 = smart_hash[hash_index].score;

				if (t1 > NEAR_MATE)
					t1 -= ply;

				else if (t1 < -NEAR_MATE)
					t1 += ply;
				*alpha = t1;
				return EQUAL_TRUE;

			case ABOVE_TRUE:

				if (smart_hash[hash_index].score <= *alpha)
				{
					*alpha = smart_hash[hash_index].score;
					return ABOVE_TRUE;
				}
				break;

			case BELOW_TRUE:
				if (smart_hash[hash_index].score >= *beta)
				{
					*beta = smart_hash[hash_index].score;
					return BELOW_TRUE;
				}
				break;
			default: ;
			}
		}
		while (false);

	if (basic_hash[hash_index].key == key)
		do
		{
			if (basic_hash[hash_index].tag == ABOVE_TRUE
				&& basic_hash[hash_index].depth >= depth - NULL_REDUCTION(depth)
				&& basic_hash[hash_index].score < *beta)
				return_tag = NULL_FAILS;

			if (basic_hash[hash_index].depth < depth)
				break;

			switch (basic_hash[hash_index].tag)
			{
			case EQUAL_TRUE:
				t1 = basic_hash[hash_index].score;

				if (t1 > NEAR_MATE)
					t1 -= ply;

				else if (t1 < -900000)
					t1 += ply;
				*alpha = t1;
				return EQUAL_TRUE;

			case ABOVE_TRUE:

				if (basic_hash[hash_index].score <= *alpha)
				{
					*alpha = basic_hash[hash_index].score;
					return ABOVE_TRUE;
				}
				break;

			case BELOW_TRUE:
				if (basic_hash[hash_index].score >= *beta)
				{
					*beta = basic_hash[hash_index].score;
					return BELOW_TRUE;
				}
				break;
			default:;
			}
		}
		while (false);
	return return_tag;
}

unsigned int get_move(const int side)
{
	const bitboard key = BLACK_SIDE(side) ? board->key : ~board->key;
	const bitboard hash_index = key & hash_entries - 1;

	if (smart_hash[hash_index].key == key)
	{
		smart_hash[hash_index].old = 0;
		return smart_hash[hash_index].move_to_try;
	}

	if (basic_hash[hash_index].key == key)
	{
		return basic_hash[hash_index].move_to_try;
	}
	return 0;
}
