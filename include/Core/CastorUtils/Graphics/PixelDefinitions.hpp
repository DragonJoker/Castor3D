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

	struct BC4x4Compressor
	{
	protected:
		using Block = std::array< uint8_t, 64u >;
		using Color = std::array< uint8_t, 4u >;

		CU_API BC4x4Compressor( uint32_t srcPixelSize
			, uint8_t( *R8 )( uint8_t const * )
			, uint8_t( *G8 )( uint8_t const * )
			, uint8_t( *B8 )( uint8_t const * )
			, uint8_t( *A8 )( uint8_t const * ) );

		CU_API uint32_t extractBlock( uint8_t const * inPtr
			, uint32_t width
			, Block & colorBlock );
		CU_API uint16_t colorTo565( Color const & color );
		CU_API void emitByte( uint8_t *& dstBuffer, uint8_t b );
		CU_API void emitWord( uint8_t *& dstBuffer, uint16_t s );
		CU_API void emitDoubleWord( uint8_t *& dstBuffer, uint32_t i );
		CU_API void emitColorIndices( uint8_t *& dstBuffer
			, Block const & colorBlock
			, Color const & minColor
			, Color const & maxColor );

	protected:
		uint32_t const srcPixelSize;
		uint8_t( *getR8 )( uint8_t const * );
		uint8_t( *getG8 )( uint8_t const * );
		uint8_t( *getB8 )( uint8_t const * );
		uint8_t( *getA8 )( uint8_t const * );
	};

	struct BC1Compressor
		: private BC4x4Compressor
	{
		CU_API BC1Compressor( uint32_t srcPixelSize
			, uint8_t( *R8 )( uint8_t const * )
			, uint8_t( *G8 )( uint8_t const * )
			, uint8_t( *B8 )( uint8_t const * )
			, uint8_t( *A8 )( uint8_t const * ) );

		CU_API void compress( Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize );

	private:
		void getMinMaxColors( Block const & colorBlock
			, Color & minColor
			, Color & maxColor );
	};

	struct BC3Compressor
		: private BC4x4Compressor
	{
		CU_API BC3Compressor( uint32_t srcPixelSize
			, uint8_t( *R8 )( uint8_t const * )
			, uint8_t( *G8 )( uint8_t const * )
			, uint8_t( *B8 )( uint8_t const * )
			, uint8_t( *A8 )( uint8_t const * ) );

		CU_API void compress( Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize );

	private:
		void getMinMaxColors( Block const & colorBlock
			, Color & minColor
			, Color & maxColor );
		void emitAlphaIndices( uint8_t *& dstBuffer
			, Block const & colorBlock
			, uint8_t const minAlpha
			, uint8_t const maxAlpha );
	};
}

#include "PixelDefinitions.inl"

#endif
