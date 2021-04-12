/*
See LICENSE file in root folder
*/
#ifndef ___CU_PxBufferCompression___
#define ___CU_PxBufferCompression___


#include "CastorUtils/Graphics/GraphicsModule.hpp"

#if CU_UseCVTT
#	include <ConvectionKernels.h>
#endif

namespace castor
{
#if CU_UseCVTT

	struct CVTTOptions
	{
		CU_API CVTTOptions();
		CU_API ~CVTTOptions();

		cvtt::Options options;
		cvtt::ETC2CompressionData * etc2CompressionData;
		cvtt::BC7EncodingPlan encodingPlan;
	};

	struct CVTTCompressorU
	{
		CU_API CVTTCompressorU( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize
			, X8UGetter getR
			, X8UGetter getG
			, X8UGetter getB
			, X8UGetter getA );

		CU_API void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize );

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
		X8UGetter getR;
		X8UGetter getG;
		X8UGetter getB;
		X8UGetter getA;
	};

	struct CVTTCompressorS
	{
		CU_API CVTTCompressorS( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize
			, X8SGetter getR
			, X8SGetter getG
			, X8SGetter getB
			, X8SGetter getA );

		CU_API void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize );

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
		X8SGetter getR;
		X8SGetter getG;
		X8SGetter getB;
		X8SGetter getA;
	};

	struct CVTTCompressorF
	{
		CU_API CVTTCompressorF( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize
			, X32FGetter getR
			, X32FGetter getG
			, X32FGetter getB
			, X32FGetter getA );

		CU_API void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize );

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
		X32FGetter getR;
		X32FGetter getG;
		X32FGetter getB;
		X32FGetter getA;
	};

#else

	struct BC4x4Compressor
	{
	protected:
		using Block = std::array< uint8_t, 64u >;
		using Color = std::array< uint8_t, 4u >;

		CU_API BC4x4Compressor( uint32_t srcPixelSize
			, X8UGetter getR
			, X8UGetter getG
			, X8UGetter getB
			, X8UGetter getA );

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
		X8UGetter getR;
		X8UGetter getG;
		X8UGetter getB;
		X8UGetter getA;
	};

	struct BC1Compressor
		: private BC4x4Compressor
	{
		CU_API BC1Compressor( uint32_t srcPixelSize
			, X8UGetter getR
			, X8UGetter getG
			, X8UGetter getB
			, X8UGetter getA );

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
			, X8UGetter getR
			, X8UGetter getG
			, X8UGetter getB
			, X8UGetter getA );

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

#endif
}

#endif
