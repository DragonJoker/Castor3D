#include "CastorUtils/Graphics/FreeImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

#if C3D_UseFreeImage
#	pragma warning( disable: 4828 )
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

		PixelFormat convertTo32Bits( FIBITMAP *& fiImage )
		{
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 32 bits" );
			}

			return PixelFormat::eR8G8B8A8_SRGB;
		}

		void outputMessageFunction( FREE_IMAGE_FORMAT fif
			, const char * msg )
		{
			std::string format;
			std::cerr << "FreeImage error: " << msg;

			if ( fif != FIF_UNKNOWN )
			{
				std::cerr << ", with format " << FreeImage_GetFormatFromFIF( fif );
			}

			std::cerr << std::endl;
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

		PixelFormat sourceFmt{ PixelFormat::eR8G8B8A8_UNORM };
		FreeImage_SetOutputMessage( outputMessageFunction );
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
		bool needsComponentSwap{ false };

		switch ( imageType )
		{
		case FIT_UINT16:
			sourceFmt = PixelFormat::eR16_UNORM;
			break;
		case FIT_INT16:
			sourceFmt = PixelFormat::eR16_SNORM;
			break;
		case FIT_UINT32:
			sourceFmt = PixelFormat::eR32_UINT;
			break;
		case FIT_INT32:
			sourceFmt = PixelFormat::eR32_SINT;
			break;
		case FIT_FLOAT:
			sourceFmt = PixelFormat::eR32_SFLOAT;
			break;
		case FIT_DOUBLE:
			sourceFmt = PixelFormat::eR64_SFLOAT;
			break;
		case FIT_COMPLEX:
			sourceFmt = PixelFormat::eR64G64_SFLOAT;
			break;
		case FIT_RGBF:
			sourceFmt = PixelFormat::eR32G32B32_SFLOAT;
			needsComponentSwap = true;
			break;
		case FIT_RGBAF:
			sourceFmt = PixelFormat::eR32G32B32A32_SFLOAT;
			needsComponentSwap = true;
		default:
			sourceFmt = convertTo32Bits( fiImage );
			needsComponentSwap = true;
			break;
		}

		uint32_t width = FreeImage_GetWidth( fiImage );
		uint32_t height = FreeImage_GetHeight( fiImage );
		Size dimensions( width, height );
		uint8_t * pixels = FreeImage_GetBits( fiImage );
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
		if ( needsComponentSwap )
		{
			swapComponents( pixels, sourceFmt, width, height );
		}
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
