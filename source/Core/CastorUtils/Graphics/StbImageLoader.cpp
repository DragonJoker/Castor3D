#include "CastorUtils/Graphics/StbImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include "stb_image.h"
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	//************************************************************************************************

	namespace stbil
	{
		static PxBufferBaseUPtr load8BitsGreyAlpha( uint8_t const * data
			, int width
			, int height
			, bool isHdr )
		{
			auto result = PxBufferBase::create( { uint32_t( width ), uint32_t( height ) }
				, isHdr ? PixelFormat::eR8G8_UNORM : PixelFormat::eR8G8_SRGB
				, data
				, isHdr ? PixelFormat::eR8G8_UNORM : PixelFormat::eR8G8_SRGB );
			auto redChannel = castor::extractComponent( result.get()
				, PixelComponent::eRed );
			auto alphaChannel = castor::extractComponent( result.get()
				, PixelComponent::eGreen );
			result = PxBufferBase::create( { uint32_t( width ), uint32_t( height ) }
			, isHdr ? PixelFormat::eR8G8B8A8_UNORM : PixelFormat::eR8G8B8A8_SRGB );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eRed
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eGreen
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eBlue
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eAlpha
				, *alphaChannel
				, *result );
			return result;
		}

		static PxBufferBaseUPtr load8BitsOther( uint8_t const * data
			, int channels
			, int width
			, int height
			, bool isHdr )
		{
			PixelFormat format;

			switch ( channels )
			{
			case STBI_grey:
				format = isHdr ? PixelFormat::eR8_UNORM : PixelFormat::eR8_SRGB;
				break;
			case STBI_rgb:
				format = isHdr ? PixelFormat::eR8G8B8_UNORM : PixelFormat::eR8G8B8_SRGB;
				break;
			default:
				format = isHdr ? PixelFormat::eR8G8B8A8_UNORM : PixelFormat::eR8G8B8A8_SRGB;
				break;
			}

			return PxBufferBase::create( { uint32_t( width ), uint32_t( height ) }
				, format
				, data
				, format );
		}

		static PxBufferBaseUPtr load8BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			PxBufferBaseUPtr result;
			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_set_flip_vertically_on_load( 1u );
			stbi_convert_iphone_png_to_rgb( 1u );
			auto isHdr = stbi_is_hdr_from_memory( input, int( size ) ) != 0;

			if ( auto data = stbi_load_from_memory( input
				, int( size )
				, &width
				, &height
				, &channels
				, 0 ) )
			{
				if ( channels == STBI_grey_alpha )
				{
					result = load8BitsGreyAlpha( data, width, height, isHdr );
				}
				else
				{
					result = load8BitsOther( data, channels, width, height, isHdr );
				}

				stbi_image_free( data );
			}
			else
			{
				CU_LoaderError( MbString( "Can't load image:\n" ) + stbi_failure_reason() );
			}

			return result;
		}

		static PxBufferBaseUPtr load32BitsGreyAlpha( float const * data
			, int width
			, int height )
		{
			using BytePtr = uint8_t const *;
			auto result = PxBufferBase::create( { uint32_t( width ), uint32_t( height ) }
				, PixelFormat::eR32G32_SFLOAT
				, BytePtr( data )
				, PixelFormat::eR32G32_SFLOAT );
			auto redChannel = castor::extractComponent( result.get()
				, PixelComponent::eRed );
			auto alphaChannel = castor::extractComponent( result.get()
				, PixelComponent::eGreen );
			result = PxBufferBase::create( { uint32_t( width ), uint32_t( height ) }
			, PixelFormat::eR32G32B32A32_SFLOAT );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eRed
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eGreen
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eBlue
				, *redChannel
				, *result );
			castor::copyBufferComponents( PixelComponent::eRed
				, PixelComponent::eAlpha
				, *alphaChannel
				, *result );
			return result;
		}

		static PxBufferBaseUPtr load32BitsOther( float const * data
			, int channels
			, int width
			, int height )
		{
			PixelFormat format;

			switch ( channels )
			{
			case 1:
				format = PixelFormat::eR32_SFLOAT;
				break;
			case 2:
				format = PixelFormat::eR32G32_SFLOAT;
				break;
			case 3:
				format = PixelFormat::eR32G32B32_SFLOAT;
				break;
			default:
				format = PixelFormat::eR32G32B32A32_SFLOAT;
				break;
			}

			using BytePtr = uint8_t const *;
			return PxBufferBase::create( Size{ uint32_t( width ), uint32_t( height ) }
				, format
				, BytePtr( data )
				, format );
		}

		static PxBufferBaseUPtr load32BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			PxBufferBaseUPtr result;
			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_set_flip_vertically_on_load( 1u );

			if ( auto data = stbi_loadf_from_memory( input
				, int( size )
				, &width
				, &height
				, &channels
				, 0 ) )
			{
				if ( channels == STBI_grey_alpha )
				{
					result = load32BitsGreyAlpha( data, width, height );
				}
				else
				{
					result = load32BitsOther( data, channels, width, height );
				}

				stbi_image_free( data );
			}
			else
			{
				CU_LoaderError( MbString( "Can't load image:\n" ) + stbi_failure_reason() );
			}

			return result;
		}

		static StringArray const StbExtensions
		{
			cuT( "jpeg" ),
			cuT( "jpg" ),
			cuT( "tga" ),
			cuT( "bmp" ),
			cuT( "psd" ),
			cuT( "gif" ),
			cuT( "hdr" ),
			cuT( "pic" ),
			cuT( "pnm" ),
			cuT( "ppm" ),
			cuT( "pgm" ),
#if !C3D_UseFreeImage
			cuT( "png" ),
#endif
		};

		static StringArray const & listExtensions()
		{
			return StbExtensions;
		}
	}

	//************************************************************************************************

	void StbImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( stbil::listExtensions()
			, castor::make_unique< StbImageLoader >() );
	}

	void StbImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( stbil::listExtensions() );
	}

	ImageLayout StbImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		if ( string::upperCase( imageFormat ).find( cuT( "HDR" ) ) != String::npos )
		{
			buffer = stbil::load32BitsPerChannel( data, size );
		}
		else
		{
			buffer = stbil::load8BitsPerChannel( data, size );
		}

		return ImageLayout{ ImageLayout::e2D, *buffer };
	}
}
