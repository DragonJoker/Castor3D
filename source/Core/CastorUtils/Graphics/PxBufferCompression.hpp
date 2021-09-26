/*
See LICENSE file in root folder
*/
#ifndef ___CU_PxBufferCompression___
#define ___CU_PxBufferCompression___

#include "CastorUtils/Graphics/PixelBufferBase.hpp"
#include "CastorUtils/Graphics/PixelComponents.hpp"

#if CU_UseCVTT
#	pragma warning( push )
#	pragma warning( disable: 4100 )
#	pragma warning( disable: 4242 )
#	pragma warning( disable: 4244 )
#	pragma warning( disable: 4365 )
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wconversion"
#	include <ConvectionKernels.h>
#	pragma GCC diagnostic pop
#	pragma warning( pop )
#endif

namespace castor
{
	using X8UGetter = uint8_t( * )( uint8_t const * );
	using X8SGetter = int8_t( * )( uint8_t const * );
	using X16FGetter = int16_t( * )( uint8_t const * );

#if CU_UseCVTT

	struct CVTTOptions
	{
		CU_API CVTTOptions();
		CU_API ~CVTTOptions();

		cvtt::Options options;
		cvtt::ETC2CompressionData * etc2CompressionData;
		cvtt::BC7EncodingPlan encodingPlan;
	};

	std::vector< cvtt::PixelBlockU8 > createBlocksU8( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X8UGetter getR
		, X8UGetter getG
		, X8UGetter getB
		, X8UGetter getA );
	std::vector< cvtt::PixelBlockS8 > createBlocksS8( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X8SGetter getR
		, X8SGetter getG
		, X8SGetter getB
		, X8SGetter getA );
	std::vector< cvtt::PixelBlockF16 > createBlocksF16( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X16FGetter getR
		, X16FGetter getG
		, X16FGetter getB
		, X16FGetter getA );

	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockU8 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize );
	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockS8 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize );
	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockF16 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize );

	template< PixelFormat PFSrc >
	struct CVTTCompressorU
	{
		CVTTCompressorU( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize )
			: options{ reinterpret_cast< CVTTOptions * >( optionsData->additionalOptions ) }
			, srcPixelSize{ srcPixelSize }
		{
		}

		void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & CU_UnusedParam( dstDimensions )
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			auto blocks = createBlocksU8( srcDimensions
				, srcPixelSize
				, srcBuffer
				, srcSize
				, getR8U< PFSrc >
				, getG8U< PFSrc >
				, getB8U< PFSrc >
				, getA8U< PFSrc > );
			compressBlocks( *options
				, blocks
				, dstFormat
				, dstBuffer
				, dstSize );
		}

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
	};

	template< PixelFormat PFSrc >
	struct CVTTCompressorS
	{
		CVTTCompressorS( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize )
			: options{ reinterpret_cast< CVTTOptions * >( optionsData->additionalOptions ) }
			, srcPixelSize{ srcPixelSize }
		{
		}

		void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & CU_UnusedParam( dstDimensions )
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			auto blocks = createBlocksS8( srcDimensions
				, srcPixelSize
				, srcBuffer
				, srcSize
				, getR8S< PFSrc >
				, getG8S< PFSrc >
				, getB8S< PFSrc >
				, getA8S< PFSrc > );
			compressBlocks( *options
				, blocks
				, dstFormat
				, dstBuffer
				, dstSize );
		}

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
	};

	template< PixelFormat PFSrc >
	struct CVTTCompressorF
	{
		CVTTCompressorF( PxBufferConvertOptions const * optionsData
			, uint32_t srcPixelSize )
			: options{ reinterpret_cast< CVTTOptions * >( optionsData->additionalOptions ) }
			, srcPixelSize{ srcPixelSize }
		{
		}

		void compress( PixelFormat dstFormat
			, Size const & srcDimensions
			, Size const & CU_UnusedParam( dstDimensions )
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			auto blocks = createBlocksF16( srcDimensions
				, srcPixelSize
				, srcBuffer
				, srcSize
				, getR16F< PFSrc >
				, getG16F< PFSrc >
				, getB16F< PFSrc >
				, getA16F< PFSrc > );
			compressBlocks( *options
				, blocks
				, dstFormat
				, dstBuffer
				, dstSize );
		}

	private:
		CVTTOptions const * options;
		uint32_t const srcPixelSize;
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
