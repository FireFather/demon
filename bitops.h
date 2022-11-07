//
//demon bitops.h
//
#pragma once

#if defined(_MSC_VER)
#include <bit>
#endif

inline int pop_count(const uint64_t b)
{
	return std::popcount(b);
}

inline int lsb(const uint64_t b)
{
	return std::countr_zero(b);
}
