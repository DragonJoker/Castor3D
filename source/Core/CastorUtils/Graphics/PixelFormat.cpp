#include "CastorUtils/Graphics/PixelFormat.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/PxBufferCompression.hpp"

#include <ashes/common/Format.hpp>

#define BCDEC_IMPLEMENTATION
#define BCDEC_BC4BC5_PRECISE
#include <bcdec.h>

#pragma GCC diagnostic ignored "-Wrestrict"

namespace c3d
{
	//*****************************************************************************************

	namespace px
	{
		static void dec_bc4_unorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc4( compressed, decompressed, destinationPitch, 0 );
		}

		static void dec_bc4_snorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc4( compressed, decompressed, destinationPitch, 1 );
		}

		static void dec_bc5_unorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc5( compressed, decompressed, destinationPitch, 0 );
		}

		static void dec_bc5_snorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc5( compressed, decompressed, destinationPitch, 1 );
		}

		static void dec_bc6h_float_unorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc6h_float( compressed, decompressed, destinationPitch, 0 );
		}

		static void dec_bc6h_float_snorm( void const * compressed, void * decompressed, int destinationPitch )
		{
			bcdec_bc6h_float( compressed, decompressed, destinationPitch, 1 );
		}

		static bool hasAlphaPixels( PxBufferBase const & alphaChannel )
		{
			return !std::all_of( alphaChannel.begin(), alphaChannel.end()
					, []( uint8_t byte )
					{
							return byte == 0x00;
					} )
				&& !std::all_of( alphaChannel.begin(), alphaChannel.end()
					, []( uint8_t byte )
					{
							return byte == 0xFF;
					} );
		}
	}

	//*****************************************************************************************

	PixelFormat getFormatByName( StringView formatName )
	{
		PixelFormat result = PixelFormat::eCOUNT;
		int i = 0u;

		while ( i < int( result ) && result == PixelFormat::eCOUNT )
		{
			switch ( PixelFormat( i ) )
			{
#define RGPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
			case PixelFormat::e##name:\
				result = ( formatName == PixelDefinitionsT< PixelFormat::e##name >::toStr() ? PixelFormat( i ) : PixelFormat::eCOUNT );\
				break;
#include <RenderGraph/PixelFormat.inl>
			default:
				break;
			}

			++i;
		}

		if ( result == PixelFormat::eCOUNT
			&& formatName == cuT( "argb32" ) )
			result = PixelFormat::eR8G8B8A8_UNORM;

		if ( result == PixelFormat::eCOUNT )
			CU_Failure( "Unsupported pixel format" );

		return result;
	}

	PixelFormat getPixelFormat( PixelFormat format, PixelComponents components )
	{
		format = getSingleComponent( format );
		auto count = components.size();

		switch ( format )
		{
		case PixelFormat::eR8_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_UNORM;
			case 2:
				return PixelFormat::eR8G8_UNORM;
			case 3:
				return PixelFormat::eR8G8B8_UNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SNORM;
			case 2:
				return PixelFormat::eR8G8_SNORM;
			case 3:
				return PixelFormat::eR8G8B8_SNORM;
			case 4:
				return PixelFormat::eR8G8B8A8_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR8_SRGB:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR8_SRGB;
			case 2:
				return PixelFormat::eR8G8_SRGB;
			case 3:
				return PixelFormat::eR8G8B8_SRGB;
			case 4:
				return PixelFormat::eR8G8B8A8_SRGB;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_UNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_UNORM;
			case 2:
				return PixelFormat::eR16G16_UNORM;
			case 3:
				return PixelFormat::eR16G16B16_UNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_UNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SNORM:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SNORM;
			case 2:
				return PixelFormat::eR16G16_SNORM;
			case 3:
				return PixelFormat::eR16G16B16_SNORM;
			case 4:
				return PixelFormat::eR16G16B16A16_SNORM;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR16_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR16_SFLOAT;
			case 2:
				return PixelFormat::eR16G16_SFLOAT;
			case 3:
				return PixelFormat::eR16G16B16_SFLOAT;
			case 4:
				return PixelFormat::eR16G16B16A16_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_UINT;
			case 2:
				return PixelFormat::eR32G32_UINT;
			case 3:
				return PixelFormat::eR32G32B32_UINT;
			case 4:
				return PixelFormat::eR32G32B32A32_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SINT;
			case 2:
				return PixelFormat::eR32G32_SINT;
			case 3:
				return PixelFormat::eR32G32B32_SINT;
			case 4:
				return PixelFormat::eR32G32B32A32_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR32_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR32_SFLOAT;
			case 2:
				return PixelFormat::eR32G32_SFLOAT;
			case 3:
				return PixelFormat::eR32G32B32_SFLOAT;
			case 4:
				return PixelFormat::eR32G32B32A32_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_UINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_UINT;
			case 2:
				return PixelFormat::eR64G64_UINT;
			case 3:
				return PixelFormat::eR64G64B64_UINT;
			case 4:
				return PixelFormat::eR64G64B64A64_UINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SINT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SINT;
			case 2:
				return PixelFormat::eR64G64_SINT;
			case 3:
				return PixelFormat::eR64G64B64_SINT;
			case 4:
				return PixelFormat::eR64G64B64A64_SINT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		case PixelFormat::eR64_SFLOAT:
			switch ( count )
			{
			case 1:
				return PixelFormat::eR64_SFLOAT;
			case 2:
				return PixelFormat::eR64G64_SFLOAT;
			case 3:
				return PixelFormat::eR64G64B64_SFLOAT;
			case 4:
				return PixelFormat::eR64G64B64A64_SFLOAT;
			default:
				return PixelFormat::eUNDEFINED;
			}
		default:
			return PixelFormat::eUNDEFINED;
		}
	}

	void convertPixel( PixelFormat srcFormat
		, uint8_t const *& srcBuffer
		, PixelFormat dstFormat
		, uint8_t *& dstBuffer )
	{
		switch ( srcFormat )
		{
#define RGPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
		case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( srcBuffer, dstBuffer, dstFormat );\
			break;
#define RGPF_ENUM_VALUE_COMPRESSED( name, value, components, alpha )
#include <RenderGraph/PixelFormat.inl>
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	void convertBuffer( Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define RGPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed ) case PixelFormat::e##name:\
			PixelDefinitionsT< PixelFormat::e##name >::convert( nullptr, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );\
			break;
#include <RenderGraph/PixelFormat.inl>
		default:
			CU_Failure( "Unsupported pixel format" );
			break;
		}
	}

	namespace pxf
	{
		using PFNDecompressBuffer = void( * )( void const * compressed, void * decompressed, int detinationPitch );

		template< PixelFormat PFSrc >
		void compressBufferT( [[maybe_unused]] PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			if constexpr ( isColourFormatV < PFSrc >
				&& !isCompressedV< PFSrc > )
			{
				switch ( dstFormat )
				{
#if CU_UseCVTT
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
				case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
				case PixelFormat::eBC2_UNORM_BLOCK:
				case PixelFormat::eBC2_SRGB_BLOCK:
				case PixelFormat::eBC4_UNORM_BLOCK:
				case PixelFormat::eBC5_UNORM_BLOCK:
				case PixelFormat::eBC7_UNORM_BLOCK:
				case PixelFormat::eBC7_SRGB_BLOCK:
					{
						CVTTCompressorU< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC4_SNORM_BLOCK:
				case PixelFormat::eBC5_SNORM_BLOCK:
					{
						CVTTCompressorS< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC6H_UFLOAT_BLOCK:
				case PixelFormat::eBC6H_SFLOAT_BLOCK:
					{
						CVTTCompressorF< PFSrc > compressor{ options
							, interrupt
							, uint32_t( getBytesPerPixel( PFSrc ) ) };
						compressor.compress( dstFormat
							, srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#else
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
					{
						BC1Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
					{
						BC3Compressor compressor{ uint32_t( getBytesPerPixel( PFSrc ) )
							, getR8U< PFSrc >
							, getG8U< PFSrc >
							, getB8U< PFSrc >
							, getA8U< PFSrc >
							, interrupt };
						compressor.compress( srcDimensions
							, dstDimensions
							, srcBuffer
							, srcSize
							, dstBuffer
							, dstSize );
					}
					break;
#endif
				default:
					break;
				}
			}
		}

		template< typename DecompressBlockT >
		bool decompressRowBlock( uint8_t const * data
			, uint32_t width
			, uint32_t height
			, uint32_t pixelSize
			, uint32_t y
			, uint32_t widthInBlocks
			, Array< uint8_t, 16 * 4u > & blockBuffer
			, uint8_t * pixelBuffer
			, DecompressBlockT decompressBlock )
		{
			uint32_t newRows = ( y * 4 + 3 >= height )
				? height - y * 4u
				: 4u;

			for ( uint32_t x = 0u; x < widthInBlocks; ++x )
			{
				if ( !decompressBlock( data, blockBuffer.data() ) )
				{
					return false;
				}

				uint32_t blockSize = 8u;
				uint8_t * pixelp = pixelBuffer
					+ y * 4u * width * pixelSize
					+ x * 4u * pixelSize;
				uint32_t newColumns;

				if ( x * 4 + 3 >= width )
				{
					newColumns = width - x * 4;
				}
				else
				{
					newColumns = 4u;
				}

				for ( uint32_t row = 0u; row < newRows; ++row )
				{
					memcpy( pixelp + row * width * pixelSize
						, blockBuffer.data() + row * 4u * pixelSize
						, newColumns * pixelSize );
				}

				data += blockSize;
			}

			return true;
		}
	}

	void compressBuffer( PxBufferConvertOptions const * options
		, std::atomic_bool const * interrupt
		, Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		switch ( srcFormat )
		{
#define RGPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
		case PixelFormat::e##name:\
			pxf::compressBufferT< PixelFormat::e##name >( options\
				, interrupt\
				, srcDimensions\
				, dstDimensions\
				, srcBuffer\
				, srcSize\
				, dstFormat\
				, dstBuffer, dstSize );\
			break;
#include <RenderGraph/PixelFormat.inl>
		default:
			break;
		}
	}

	PxBufferBaseUPtr decompressBuffer( PxBufferBase const & src )
	{
		auto result = src.clone();

		if ( isCompressed( src.getFormat() ) )
		{
			pxf::PFNDecompressBuffer decompressBuffer = nullptr;
			bool isSRGB = false;
			bool isFloat = false;

			switch ( src.getFormat() )
			{
			case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				isSRGB = true;
				[[fallthrough]];
			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				decompressBuffer = bcdec_bc1;
				break;
			case PixelFormat::eBC2_SRGB_BLOCK:
				isSRGB = true;
				[[fallthrough]];
			case PixelFormat::eBC2_UNORM_BLOCK:
				decompressBuffer = bcdec_bc2;
				break;
			case PixelFormat::eBC3_SRGB_BLOCK:
				isSRGB = true;
				[[fallthrough]];
			case PixelFormat::eBC3_UNORM_BLOCK:
				decompressBuffer = bcdec_bc3;
				break;
			case PixelFormat::eBC4_UNORM_BLOCK:
				decompressBuffer = px::dec_bc4_unorm;
				break;
			case PixelFormat::eBC4_SNORM_BLOCK:
				decompressBuffer = px::dec_bc4_snorm;
				break;
			case PixelFormat::eBC5_UNORM_BLOCK:
				decompressBuffer = px::dec_bc5_unorm;
				break;
			case PixelFormat::eBC5_SNORM_BLOCK:
				decompressBuffer = px::dec_bc5_snorm;
				break;
			case PixelFormat::eBC6H_UFLOAT_BLOCK:
				decompressBuffer = px::dec_bc6h_float_unorm;
				isFloat = true;
				break;
			case PixelFormat::eBC6H_SFLOAT_BLOCK:
				decompressBuffer = px::dec_bc6h_float_snorm;
				isFloat = true;
				break;
			case PixelFormat::eBC7_SRGB_BLOCK:
				isSRGB = true;
				[[fallthrough]];
			case PixelFormat::eBC7_UNORM_BLOCK:
				decompressBuffer = bcdec_bc7;
				isFloat = true;
				break;
			default:
				CU_Failure( "Unsupported compression format" );
				return result;
			}

			result = PxBufferBase::create( src.getDimensions()
				, ( isFloat
					? PixelFormat::eR32G32B32A32_SFLOAT
					: ( isSRGB ? PixelFormat::eR8G8B8A8_SRGB : PixelFormat::eR8G8B8A8_UNORM ) ) );
			uint8_t * decompressed = result->getPtr();
			uint8_t const * compressed = src.getConstPtr();
			decompressBuffer( compressed, decompressed, 0 );
		}

		return result;
	}

	String getName( PixelComponent const & component )
	{
		String result;
		switch ( component )
		{
		case PixelComponent::eRed:
			result = cuT( "red" );
			break;
		case PixelComponent::eGreen:
			result = cuT( "green" );
			break;
		case PixelComponent::eBlue:
			result = cuT( "blue" );
			break;
		case PixelComponent::eAlpha:
			result = cuT( "alpha" );
			break;
		case PixelComponent::eDepth:
			result = cuT( "depth" );
			break;
		case PixelComponent::eStencil:
			result = cuT( "stencil" );
			break;
		default:
			result = cuT( "unknown" );
			break;
		}

		return result;
	}

	String getName( PixelComponents const & components )
	{
		String result;
		String sep;

		for ( auto component : components )
		{
			result += sep + getName( component );
			sep = cuT( "|" );
		}

		return result;
	}

	bool hasAlphaChannel( Image const & image )
	{
		if ( auto alphaChannel = image.getAlphaChannel() )
			return px::hasAlphaPixels( *alphaChannel );
		if ( auto alphaChannel = extractComponent( *image.getPixels(), PixelComponent::eAlpha ) )
			return px::hasAlphaPixels( *alphaChannel );
		return false;
	}
}
