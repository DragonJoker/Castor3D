#include "CastorUtils/Graphics/FreeImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

#if C3D_UseFreeImage
#include <FreeImage.h>
#endif

namespace castor
{
	//************************************************************************************************

	namespace
	{
		bool preMultiplyWithAlpha( PxBufferBaseSPtr pixelBuffer )
		{
			struct Pixel
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			int width = pixelBuffer->getWidth();
			int height = pixelBuffer->getHeight();
			auto buffer = reinterpret_cast< Pixel * >( pixelBuffer->getPtr() );

			for ( int y = 0; y < height; ++y )
			{
				for ( int x = 0; x < width; ++x )
				{
					const uint8_t alpha = buffer->a;

					// slightly faster: care for two special cases
					if ( alpha == 0x00 )
					{
						// special case for alpha == 0x00
						// color * 0x00 / 0xFF = 0x00
						buffer->r = 0x00;
						buffer->g = 0x00;
						buffer->b = 0x00;
					}
					else if ( alpha == 0xFF )
					{
						// nothing to do for alpha == 0xFF
						// color * 0xFF / 0xFF = color
						continue;
					}
					else
					{
						buffer->r = uint8_t( ( alpha * uint16_t( buffer->r ) + 127 ) / 255 );
						buffer->g = uint8_t( ( alpha * uint16_t( buffer->g ) + 127 ) / 255 );
						buffer->b = uint8_t( ( alpha * uint16_t( buffer->b ) + 127 ) / 255 );
					}

					++buffer;
				}
			}

			return true;
		}

#if C3D_UseFreeImage

		uint32_t DLL_CALLCONV readProc( void * buffer, uint32_t size, uint32_t count, fi_handle fiHandle )
		{
			BinaryFile * file = reinterpret_cast< BinaryFile * >( fiHandle );
			return uint32_t( file->readArray( reinterpret_cast< uint8_t * >( buffer ), size * count ) );
		}

		int DLL_CALLCONV seekProc( fi_handle fiHandle, long offset, int origin )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( fiHandle );
			return pFile->seek( offset, File::OffsetMode( origin ) );
		}

		long DLL_CALLCONV tellProc( fi_handle fiHandle )
		{
			BinaryFile * file = reinterpret_cast< BinaryFile * >( fiHandle );
			return long( file->tell() );
		}

		void errorHandler( FREE_IMAGE_FORMAT fif, const char * message )
		{
			if ( fif != FIF_UNKNOWN )
			{
				Logger::logWarning( std::stringstream() << "FreeImage - " << FreeImage_GetFormatFromFIF( fif ) << " Format - " << message );
			}
			else
			{
				Logger::logWarning( std::stringstream() << "FreeImage - Unknown Format - " << message );
			}
		}

		void swapComponents( uint8_t * pixels, PixelFormat format, uint32_t width, uint32_t height )
		{
			uint32_t count{ width * height };
			uint32_t bpp{ PF::getBytesPerPixel( format ) };
			uint32_t bpc{ 0u };

			if ( PF::hasAlpha( format ) )
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
				cuT( "jpeg" ),
				cuT( "jpg" ),
				cuT( "png" ),
				cuT( "tga" ),
				cuT( "bmp" ),
				cuT( "psd" ),
				cuT( "gif" ),
				cuT( "hdr" ),
				cuT( "pic" ),
				cuT( "pnm" ),
				cuT( "ppm" ),
				cuT( "pgm" ),
				cuT( "tif" ),
				cuT( "exr" ),
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

	ImageLayout FreeImageLoader::load( String const & imageFormat
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

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType( fiImage );

		if ( type == FIC_PALETTE )
		{
			if ( FreeImage_IsTransparent( fiImage ) )
			{
				sourceFmt = PixelFormat::eR8G8B8A8_UNORM;
				FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
				FreeImage_Unload( fiImage );
				fiImage = dib;

				if ( !fiImage )
				{
					CU_LoaderError( "Can't convert image to 32 bits with alpha" );
				}
			}
			else
			{
				sourceFmt = PixelFormat::eR8G8B8_UNORM;
				FIBITMAP * dib = FreeImage_ConvertTo24Bits( fiImage );
				FreeImage_Unload( fiImage );
				fiImage = dib;

				if ( !fiImage )
				{
					CU_LoaderError( "Can't convert image to 24 bits" );
				}
			}
		}
		else if ( type == FIC_RGBALPHA )
		{
			sourceFmt = PixelFormat::eR8G8B8A8_UNORM;
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 32 bits with alpha" );
			}
		}
		else if ( fiFormat == FIF_HDR
			|| fiFormat == FIF_EXR )
		{
			auto bpp = FreeImage_GetBPP( fiImage ) / 8;
			auto type = FreeImage_GetImageType( fiImage );

			if ( type == FIT_RGBAF )
			{
				if ( bpp == PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Size )
				{
					sourceFmt = PixelFormat::eR16G16B16A16_SFLOAT;
				}
				else if ( bpp == PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Size )
				{
					sourceFmt = PixelFormat::eR32G32B32A32_SFLOAT;
				}
				else
				{
					CU_LoaderError( "Unsupported HDR RGBA image format" );
				}
			}
			else if ( type == FIT_RGBF )
			{
				if ( bpp == PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Size )
				{
					sourceFmt = PixelFormat::eR16G16B16_SFLOAT;
				}
				else if ( bpp == PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Size )
				{
					sourceFmt = PixelFormat::eR32G32B32_SFLOAT;
				}
				else
				{
					CU_LoaderError( "Unsupported HDR RGB image format" );
				}
			}
			else if ( type == FIT_FLOAT )
			{
				if ( bpp == PixelDefinitions< PixelFormat::eR16_SFLOAT >::Size )
				{
					sourceFmt = PixelFormat::eR16_SFLOAT;
				}
				else if ( bpp == PixelDefinitions< PixelFormat::eR32_SFLOAT >::Size )
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
			sourceFmt = PixelFormat::eR8G8B8_UNORM;
			FIBITMAP * dib = FreeImage_ConvertTo24Bits( fiImage );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 24 bits" );
			}
		}

		uint32_t width = FreeImage_GetWidth( fiImage );
		uint32_t height = FreeImage_GetHeight( fiImage );
		Size dimensions( width, height );
		auto memorySize = FreeImage_GetMemorySize( fiImage );
		auto levelSize = ashes::getSize( VkExtent2D{ dimensions.getWidth(), dimensions.getHeight() }
			, VkFormat( sourceFmt ) );
		CU_Require( memorySize >= levelSize );
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
