#include "float_radix_sort.hpp"

#define PREFETCH 1

#if PREFETCH
#include <xmmintrin.h>	// for prefetch
#define pfval	64
#define pfval2	128
#define pf(x)	_mm_prefetch((const char*)(x + i + pfval), 0)
#define pf2(x)	_mm_prefetch((const char*)(x + i + pfval2), 0)
#else
#define pf(x)
#define pf2(x)
#endif

#if _WINDOWS
#define finline __forceinline
#else
#define finline inline
#endif

// ================================================================================================
// flip a float for sorting
//  finds SIGN of fp number.
//  if it's 1 (negative float), it flips all bits
//  if it's 0 (positive float), it flips the sign only
// ================================================================================================
finline unsigned FloatFlip(unsigned f)
{
	unsigned mask = -int(f >> 31) | 0x80000000;
	return f ^ mask;
}

finline void FloatFlipX(unsigned &f)
{
	unsigned mask = -int(f >> 31) | 0x80000000;
	f ^= mask;
}

// ================================================================================================
// flip a float back (invert FloatFlip)
//  signed was flipped from above, so:
//  if sign is 1 (negative), it flips the sign bit back
//  if sign is 0 (positive), it flips all bits back
// ================================================================================================
finline unsigned IFloatFlip(unsigned f)
{
	unsigned mask = ((f >> 31) - 1) | 0x80000000;
	return f ^ mask;
}

// ---- utils for accessing 11-bit quantities
#define _0(x)	(x & 0x7FF)
#define _1(x)	(x >> 11 & 0x7FF)
#define _2(x)	(x >> 22 )

void float_radix_sort(float *farray, float *sorted, unsigned elements)
{
	unsigned i;
	unsigned *sort = (unsigned*)sorted;
	unsigned *array = (unsigned*)farray;

	// 3 histograms on the stack:
	const unsigned kHist = 2048;
	unsigned b0[kHist * 3];

	unsigned *b1 = b0 + kHist;
	unsigned *b2 = b1 + kHist;

	for (i = 0; i < kHist * 3; i++)
		b0[i] = 0;
	//memset(b0, 0, kHist * 12);

	// 1.  parallel histogramming pass
	for (i = 0; i < elements; i++) {
		pf(array);
		unsigned fi = FloatFlip((unsigned&)array[i]);
		b0[_0(fi)] ++;
		b1[_1(fi)] ++;
		b2[_2(fi)] ++;
	}
	
	// 2.  Sum the histograms -- each histogram entry records the number of values preceding itself.
	{
		unsigned sum0 = 0, sum1 = 0, sum2 = 0;
		unsigned tsum;
		for (i = 0; i < kHist; i++) {

			tsum = b0[i] + sum0;
			b0[i] = sum0 - 1;
			sum0 = tsum;

			tsum = b1[i] + sum1;
			b1[i] = sum1 - 1;
			sum1 = tsum;

			tsum = b2[i] + sum2;
			b2[i] = sum2 - 1;
			sum2 = tsum;
		}
	}

	// byte 0: floatflip entire value, read/write histogram, write out flipped
	for (i = 0; i < elements; i++) {

		unsigned fi = array[i];
		FloatFlipX(fi);
		unsigned pos = _0(fi);
		
		pf2(array);
		sort[++b0[pos]] = fi;
	}

	// byte 1: read/write histogram, copy
	//   sorted -> array
	for (i = 0; i < elements; i++) {
		unsigned si = sort[i];
		unsigned pos = _1(si);
		pf2(sort);
		array[++b1[pos]] = si;
	}

	// byte 2: read/write histogram, copy & flip out
	//   array -> sorted
	for (i = 0; i < elements; i++) {
		unsigned ai = array[i];
		unsigned pos = _2(ai);

		pf2(array);
		sort[++b2[pos]] = IFloatFlip(ai);
	}

	// to write original:
	// memcpy(array, sorted, elements * 4);
}
