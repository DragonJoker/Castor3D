/*
See LICENSE file in root folder
*/
#ifndef ___CU_BoxFilterKernel_H___
#define ___CU_BoxFilterKernel_H___

#include "CastorUtils/Graphics/PixelFormat.hpp"

namespace castor
{
	template< PixelFormat PFT >
	struct KernelBoxFilterT
	{
		static void compute( VkExtent2D const & fullExtent
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer
			, uint32_t level
			, uint32_t levelSize );
	};
}

#include "BoxFilterKernel.inl"

#endif
