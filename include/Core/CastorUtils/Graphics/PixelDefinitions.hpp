/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelDefinitions___
#define ___CU_PixelDefinitions___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Exception/Exception.hpp"

#include <vector>
#include <algorithm>
#include <numeric>

namespace castor
{
	bool decompressBC1Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
	bool decompressBC3Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
	bool decompressBC5Block( uint8_t const * bitstring
		, uint8_t * pixelBuffer );
}

#include "PixelDefinitions.inl"

#endif
