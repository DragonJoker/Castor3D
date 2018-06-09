#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Graphics/Image.hpp"

extern "C"
{
#	include <FreeImage.h>
}

namespace castor
{
	//************************************************************************************************

	namespace
	{
		uint32_t next2Pow( uint32_t p_uiDim )
		{
			static uint32_t TwoPows[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };// should be enough for image dimensions ...
			static uint32_t Size = sizeof( TwoPows ) / sizeof( uint32_t );
			uint32_t uiReturn = 0;

			for ( uint32_t i = 0; i < Size && uiReturn < p_uiDim; i++ )
			{
				uiReturn = TwoPows[i];
			}

			return uiReturn;
		}

		uint32_t DLL_CALLCONV readProc( void * p_buffer, uint32_t p_uiSize, uint32_t p_count, fi_handle p_fiHandle )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
			return uint32_t( pFile->readArray( reinterpret_cast< uint8_t * >( p_buffer ), p_uiSize * p_count ) );
		}

		int DLL_CALLCONV seekProc( fi_handle p_fiHandle, long p_lOffset, int p_iOrigin )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
			return pFile->seek( p_lOffset, File::OffsetMode( p_iOrigin ) );
		}

		long DLL_CALLCONV tellProc( fi_handle p_fiHandle )
		{
			BinaryFile * pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
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

		void swapComponents( uint8_t * p_pixels, PixelFormat p_format, uint32_t p_width, uint32_t p_height )
		{
			uint32_t count{ p_width * p_height };
			uint32_t bpp{ PF::getBytesPerPixel( p_format ) };
			uint32_t bpc{ 0u };

			if ( PF::hasAlpha( p_format ) )
			{
				bpc = bpp / 4;
			}
			else
			{
				bpc = bpp / 3;
			}

			uint8_t * r{ p_pixels + 0 * bpc };
			uint8_t * b{ p_pixels + 2 * bpc };

			for ( uint32_t i = 0; i < count; i++ )
			{
				std::swap( *r, *b );
				r += bpp;
				b += bpp;
			}
		}
	}

	//************************************************************************************************

	Image::BinaryLoader::BinaryLoader()
	{
	}

	bool Image::BinaryLoader::operator()( Image & p_image, Path const & p_path )
	{
		if ( p_path.empty() )
		{
			LOADER_ERROR( "Can't load image : path is empty" );
		}

		p_image.m_buffer.reset();
		PixelFormat ePF = PixelFormat::eR8G8B8;
		int flags = BMP_DEFAULT;
		FREE_IMAGE_FORMAT fiFormat = FreeImage_GetFileType( string::stringCast< char >( p_path ).c_str(), 0 );

		if ( fiFormat == FIF_UNKNOWN )
		{
			fiFormat = FreeImage_GetFIFFromFilename( string::stringCast< char >( p_path ).c_str() );
		}
		else if ( fiFormat == FIF_TIFF )
		{
			flags = TIFF_DEFAULT;
		}

		if ( fiFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading( fiFormat ) )
		{
			LOADER_ERROR( "Can't load image : unsupported image format" );
		}

		auto fiImage = FreeImage_Load( fiFormat, string::stringCast< char >( p_path ).c_str() );

		if ( !fiImage )
		{
			BinaryFile file( p_path, uint32_t( File::OpenMode::eRead ) | uint32_t( File::OpenMode::eBinary ) );
			FreeImageIO fiIo;
			fiIo.read_proc = readProc;
			fiIo.write_proc = nullptr;
			fiIo.seek_proc = seekProc;
			fiIo.tell_proc = tellProc;
			fiImage = FreeImage_LoadFromHandle( fiFormat, & fiIo, fi_handle( & file ), flags );

			if ( !fiImage )
			{
				LOADER_ERROR( "Can't load image : " + string::stringCast< char >( p_path ) );
			}
		}

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType( fiImage );
		uint32_t width = FreeImage_GetWidth( fiImage );
		uint32_t height = FreeImage_GetHeight( fiImage );
		Size size( width, height );

		if ( type == FIC_PALETTE )
		{
			if ( FreeImage_IsTransparent( fiImage ) )
			{
				ePF = PixelFormat::eA8R8G8B8;
				FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
				FreeImage_PreMultiplyWithAlpha( dib );
				FreeImage_Unload( fiImage );
				fiImage = dib;

				if ( !fiImage )
				{
					LOADER_ERROR( "Can't convert image to 32 bits with alpha : " + string::stringCast< char >( p_path ) );
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
					LOADER_ERROR( "Can't convert image to 24 bits : " + string::stringCast< char >( p_path ) );
				}
			}
		}
		else if ( type == FIC_RGBALPHA )
		{
			ePF = PixelFormat::eA8R8G8B8;
			FIBITMAP * dib = FreeImage_ConvertTo32Bits( fiImage );
			FreeImage_PreMultiplyWithAlpha( dib );
			FreeImage_Unload( fiImage );
			fiImage = dib;

			if ( !fiImage )
			{
				LOADER_ERROR( "Can't convert image to 32 bits with alpha : " + string::stringCast< char >( p_path ) );
			}
		}
		else if ( fiFormat == FIF_HDR )
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
				LOADER_ERROR( "Unsupported HDR image format" );
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
				LOADER_ERROR( "Can't convert image to 24 bits : " + string::stringCast< char >( p_path ) );
			}
		}

		if ( !p_image.m_buffer )
		{
			uint8_t * pixels = FreeImage_GetBits( fiImage );
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			swapComponents( pixels, ePF, width, height );

#endif
			p_image.m_buffer = PxBufferBase::create( size, ePF, pixels, ePF );
			FreeImage_Unload( fiImage );
		}

		return p_image.m_buffer != nullptr;
	}

	//************************************************************************************************

	Image::BinaryWriter::BinaryWriter()
	{
	}

	bool Image::BinaryWriter::operator()( Image const & p_image, Path const & p_path )
	{
		bool result = false;
		FIBITMAP * fiImage = nullptr;
		Size const & size = p_image.getDimensions();
		int32_t w = int32_t( size.getWidth() );
		int32_t h = int32_t( size.getHeight() );

		if ( p_path.getExtension() == cuT( "png" ) )
		{
			fiImage = FreeImage_Allocate( w, h, 32 );
			PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eA8R8G8B8, p_image.getBuffer(), p_image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			swapComponents( pBufferRGB->ptr(), PixelFormat::eA8R8G8B8, w, h );

#endif

			if ( fiImage )
			{

				memcpy( FreeImage_GetBits( fiImage ), pBufferRGB->constPtr(), pBufferRGB->size() );
				FREE_IMAGE_FORMAT fif = FIF_PNG;
				result = FreeImage_Save( fif, fiImage, string::stringCast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}
		else if ( p_path.getExtension() == cuT( "hdr" ) )
		{
			if ( PF::hasAlpha( p_image.getPixelFormat() ) )
			{
				fiImage = FreeImage_AllocateT( FIT_RGBAF, w, h );
				PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eRGBA32F, p_image.getBuffer(), p_image.getPixelFormat() );

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
				PxBufferBaseSPtr pBufferRGB = PxBufferBase::create( size, PixelFormat::eRGB32F, p_image.getBuffer(), p_image.getPixelFormat() );

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
				result = FreeImage_Save( fif, fiImage, string::stringCast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}
		else
		{
			PxBufferBaseSPtr pBuffer;

			if ( p_image.getPixelFormat() != PixelFormat::eL8 )
			{
				fiImage = FreeImage_Allocate( w, h, 24 );
				pBuffer = PxBufferBase::create( size, PixelFormat::eR8G8B8, p_image.getBuffer(), p_image.getPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				swapComponents( pBuffer->ptr(), PixelFormat::eR8G8B8, w, h );

#endif
				w *= 3;
			}
			else
			{
				fiImage = FreeImage_Allocate( w, h, 8 );
				pBuffer = p_image.getPixels();
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

				result = FreeImage_Save( FIF_BMP, fiImage, string::stringCast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( fiImage );
			}
		}

		return result;
	}

	//************************************************************************************************

	Image & Image::resample( Size const & p_size )
	{
		Size const & size = getDimensions();

		if ( p_size != size )
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
				memcpy( FreeImage_GetBits( fiImage ), m_buffer->constPtr(), m_buffer->size() );
				uint32_t width = p_size.getWidth();
				uint32_t height = p_size.getHeight();
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
					m_buffer = PxBufferBase::create( p_size, ePF, pixels, ePF );
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
