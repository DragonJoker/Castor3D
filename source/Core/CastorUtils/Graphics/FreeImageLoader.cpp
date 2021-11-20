#include "CastorUtils/Graphics/FreeImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

#if C3D_UseFreeImage
#	include <FreeImage.h>
#endif

namespace castor
{
	//************************************************************************************************

	namespace
	{
#if C3D_UseFreeImage

		void swapComponents( uint8_t * pixels, PixelFormat format, uint32_t width, uint32_t height )
		{
			uint32_t count{ width * height };
			auto bpp( uint32_t( getBytesPerPixel( format ) ) );
			uint32_t bpc{ 0u };

			if ( hasAlpha( format ) )
			{
				bpc = bpp / 4;
			}
			else
			{
				bpc = bpp / 3;
			}

			uint8_t * r{ pixels + 0 * bpc };
			uint8_t * b{ pixels + 2 * bpc };

			for ( uint32_t i = 0; i < count; i++ )
			{
				std::swap( *r, *b );
				r += bpp;
				b += bpp;
			}
		}

#endif

		StringArray const & listExtensions()
		{
			static StringArray const list
#if C3D_UseFreeImage
			{
				cuT( "png" ),
			}
#endif
			;
			return list;
		}
	}

	//************************************************************************************************

	void FreeImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< FreeImageLoader >() );
	}

	void FreeImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout FreeImageLoader::load( String const & CU_UnusedParam( imageFormat )
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
#if C3D_UseFreeImage

		PixelFormat sourceFmt = PixelFormat::eR8G8B8_UNORM;
		auto fiMemory = FreeImage_OpenMemory( const_cast< uint8_t * >( data ), size );
		FREE_IMAGE_FORMAT fiFormat = FreeImage_GetFileTypeFromMemory( fiMemory, 0 );

		if ( fiFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading( fiFormat ) )
		{
			CU_LoaderError( "Can't load image: unsupported image format" );
		}

		auto fiImage = FreeImage_LoadFromMemory( fiFormat, fiMemory, 0 );

		if ( !fiImage )
		{
			CU_LoaderError( "Can't load image" );
		}

		auto imageType = FreeImage_GetImageType( fiImage );

		if ( fiFormat == FIF_HDR
			|| fiFormat == FIF_EXR )
		{
			auto bpp = FreeImage_GetBPP( fiImage ) / 8;

			if ( imageType == FIT_RGBAF )
			{
				if ( bpp == getBytesPerPixel( PixelFormat::eR16G16B16A16_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR16G16B16A16_SFLOAT;
				}
				else if ( bpp == getBytesPerPixel( PixelFormat::eR32G32B32A32_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR32G32B32A32_SFLOAT;
				}
				else
				{
					CU_LoaderError( "Unsupported HDR RGBA image format" );
				}
			}
			else if ( imageType == FIT_RGBF )
			{
				if ( bpp == getBytesPerPixel( PixelFormat::eR16G16B16_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR16G16B16_SFLOAT;
				}
				else if ( bpp == getBytesPerPixel( PixelFormat::eR32G32B32_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR32G32B32_SFLOAT;
				}
				else
				{
					CU_LoaderError( "Unsupported HDR RGB image format" );
				}
			}
			else if ( imageType == FIT_FLOAT )
			{
				if ( bpp == getBytesPerPixel( PixelFormat::eR16_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR16_SFLOAT;
				}
				else if ( bpp == getBytesPerPixel( PixelFormat::eR32_SFLOAT ) )
				{
					sourceFmt = PixelFormat::eR32_SFLOAT;
				}
				else
				{
					CU_LoaderError( "Unsupported HDR R image format" );
				}
			}
		}
		else
		{
			sourceFmt = PixelFormat::eR8G8B8A8_UNORM;
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 32 bits" );
			}
		}

		uint32_t width = FreeImage_GetWidth( fiImage );
		uint32_t height = FreeImage_GetHeight( fiImage );
		Size dimensions( width, height );
		uint8_t * pixels = FreeImage_GetBits( fiImage );
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
		swapComponents( pixels, sourceFmt, width, height );
#endif
		buffer = PxBufferBase::create( dimensions
			, sourceFmt
			, pixels
			, sourceFmt );
		FreeImage_Unload( fiImage );
		FreeImage_CloseMemory( fiMemory );
		return ImageLayout{ ImageLayout::e2D, *buffer };

#else

		return ImageLayout{ ImageLayout::e2D };

#endif
	}
}
