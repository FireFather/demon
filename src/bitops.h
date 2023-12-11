//
//demon bitops.h
//
#pragma once

#if defined(_MSC_VER)
#include <bit>
#endif

inline int pop_count(const bitboard b)
{
	return std::popcount(b);
}

inline int lsb(const bitboard b)
{
	return std::countr_zero(b);
}
