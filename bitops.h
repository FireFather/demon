#include <nmmintrin.h>
#include <intrin.h> 

__forceinline static int pop_count(unsigned __int64 x) {
	return (int)_mm_popcnt_u64(x);
}

__forceinline int lsb(unsigned __int64 x)
{
	register unsigned long y;
	_BitScanForward64(&y, x);
	return y;
}

