#include "CastorUtils/Graphics/PxBufferCompression.hpp"

#include "CastorUtils/Align/Aligned.hpp"
#include "CastorUtils/Graphics/PixelBufferBase.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
#if CU_UseCVTT

	//*****************************************************************************************

	namespace
	{
		template< typename TypeT >
		struct BlockTyperT;

		template<>
		struct BlockTyperT< uint8_t >
		{
			using Type = cvtt::PixelBlockU8;
		};

		template<>
		struct BlockTyperT< int8_t >
		{
			using Type = cvtt::PixelBlockS8;
		};

		template<>
		struct BlockTyperT< int16_t >
		{
			using Type = cvtt::PixelBlockF16;
		};

		template< typename TypeT >
		using BlockTypeT = typename BlockTyperT< TypeT >::Type;

		template< typename TypeT >
		void getLinePixels( BlockTypeT< TypeT > & block
			, uint32_t x
			, uint32_t w
			, uint32_t srcPixelSize
			, uint32_t lineIndex
			, uint8_t const *& linePtr
			, TypeT( *getR )( uint8_t const * )
			, TypeT( *getG )( uint8_t const * )
			, TypeT( *getB )( uint8_t const * )
			, TypeT( *getA )( uint8_t const * ) )
		{
			auto baseIndex = lineIndex * 4u;
			block.m_pixels[baseIndex + 0][0] = getR( linePtr );
			block.m_pixels[baseIndex + 0][1] = getG( linePtr );
			block.m_pixels[baseIndex + 0][2] = getB( linePtr );
			block.m_pixels[baseIndex + 0][3] = getA( linePtr );
			linePtr += ( x + 0 >= w ) ? 0 : srcPixelSize;
			block.m_pixels[baseIndex + 1][0] = getR( linePtr );
			block.m_pixels[baseIndex + 1][1] = getG( linePtr );
			block.m_pixels[baseIndex + 1][2] = getB( linePtr );
			block.m_pixels[baseIndex + 1][3] = getA( linePtr );
			linePtr += ( x + 1 >= w ) ? 0 : srcPixelSize;
			block.m_pixels[baseIndex + 2][0] = getR( linePtr );
			block.m_pixels[baseIndex + 2][1] = getG( linePtr );
			block.m_pixels[baseIndex + 2][2] = getB( linePtr );
			block.m_pixels[baseIndex + 2][3] = getA( linePtr );
			linePtr += ( x + 2 >= w ) ? 0 : srcPixelSize;
			block.m_pixels[baseIndex + 3][0] = getR( linePtr );
			block.m_pixels[baseIndex + 3][1] = getG( linePtr );
			block.m_pixels[baseIndex + 3][2] = getB( linePtr );
			block.m_pixels[baseIndex + 3][3] = getA( linePtr );
			linePtr += ( x + 3 >= w ) ? 0 : srcPixelSize;
		}

		template< typename TypeT >
		std::vector< BlockTypeT< TypeT > > extractBlocks( Size const & srcDimensions
			, uint32_t srcPixelSize
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, TypeT( *getR )( uint8_t const * )
			, TypeT( *getG )( uint8_t const * )
			, TypeT( *getB )( uint8_t const * )
			, TypeT( *getA )( uint8_t const * ) )
		{
			auto origin = srcBuffer;
			auto w = srcDimensions.getWidth();
			auto h = srcDimensions.getHeight();
			std::vector< BlockTypeT< TypeT > > result;
			auto bw = uint32_t( ashes::getAlignedSize( w, 4u ) );
			auto bh = uint32_t( ashes::getAlignedSize( h, 4u ) );
			result.reserve( ( bw * bh ) / 16u );
			auto srcLineSize = w * srcPixelSize;

			for ( uint32_t y = 0; y < bh; y += 4u )
			{
				auto line0Ptr = srcBuffer;
				srcBuffer += ( y + 0 >= h ) ? 0 : srcLineSize;
				assert( ptrdiff_t( srcBuffer - origin ) <= srcSize );
				auto line1Ptr = srcBuffer;
				srcBuffer += ( y + 1 >= h ) ? 0 : srcLineSize;
				assert( ptrdiff_t( srcBuffer - origin ) <= srcSize );
				auto line2Ptr = srcBuffer;
				srcBuffer += ( y + 2 >= h ) ? 0 : srcLineSize;
				assert( ptrdiff_t( srcBuffer - origin ) <= srcSize );
				auto line3Ptr = srcBuffer;
				srcBuffer += ( y + 3 >= h ) ? 0 : srcLineSize;
				assert( ptrdiff_t( srcBuffer - origin ) <= srcSize );

				for ( uint32_t x = 0; x < bw; x += 4u )
				{
					BlockTypeT< TypeT > block;
					getLinePixels( block, x, w, srcPixelSize, 0u, line0Ptr, getR, getG, getB, getA );
					getLinePixels( block, x, w, srcPixelSize, 1u, line1Ptr, getR, getG, getB, getA );
					getLinePixels( block, x, w, srcPixelSize, 2u, line2Ptr, getR, getG, getB, getA );
					getLinePixels( block, x, w, srcPixelSize, 3u, line3Ptr, getR, getG, getB, getA );
					result.push_back( block );
				}
			}

			auto aligned = ashes::getAlignedSize( result.size(), cvtt::NumParallelBlocks );

			while ( result.size() < aligned )
			{
				result.push_back( {} );
			}

			return result;
		}

		void * allocETC2( void * context, size_t size )
		{
			return alignedAlloc( 16u, size );
		}

		void freeETC2( void * context, void * ptr, size_t size )
		{
			alignedFree( ptr );
		}
	}

	//*****************************************************************************************

	cvtt::Options initialiseOptions()
	{
		cvtt::Options result;
		uint32_t flags = cvtt::Flags::Fastest;
		flags |= cvtt::Flags::BC7_RespectPunchThrough;
		result.flags = flags;
		return result;
	}

	CVTTOptions::CVTTOptions()
		: options{ initialiseOptions() }
		, etc2CompressionData{ cvtt::Kernels::AllocETC2Data( allocETC2, this, options ) }
	{
		cvtt::Kernels::ConfigureBC7EncodingPlanFromQuality( encodingPlan, 70 );
	}

	CVTTOptions::~CVTTOptions()
	{
		cvtt::Kernels::ReleaseETC2Data( etc2CompressionData, freeETC2 );
	}

	//*****************************************************************************************

	std::vector< cvtt::PixelBlockU8 > createBlocksU8( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X8UGetter getR
		, X8UGetter getG
		, X8UGetter getB
		, X8UGetter getA )
	{
		return extractBlocks( srcDimensions
			, srcPixelSize
			, srcBuffer
			, srcSize
			, getR
			, getG
			, getB
			, getA );
	}

	std::vector< cvtt::PixelBlockS8 > createBlocksS8( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X8SGetter getR
		, X8SGetter getG
		, X8SGetter getB
		, X8SGetter getA )
	{
		return extractBlocks( srcDimensions
			, srcPixelSize
			, srcBuffer
			, srcSize
			, getR
			, getG
			, getB
			, getA );
	}

	std::vector< cvtt::PixelBlockF16 > createBlocksF16( Size const & srcDimensions
		, uint32_t srcPixelSize
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, X16FGetter getR
		, X16FGetter getG
		, X16FGetter getB
		, X16FGetter getA )
	{
		return extractBlocks( srcDimensions
			, srcPixelSize
			, srcBuffer
			, srcSize
			, getR
			, getG
			, getB
			, getA );
	}

	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockU8 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		using namespace cvtt::Kernels;
		auto blocks = blocksCont.data();
		uint32_t written = 0u;
		auto pixelSize = PF::getBytesPerPixel( dstFormat );

		for ( auto it = blocksCont.begin(); it != blocksCont.end(); it += cvtt::NumParallelBlocks )
		{
				switch ( dstFormat )
				{
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
				case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
					EncodeBC1( dstBuffer, blocks, options.options );
					break;
				case PixelFormat::eBC2_UNORM_BLOCK:
				case PixelFormat::eBC2_SRGB_BLOCK:
					EncodeBC2( dstBuffer, blocks, options.options );
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
					EncodeBC3( dstBuffer, blocks, options.options );
					break;
				case PixelFormat::eBC4_UNORM_BLOCK:
					EncodeBC4U( dstBuffer, blocks, options.options );
					break;
				case PixelFormat::eBC5_UNORM_BLOCK:
					EncodeBC5U( dstBuffer, blocks, options.options );
					break;
				case PixelFormat::eBC7_UNORM_BLOCK:
				case PixelFormat::eBC7_SRGB_BLOCK:
					EncodeBC7( dstBuffer, blocks, options.options, options.encodingPlan );
					break;
				case PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
				case PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
					EncodeETC2( dstBuffer, blocks, options.options, options.etc2CompressionData );
					break;
				case PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
				case PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
					EncodeETC2PunchthroughAlpha( dstBuffer, blocks, options.options, options.etc2CompressionData );
					break;
				case PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
				case PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
					EncodeETC2RGBA( dstBuffer, blocks, options.options, options.etc2CompressionData );
					break;
				default:
					break;
				}

			written += cvtt::NumParallelBlocks * pixelSize;
			dstBuffer += cvtt::NumParallelBlocks * pixelSize;
			blocks += cvtt::NumParallelBlocks;
		}

		assert( written <= dstSize );
	}

	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockS8 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		using namespace cvtt::Kernels;
		auto blocks = blocksCont.data();
		uint32_t written = 0u;
		auto pixelSize = PF::getBytesPerPixel( dstFormat );

		for ( auto it = blocksCont.begin(); it != blocksCont.end(); it += cvtt::NumParallelBlocks )
		{
			switch ( dstFormat )
			{
			case castor::PixelFormat::eBC4_SNORM_BLOCK:
				EncodeBC4S( dstBuffer, blocks, options.options );
				break;
			case castor::PixelFormat::eBC5_SNORM_BLOCK:
				EncodeBC5S( dstBuffer, blocks, options.options );
				break;
			default:
				break;
			}

			written += cvtt::NumParallelBlocks * pixelSize;
			dstBuffer += cvtt::NumParallelBlocks * pixelSize;
			blocks += cvtt::NumParallelBlocks;
		}

		assert( written <= dstSize );
	}

	void compressBlocks( CVTTOptions  const & options
		, std::vector< cvtt::PixelBlockF16 > const & blocksCont
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		using namespace cvtt::Kernels;
		auto blocks = blocksCont.data();
		uint32_t written = 0u;
		auto pixelSize = PF::getBytesPerPixel( dstFormat );

		for ( auto it = blocksCont.begin(); it != blocksCont.end(); it += cvtt::NumParallelBlocks )
		{
			switch ( dstFormat )
			{
			case castor::PixelFormat::eBC6H_UFLOAT_BLOCK:
				EncodeBC6HU( dstBuffer, blocks, options.options );
				break;
			case castor::PixelFormat::eBC6H_SFLOAT_BLOCK:
				EncodeBC6HS( dstBuffer, blocks, options.options );
				break;
			default:
				break;
			}

			written += cvtt::NumParallelBlocks * pixelSize;
			dstBuffer += cvtt::NumParallelBlocks * pixelSize;
			blocks += cvtt::NumParallelBlocks;
		}

		assert( written <= dstSize );
	}

#else

	//*****************************************************************************************

	BC4x4Compressor::BC4x4Compressor( uint32_t srcPixelSize
		, X8UGetter getR
		, X8UGetter getG
		, X8UGetter getB
		, X8UGetter getA )
		: srcPixelSize{ srcPixelSize }
		, getR{ getR }
		, getG{ getG }
		, getB{ getB }
		, getA{ getA }
	{
	}

	uint32_t BC4x4Compressor::extractBlock( uint8_t const * inPtr
		, uint32_t width
		, Block & colorBlock )
	{
		auto rgba = colorBlock.data();
		uint32_t read = 0u;

		for ( int j = 0; j < 4; j++ )
		{
			auto linePtr = inPtr;

			for ( int i = 0; i < 4; i++ )
			{
				rgba[0] = getR( linePtr );
				rgba[1] = getG( linePtr );
				rgba[2] = getB( linePtr );
				rgba[3] = getA( linePtr );
				rgba += 4;
				linePtr += srcPixelSize;
				read += srcPixelSize;
			}

			inPtr += width * srcPixelSize;
		}

		return read;
	}

	uint16_t BC4x4Compressor::colorTo565( Color const & color )
	{
		return ( ( color[0] >> 3 ) << 11 )
			| ( ( color[1] >> 2 ) << 5 )
			| ( color[2] >> 3 );
	}

	void BC4x4Compressor::emitByte( uint8_t *& dstBuffer, uint8_t b )
	{
		dstBuffer[0] = b;
		dstBuffer += 1;
	}

	void BC4x4Compressor::emitWord( uint8_t *& dstBuffer, uint16_t s )
	{
		dstBuffer[0] = ( s >> 0 ) & 255;
		dstBuffer[1] = ( s >> 8 ) & 255;
		dstBuffer += 2;
	}

	void BC4x4Compressor::emitDoubleWord( uint8_t *& dstBuffer, uint32_t i )
	{
		dstBuffer[0] = ( i >> 0 ) & 255;
		dstBuffer[1] = ( i >> 8 ) & 255;
		dstBuffer[2] = ( i >> 16 ) & 255;
		dstBuffer[3] = ( i >> 24 ) & 255;
		dstBuffer += 4;
	}

	void BC4x4Compressor::emitColorIndices( uint8_t *& dstBuffer
		, Block const & colorBlock
		, Color const & minColor
		, Color const & maxColor )
	{
		static uint8_t constexpr maskC565_5 = 0xF8;    // 0xFF minus last three bits
		static uint8_t constexpr maskC565_6 = 0xFC;    // 0xFF minus last two bits

		uint8_t colors[4][4];
		uint32_t indices[16];
		uint32_t result = 0u;
		colors[0][0] = ( maxColor[0] & maskC565_5 ) | ( maxColor[0] >> 5 );
		colors[0][1] = ( maxColor[1] & maskC565_6 ) | ( maxColor[1] >> 6 );
		colors[0][2] = ( maxColor[2] & maskC565_5 ) | ( maxColor[2] >> 5 );
		colors[1][0] = ( minColor[0] & maskC565_5 ) | ( minColor[0] >> 5 );
		colors[1][1] = ( minColor[1] & maskC565_6 ) | ( minColor[1] >> 6 );
		colors[1][2] = ( minColor[2] & maskC565_5 ) | ( minColor[2] >> 5 );
		colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
		colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
		colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
		colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
		colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
		colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;

		for ( int i = 15; i >= 0; --i )
		{
			auto c0 = colorBlock[i * 4 + 0];
			auto c1 = colorBlock[i * 4 + 1];
			auto c2 = colorBlock[i * 4 + 2];

			auto d0 = abs( colors[0][0] - c0 ) + abs( colors[0][1] - c1 ) + abs( colors[0][2] - c2 );
			auto d1 = abs( colors[1][0] - c0 ) + abs( colors[1][1] - c1 ) + abs( colors[1][2] - c2 );
			auto d2 = abs( colors[2][0] - c0 ) + abs( colors[2][1] - c1 ) + abs( colors[2][2] - c2 );
			auto d3 = abs( colors[3][0] - c0 ) + abs( colors[3][1] - c1 ) + abs( colors[3][2] - c2 );

			auto b0 = uint32_t( d0 > d3 );
			auto b1 = uint32_t( d1 > d2 );
			auto b2 = uint32_t( d0 > d2 );
			auto b3 = uint32_t( d1 > d3 );
			auto b4 = uint32_t( d2 > d3 );

			auto x0 = b1 & b2;
			auto x1 = b0 & b3;
			auto x2 = b0 & b4;

			result |= ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
		}

		emitDoubleWord( dstBuffer, result );
	}

	//*****************************************************************************************

	namespace
	{
		void copyLineData( uint32_t pixelSize
			, uint32_t width
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer )
		{
			switch ( width )
			{
			case 1u:
				// Duplicate source pixel into the 4 pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			case 2u:
				// Duplicate first source pixel into the 2 first pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate second source pixel into the 2 last pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			case 3u:
				// Duplicate first source pixel into the first pixel of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate second source pixel into the second pixel of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				srcBuffer += pixelSize;
				// Duplicate third source pixel into the 2 last pixels of the block line
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				std::memcpy( dstBuffer, srcBuffer, pixelSize );
				dstBuffer += pixelSize;
				break;
			default:
				std::memcpy( dstBuffer, srcBuffer, pixelSize * width );
				break;
			}
		}

		void copyBlockData( uint32_t pixelSize
			, uint32_t srcLineSize
			, uint32_t dstLineSize
			, uint32_t width
			, uint32_t height
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer )
		{
			switch ( height )
			{
			case 1u:
				// Duplicate source line into the 4 lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				break;
			case 2u:
				// Duplicate first line line into the 2 first lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate second line line into the 2 last lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				break;
			case 3u:
				// Duplicate first line line into the first line of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate second line line into the second line of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				srcBuffer += srcLineSize;
				// Duplicate third line line into the 2 last lines of the block
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				dstBuffer += dstLineSize;
				copyLineData( pixelSize, width, srcBuffer, dstBuffer );
				break;
			default:
				for ( auto i = 0u; i < height; ++i )
				{
					copyLineData( pixelSize, width, srcBuffer, dstBuffer );
					dstBuffer += dstLineSize;
					srcBuffer += srcLineSize;
				}
				break;
			}
		}
	}

	//*****************************************************************************************

	BC1Compressor::BC1Compressor( uint32_t srcPixelSize
		, X8UGetter getR
		, X8UGetter getG
		, X8UGetter getB
		, X8UGetter getA )
		: BC4x4Compressor{ srcPixelSize, getR, getG, getB, getA }
	{
	}

	void BC1Compressor::compress( Size const & srcDimensions
		, Size const & dstDimensions
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		Block block;
		Color minColor;
		Color maxColor;
		auto srcHeight = srcDimensions.getHeight();
		auto srcWidth = srcDimensions.getWidth();

		if ( srcWidth < 4u || srcHeight < 4u )
		{
			return;
		}

		uint32_t written = 0u;
		uint32_t read = 0u;

		for ( uint32_t j = 0u; j < srcHeight; j += 4, srcBuffer += srcWidth * 4 * srcPixelSize )
		{
			for ( uint32_t i = 0; i < srcWidth; i += 4 )
			{
				read += extractBlock( srcBuffer + i * srcPixelSize, srcWidth, block );
				getMinMaxColors( block, minColor, maxColor );

				emitWord( dstBuffer, colorTo565( maxColor ) );
				emitWord( dstBuffer, colorTo565( minColor ) );
				emitColorIndices( dstBuffer, block, minColor, maxColor );
				written += 8u;
			}
		}

		CU_Assert( written == dstSize, "Written too many bytes" );
		CU_Assert( read == srcSize, "Read too many bytes" );
	}

	void BC1Compressor::getMinMaxColors( Block const & colorBlock
		, Color & minColor
		, Color & maxColor )
	{
		static int constexpr insetShift = 4;       // inset the bounding box with ( range >> shift )
		uint8_t inset[3];
		minColor[0] = minColor[1] = minColor[2] = 255;
		maxColor[0] = maxColor[1] = maxColor[2] = 0;

		for ( int i = 0; i < 16; i++ )
		{
			if ( colorBlock[i * 4 + 0] < minColor[0] )
			{
				minColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] < minColor[1] )
			{
				minColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] < minColor[2] )
			{
				minColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 0] > maxColor[0] )
			{
				maxColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] > maxColor[1] )
			{
				maxColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] > maxColor[2] )
			{
				maxColor[2] = colorBlock[i * 4 + 2];
			}
		}

		inset[0] = ( maxColor[0] - minColor[0] ) >> insetShift;
		inset[1] = ( maxColor[1] - minColor[1] ) >> insetShift;
		inset[2] = ( maxColor[2] - minColor[2] ) >> insetShift;
		minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
		minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
		minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
		maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
		maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
		maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
	}

	//*****************************************************************************************

	BC3Compressor::BC3Compressor( uint32_t srcPixelSize
		, X8UGetter getR
		, X8UGetter getG
		, X8UGetter getB
		, X8UGetter getA )
		: BC4x4Compressor{ srcPixelSize, getR, getG, getB, getA }
	{
	}

	void BC3Compressor::compress( Size const & srcDimensions
		, Size const & dstDimensions
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		Block block;
		Color minColor;
		Color maxColor;
		auto srcHeight = srcDimensions.getHeight();
		auto srcWidth = srcDimensions.getWidth();

		if ( srcWidth < 4u || srcHeight < 4u )
		{
			return;
		}

		uint32_t written = 0u;
		uint32_t read = 0u;

		for ( uint32_t j = 0u; j < srcHeight; j += 4, srcBuffer += srcWidth * 4 * srcPixelSize )
		{
			for ( uint32_t i = 0; i < srcWidth; i += 4 )
			{
				read += extractBlock( srcBuffer + i * srcPixelSize, srcWidth, block );
				getMinMaxColors( block, minColor, maxColor );

				emitByte( dstBuffer, maxColor[3] );
				emitByte( dstBuffer, minColor[3] );
				emitAlphaIndices( dstBuffer, block, minColor[3], maxColor[3] );

				emitWord( dstBuffer, colorTo565( maxColor ) );
				emitWord( dstBuffer, colorTo565( minColor ) );
				emitColorIndices( dstBuffer, block, minColor, maxColor );
				written += 8u;
			}
		}

		CU_Assert( written == dstSize, "Written too many bytes" );
		CU_Assert( read == srcSize, "Read too many bytes" );
	}

	void BC3Compressor::getMinMaxColors( Block const & colorBlock
		, Color & minColor
		, Color & maxColor )
	{
		static int constexpr insetShift = 4;       // inset the bounding box with ( range >> shift )
		Color inset;
		minColor[0] = minColor[1] = minColor[2] = 255;
		maxColor[0] = maxColor[1] = maxColor[2] = 0;

		for ( int i = 0; i < 16; i++ )
		{
			if ( colorBlock[i * 4 + 0] < minColor[0] )
			{
				minColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] < minColor[1] )
			{
				minColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] < minColor[2] )
			{
				minColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 3] < minColor[3] )
			{
				minColor[3] = colorBlock[i * 4 + 3];
			}
			if ( colorBlock[i * 4 + 0] > maxColor[0] )
			{
				maxColor[0] = colorBlock[i * 4 + 0];
			}
			if ( colorBlock[i * 4 + 1] > maxColor[1] )
			{
				maxColor[1] = colorBlock[i * 4 + 1];
			}
			if ( colorBlock[i * 4 + 2] > maxColor[2] )
			{
				maxColor[2] = colorBlock[i * 4 + 2];
			}
			if ( colorBlock[i * 4 + 3] > maxColor[3] )
			{
				maxColor[3] = colorBlock[i * 4 + 3];
			}
		}

		inset[0] = ( maxColor[0] - minColor[0] ) >> insetShift;
		inset[1] = ( maxColor[1] - minColor[1] ) >> insetShift;
		inset[2] = ( maxColor[2] - minColor[2] ) >> insetShift;
		inset[3] = ( maxColor[3] - minColor[3] ) >> insetShift;
		minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
		minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
		minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
		minColor[3] = ( minColor[3] + inset[3] <= 255 ) ? minColor[3] + inset[3] : 255;
		maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
		maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
		maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
		maxColor[3] = ( maxColor[3] >= inset[3] ) ? maxColor[3] - inset[3] : 0;
	}

	void BC3Compressor::emitAlphaIndices( uint8_t *& dstBuffer
		, Block const & colorBlock
		, uint8_t const minAlpha
		, uint8_t const maxAlpha )
	{
		assert( maxAlpha > minAlpha );
		uint8_t indices[16];
		uint8_t mid = ( maxAlpha - minAlpha ) / ( 2 * 7 );
		uint8_t ab1 = minAlpha + mid;
		uint8_t ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / 7 + mid;
		uint8_t ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / 7 + mid;
		uint8_t ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / 7 + mid;
		uint8_t ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / 7 + mid;
		uint8_t ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / 7 + mid;
		uint8_t ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / 7 + mid;
		auto colorBlockData = colorBlock.data();
		colorBlockData += 3;

		for ( int i = 0; i < 16; i++ )
		{
			uint8_t a = colorBlockData[i * 4];
			auto b1 = uint16_t( a <= ab1 );
			auto b2 = uint16_t( a <= ab2 );
			auto b3 = uint16_t( a <= ab3 );
			auto b4 = uint16_t( a <= ab4 );
			auto b5 = uint16_t( a <= ab5 );
			auto b6 = uint16_t( a <= ab6 );
			auto b7 = uint16_t( a <= ab7 );
			auto index = uint8_t( ( b1 + b2 + b3 + b4 + b5 + b6 + b7 + 1u ) & 0x07u );
			indices[i] = index ^ uint8_t( 2u > index );
		}

		emitByte( dstBuffer, ( indices[0] >> 0 ) | ( indices[1] << 3 ) | ( indices[2] << 6 ) );
		emitByte( dstBuffer, ( indices[2] >> 2 ) | ( indices[3] << 1 ) | ( indices[4] << 4 ) | ( indices[5] << 7 ) );
		emitByte( dstBuffer, ( indices[5] >> 1 ) | ( indices[6] << 2 ) | ( indices[7] << 5 ) );
		emitByte( dstBuffer, ( indices[8] >> 0 ) | ( indices[9] << 3 ) | ( indices[10] << 6 ) );
		emitByte( dstBuffer, ( indices[10] >> 2 ) | ( indices[11] << 1 ) | ( indices[12] << 4 ) | ( indices[13] << 7 ) );
		emitByte( dstBuffer, ( indices[13] >> 1 ) | ( indices[14] << 2 ) | ( indices[15] << 5 ) );
	}

	//*****************************************************************************************

#endif
}
