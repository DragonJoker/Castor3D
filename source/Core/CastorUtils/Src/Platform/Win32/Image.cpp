#include "Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "Graphics/Image.hpp"
#include "Data/LoaderException.hpp"
#include "Data/Path.hpp"
#include "Graphics/Rectangle.hpp"
#include "Log/Logger.hpp"

extern "C"
{
#	include <FreeImage.h>
}

namespace castor
{
	//************************************************************************************************

	namespace
	{
		uint32_t next2Pow( uint32_t dim )
		{
			static uint32_t TwoPows[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };// should be enough for image dimensions ...
			static uint32_t Size = sizeof( TwoPows ) / sizeof( uint32_t );
			uint32_t result = 0;

			for ( uint32_t i = 0; i < Size && result < dim; i++ )
			{
				result = TwoPows[i];
			}

			return result;
		}

		uint32_t DLL_CALLCONV readProc( void * buffer, uint32_t size, uint32_t count, fi_handle handle )
		{
			BinaryFile * file = reinterpret_cast< BinaryFile * >( handle );
			return uint32_t( file->readArray( reinterpret_cast< uint8_t * >( buffer ), size * count ) );
		}

		int DLL_CALLCONV seekProc( fi_handle handle, long offset, int origin )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( handle );
			return pFile->seek( offset, File::OffsetMode( origin ) );
		}

		long DLL_CALLCONV tellProc( fi_handle handle )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( handle );
			return long( pFile->tell() );
		}

		void freeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char * message )
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

		bool preMultiplyWithAlpha( FIBITMAP * dib )
		{
			if ( !FreeImage_HasPixels( dib ) )
			{
				return false;
			}

			if ( ( FreeImage_GetBPP( dib ) != 32 ) || ( FreeImage_GetImageType( dib ) != FIT_BITMAP ) )
			{
				return false;
			}

			int width = FreeImage_GetWidth( dib );
			int height = FreeImage_GetHeight( dib );

			for ( int y = 0; y < height; y++ )
			{
				auto * bits = FreeImage_GetScanLine( dib, y );

				for ( int x = 0; x < width; x++, bits += 4 )
				{
					const uint8_t alpha = bits[FI_RGBA_ALPHA];

					// slightly faster: care for two special cases
					if ( alpha == 0x00 )
					{
						// special case for alpha == 0x00
						// color * 0x00 / 0xFF = 0x00
						bits[FI_RGBA_BLUE] = 0x00;
						bits[FI_RGBA_GREEN] = 0x00;
						bits[FI_RGBA_RED] = 0x00;
					}
					else if ( alpha == 0xFF )
					{
						// nothing to do for alpha == 0xFF
						// color * 0xFF / 0xFF = color
						continue;
					}
					else
					{
						bits[FI_RGBA_BLUE] = uint8_t( ( alpha * uint16_t( bits[FI_RGBA_BLUE] ) + 127 ) / 255 );
						bits[FI_RGBA_GREEN] = uint8_t( ( alpha * uint16_t( bits[FI_RGBA_GREEN] ) + 127 ) / 255 );
						bits[FI_RGBA_RED] = uint8_t( ( alpha * uint16_t( bits[FI_RGBA_RED] ) + 127 ) / 255 );
					}
				}
			}

			return true;
		}
	}

	//************************************************************************************************

	Image::BinaryLoader::BinaryLoader()
	{
	}

	bool Image::BinaryLoader::operator()( Image & image
		, Path const & path
		, bool dropAlpha )
	{
		if ( path.empty() )
		{
			CU_LoaderError( "Can't load image : path is empty" );
		}

		image.m_buffer.reset();
		PixelFormat ePF = PixelFormat::eR8G8B8;
		int flags = BMP_DEFAULT;
		FREE_IMAGE_FORMAT fiFormat = FreeImage_GetFileType( string::stringCast< char >( path ).c_str(), 0 );

		if ( fiFormat == FIF_UNKNOWN )
		{
			fiFormat = FreeImage_GetFIFFromFilename( string::stringCast< char >( path ).c_str() );
		}
		else if ( fiFormat == FIF_TIFF )
		{
			flags = TIFF_DEFAULT;
		}

		if ( fiFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading( fiFormat ) )
		{
			CU_LoaderError( "Can't load image : unsupported image format" );
		}

		auto fiImage = FreeImage_Load( fiFormat, string::stringCast< char >( path ).c_str() );

		if ( !fiImage )
		{
			BinaryFile file( path, uint32_t( File::OpenMode::eRead ) | uint32_t( File::OpenMode::eBinary ) );
			FreeImageIO fiIo;
			fiIo.read_proc = readProc;
			fiIo.write_proc = nullptr;
			fiIo.seek_proc = seekProc;
			fiIo.tell_proc = tellProc;
			fiImage = FreeImage_LoadFromHandle( fiFormat, & fiIo, fi_handle( & file ), flags );

			if ( !fiImage )
			{
				CU_LoaderError( "Can't load image : " + string::stringCast< char >( path ) );
			}
		}

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType( fiImage );
		uint32_t width = FreeImage_GetWidth( fiImage );
		uint32_t height = FreeImage_GetHeight( fiImage );
		Size size{ width, height };

		if ( type == FIC_PALETTE )
		{
			if ( FreeImage_IsTransparent( fiImage ) )
			{
				ePF = PixelFormat::eA8R8G8B8;
				FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );

				if ( !dropAlpha )
				{
					preMultiplyWithAlpha( dib );
				}

				FreeImage_Unload( fiImage );
				fiImage = dib;

				if ( !fiImage )
				{
					CU_LoaderError( "Can't convert image to 32 bits with alpha : " + string::stringCast< char >( path ) );
				}
			}
			else
			{
				ePF = PixelFormat::eA8R8G8B8;
				FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
				FreeImage_Unload( fiImage );
				fiImage = dib;

				if ( !fiImage )
				{
					CU_LoaderError( "Can't convert image to 24 bits : " + string::stringCast< char >( path ) );
				}
			}
		}
		else if ( type == FIC_RGBALPHA )
		{
			ePF = PixelFormat::eA8R8G8B8;
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );

			if ( !dropAlpha )
			{
				preMultiplyWithAlpha( dib );
			}

			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 32 bits with alpha : " + string::stringCast< char >( path ) );
			}
		}
		else if ( fiFormat == FIF_HDR
			|| fiFormat == FIF_EXR )
		{
			auto bpp = FreeImage_GetBPP( fiImage ) / 8;

			if ( bpp == PixelDefinitions< PixelFormat::eRGB16F >::Size )
			{
				ePF = PixelFormat::eRGB16F;
			}
			else if ( bpp == PixelDefinitions< PixelFormat::eRGB32F >::Size )
			{
				ePF = PixelFormat::eRGB32F;
			}
			else
			{
				CU_LoaderError( "Unsupported HDR image format" );
			}
		}
		else
		{
			ePF = PixelFormat::eA8R8G8B8;
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				CU_LoaderError( "Can't convert image to 24 bits : " + string::stringCast< char >( path ) );
			}
		}

		if ( !image.m_buffer )
		{
			uint8_t * pixels = FreeImage_GetBits( fiImage );
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			swapComponents( pixels, ePF, width, height );

#endif
			image.m_buffer = PxBufferBase::create( size, ePF, pixels, ePF );
			FreeImage_Unload( fiImage );
		}

		return image.m_buffer != nullptr;
	}

	//************************************************************************************************

	Image::BinaryWriter::BinaryWriter()
	{
	}

	bool Image::BinaryWriter::operator()( Image const & image, Path const & path )
	{
		bool result = false;
		FIBITMAP * fiImage = nullptr;
		Size const & size = image.getDimensions();
		int32_t w = int32_t( size.getWidth() );
		int32_t h = int32_t( size.getHeight() );

		if ( path.getExtension() == cuT( "png" ) )
		{
			fiImage = FreeImage_Allocate( w, h, 32 );
			PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eA8R8G8B8, image.getBuffer(), image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			swapComponents( pBufferRGB->ptr(), PixelFormat::eA8R8G8B8, w, h );

#endif

			if ( fiImage )
			{

				memcpy( FreeImage_GetBits( fiImage ), pBufferRGB->constPtr(), pBufferRGB->size() );
				FREE_IMAGE_FORMAT fif = FIF_PNG;
				result = FreeImage_Save( fif, fiImage, string::stringCast< char >( path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}
		else if ( path.getExtension() == cuT( "hdr" ) )
		{
			if ( PF::hasAlpha( image.getPixelFormat() ) )
			{
				fiImage = FreeImage_AllocateT( FIT_RGBAF, w, h );
				PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eRGBA32F, image.getBuffer(), image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				swapComponents( pBufferRGB->ptr(), PixelFormat::eRGBA32F, w, h );

#endif

				if ( fiImage )
				{
					memcpy( FreeImage_GetBits( fiImage ), pBufferRGB->constPtr(), pBufferRGB->size() );
					FIBITMAP * dib = FreeImage_ConvertToRGBF( fiImage );
					FreeImage_Unload( fiImage );
					fiImage = dib;
				}
			}
			else
			{
				fiImage = FreeImage_AllocateT( FIT_RGBF, w, h );
				PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eRGB32F, image.getBuffer(), image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				swapComponents( pBufferRGB->ptr(), PixelFormat::eRGB32F, w, h );

#endif

				if ( fiImage )
				{
					memcpy( FreeImage_GetBits( fiImage ), pBufferRGB->constPtr(), pBufferRGB->size() );
				}
			}

			if ( fiImage )
			{
				FREE_IMAGE_FORMAT fif = FIF_HDR;
				result = FreeImage_Save( fif, fiImage, string::stringCast< char >( path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}
		else
		{
			PxBufferBaseSPtr pBuffer;

			if ( image.getPixelFormat() != PixelFormat::eL8 )
			{
				fiImage = FreeImage_Allocate( w, h, 24 );
				pBuffer = PxBufferBase::create( size, PixelFormat::eR8G8B8, image.getBuffer(), image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				swapComponents( pBuffer->ptr(), PixelFormat::eR8G8B8, w, h );

#endif
				w *= 3;
			}
			else
			{
				fiImage = FreeImage_Allocate( w, h, 8 );
				pBuffer = image.getPixels();
			}

			if ( fiImage && pBuffer )
			{
				uint32_t pitch = FreeImage_GetPitch( fiImage );
				uint8_t * dst = FreeImage_GetBits( fiImage );
				uint8_t const * src = pBuffer->constPtr();

				if ( !( pitch % w ) )
				{
					memcpy( dst, src, pBuffer->size() );
				}
				else
				{
					for ( int32_t i = 0; i < h; ++i )
					{
						memcpy( dst, src, w );
						dst += pitch;
						src += w;
					}
				}

				result = FreeImage_Save( FIF_BMP, fiImage, string::stringCast< char >( path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}

		return result;
	}

	//************************************************************************************************

	Image & Image::resample( Size const & wantedSize )
	{
		Size const & size = getDimensions();

		if ( wantedSize != size )
		{
			FIBITMAP * fiImage = nullptr;
			int32_t w = int32_t( size.getWidth() );
			int32_t h = int32_t( size.getHeight() );
			PixelFormat ePF = getPixelFormat();
			uint32_t uiBpp = PF::getBytesPerPixel( ePF );

			switch ( ePF )
			{
			case PixelFormat::eR8G8B8:
				fiImage = FreeImage_AllocateT( FIT_BITMAP, w, h, 24 );
				break;

			case PixelFormat::eA8R8G8B8:
				fiImage = FreeImage_AllocateT( FIT_BITMAP, w, h, 32 );
				break;
			}

			if ( fiImage )
			{
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				swapComponents( m_buffer->ptr(), m_buffer->format(), w, h );

#endif
				memcpy( FreeImage_GetBits( fiImage ), m_buffer->constPtr(), m_buffer->size() );
				uint32_t width = wantedSize.getWidth();
				uint32_t height = wantedSize.getHeight();
				FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType( fiImage );
				FIBITMAP * pRescaled = FreeImage_Rescale( fiImage, width, height, FILTER_BICUBIC );

				if ( pRescaled )
				{
					FreeImage_Unload( fiImage );
					fiImage = pRescaled;
					width = FreeImage_GetWidth( fiImage );
					height = FreeImage_GetHeight( fiImage );
					uint8_t * pixels = FreeImage_GetBits( fiImage );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

					swapComponents( pixels, m_buffer->format(), width, height );

#endif
					m_buffer = PxBufferBase::create( wantedSize, ePF, pixels, ePF );
				}

				FreeImage_Unload( fiImage );
			}
		}

		return *this;
	}

	void Image::initialiseImageLib()
	{
		FreeImage_Initialise();
		FreeImage_SetOutputMessage( freeImageErrorHandler );
	}

	void Image::cleanupImageLib()
	{
		FreeImage_DeInitialise();
	}

	//************************************************************************************************
}

#endif
