/*
See LICENSE file in root folder
*/
#ifndef ___CU_LanczosFilterKernel_H___
#define ___CU_LanczosFilterKernel_H___

#include "CastorUtils/Graphics/Pixel.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Graphics/Position.hpp"

namespace castor
{
	template< PixelFormat PFT >
	struct KernelLanczosFilterT
	{
		static void compute( VkExtent2D const & fullExtent
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer
			, uint32_t level
			, uint32_t levelSize );
	};
}

#include "LanczosFilterKernel.inl"

#endif
