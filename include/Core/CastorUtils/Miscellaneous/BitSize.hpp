/*
See LICENSE file in root folder
*/
#ifndef ___CU_BitSize_HPP___
#define ___CU_BitSize_HPP___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

namespace castor
{
	inline int getBitSize( uint64_t value )
	{
		static constexpr int bitPatternToLog2[128]
		{
			0, // change to 0 if you want bitSize(0) = 0
			48, -1, -1, 31, -1, 15, 51, -1, 63, 5, -1, -1, -1, 19, -1,
			23, 28, -1, -1, -1, 40, 36, 46, -1, 13, -1, -1, -1, 34, -1, 58,
			-1, 60, 2, 43, 55, -1, -1, -1, 50, 62, 4, -1, 18, 27, -1, 39,
			45, -1, -1, 33, 57, -1, 1, 54, -1, 49, -1, 17, -1, -1, 32, -1,
			53, -1, 16, -1, -1, 52, -1, -1, -1, 64, 6, 7, 8, -1, 9, -1,
			-1, -1, 20, 10, -1, -1, 24, -1, 29, -1, -1, 21, -1, 11, -1, -1,
			41, -1, 25, 37, -1, 47, -1, 30, 14, -1, -1, -1, -1, 22, -1, -1,
			35, 12, -1, -1, -1, 59, 42, -1, -1, 61, 3, 26, 38, 44, -1, 56
		};
		static uint64_t constexpr multiplicator = 0x6c04f118e9966f6bull;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value |= value >> 32;
		return bitPatternToLog2[(value * multiplicator) >> 57];
	}

	inline int getBitSize( uint32_t value )
	{
		return getBitSize( uint64_t( value ) );
	}
}

#endif
