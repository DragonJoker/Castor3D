#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Graphics/Image.hpp"
#include "Data/Path.hpp"
#include "Graphics/Rectangle.hpp"
#include "Log/Logger.hpp"

extern "C"
{
#	include <FreeImage.h>
}

namespace Castor
{
	//************************************************************************************************

	namespace
	{
		uint32_t Next2Pow( uint32_t p_uiDim )
		{
			static uint32_t TwoPows[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576 };// should be enough for image dimensions ...
			static uint32_t Size = sizeof( TwoPows ) / sizeof( uint32_t );
			uint32_t l_uiReturn = 0;

			for ( uint32_t i = 0; i < Size && l_uiReturn < p_uiDim; i++ )
			{
				l_uiReturn = TwoPows[i];
			}

			return l_uiReturn;
		}

		uint32_t DLL_CALLCONV ReadProc( void * p_buffer, uint32_t p_uiSize, uint32_t p_count, fi_handle p_fiHandle )
		{
			BinaryFile * l_pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
			return uint32_t( l_pFile->ReadArray( reinterpret_cast< uint8_t * >( p_buffer ), p_uiSize * p_count ) );
		}

		int DLL_CALLCONV SeekProc( fi_handle p_fiHandle, long p_lOffset, int p_iOrigin )
		{
			BinaryFile * l_pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
			return l_pFile->Seek( p_lOffset, File::OffsetMode( p_iOrigin ) );
		}

		long DLL_CALLCONV TellProc( fi_handle p_fiHandle )
		{
			BinaryFile * l_pFile = reinterpret_cast< BinaryFile * >( p_fiHandle );
			return long( l_pFile->Tell() );
		}

		void FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char * message )
		{
			if ( fif != FIF_UNKNOWN )
			{
				Logger::LogWarning( std::stringstream() << "FreeImage - " << FreeImage_GetFormatFromFIF( fif ) << " Format - " << message );
			}
			else
			{
				Logger::LogWarning( std::stringstream() << "FreeImage - Unknown Format - " << message );
			}
		}

		void SwapComponents( uint8_t * p_pixels, PixelFormat p_format, uint32_t p_width, uint32_t p_height )
		{
			uint32_t l_count{ p_width * p_height };
			uint32_t l_bpp{ PF::GetBytesPerPixel( p_format ) };
			uint32_t l_bpc{ 0u };

			if ( PF::HasAlpha( p_format ) )
			{
				l_bpc = l_bpp / 4;
			}
			else
			{
				l_bpc = l_bpp / 3;
			}

			uint8_t * l_r{ p_pixels + 0 * l_bpc };
			uint8_t * l_b{ p_pixels + 2 * l_bpc };

			for ( uint32_t i = 0; i < l_count; i++ )
			{
				std::swap( *l_r, *l_b );
				l_r += l_bpp;
				l_b += l_bpp;
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
		PixelFormat l_ePF = PixelFormat::eR8G8B8;
		int l_flags = BMP_DEFAULT;
		FREE_IMAGE_FORMAT l_fiFormat = FreeImage_GetFileType( string::string_cast< char >( p_path ).c_str(), 0 );

		if ( l_fiFormat == FIF_UNKNOWN )
		{
			l_fiFormat = FreeImage_GetFIFFromFilename( string::string_cast< char >( p_path ).c_str() );
		}
		else if ( l_fiFormat == FIF_TIFF )
		{
			l_flags = TIFF_DEFAULT;
		}

		if ( l_fiFormat == FIF_UNKNOWN || !FreeImage_FIFSupportsReading( l_fiFormat ) )
		{
			LOADER_ERROR( "Can't load image : unsupported image format" );
		}

		auto l_fiImage = FreeImage_Load( l_fiFormat, string::string_cast< char >( p_path ).c_str() );

		if ( !l_fiImage )
		{
			BinaryFile l_file( p_path, uint32_t( File::OpenMode::eRead ) | uint32_t( File::OpenMode::eBinary ) );
			FreeImageIO l_fiIo;
			l_fiIo.read_proc = ReadProc;
			l_fiIo.write_proc = nullptr;
			l_fiIo.seek_proc = SeekProc;
			l_fiIo.tell_proc = TellProc;
			l_fiImage = FreeImage_LoadFromHandle( l_fiFormat, & l_fiIo, fi_handle( & l_file ), l_flags );

			if ( !l_fiImage )
			{
				LOADER_ERROR( "Can't load image : " + string::string_cast< char >( p_path ) );
			}
		}

		FREE_IMAGE_COLOR_TYPE l_type = FreeImage_GetColorType( l_fiImage );
		uint32_t l_width = FreeImage_GetWidth( l_fiImage );
		uint32_t l_height = FreeImage_GetHeight( l_fiImage );
		Size l_size( l_width, l_height );

		if ( l_type == FIC_RGBALPHA )
		{
			l_ePF = PixelFormat::eA8R8G8B8;
			FIBITMAP * l_dib = FreeImage_ConvertTo32Bits( l_fiImage );
			FreeImage_Unload( l_fiImage );
			l_fiImage = l_dib;

			if ( !l_fiImage )
			{
				LOADER_ERROR( "Can't convert image to 32 bits with alpha : " + string::string_cast< char >( p_path ) );
			}
		}
		else if ( l_fiFormat == FIF_HDR )
		{
			auto l_bpp = FreeImage_GetBPP( l_fiImage ) / 8;

			if ( l_bpp == pixel_definitions< PixelFormat::eRGB16F >::Size )
			{
				l_ePF = PixelFormat::eRGB16F;
			}
			else if ( l_bpp == pixel_definitions< PixelFormat::eRGB32F >::Size )
			{
				l_ePF = PixelFormat::eRGB32F;
			}
			else
			{
				LOADER_ERROR( "Unsupported HDR image format" );
			}
		}
		else
		{
			l_ePF = PixelFormat::eR8G8B8;
			FIBITMAP * l_dib = FreeImage_ConvertTo24Bits( l_fiImage );
			FreeImage_Unload( l_fiImage );
			l_fiImage = l_dib;

			if ( !l_fiImage )
			{
				LOADER_ERROR( "Can't convert image to 24 bits : " + string::string_cast< char >( p_path ) );
			}
		}

		if ( !p_image.m_buffer )
		{
			uint8_t * l_pixels = FreeImage_GetBits( l_fiImage );
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			SwapComponents( l_pixels, l_ePF, l_width, l_height );

#endif
			p_image.m_buffer = PxBufferBase::create( l_size, l_ePF, l_pixels, l_ePF );
			FreeImage_Unload( l_fiImage );
		}

		return p_image.m_buffer != nullptr;
	}

	//************************************************************************************************

	Image::BinaryWriter::BinaryWriter()
	{
	}

	bool Image::BinaryWriter::operator()( Image const & p_image, Path const & p_path )
	{
		bool l_result = false;
		FIBITMAP * l_fiImage = nullptr;
		Size const & l_size = p_image.GetDimensions();
		int32_t l_w = int32_t( l_size.width() );
		int32_t l_h = int32_t( l_size.height() );

		if ( p_path.GetExtension() == cuT( "png" ) )
		{
			l_fiImage = FreeImage_Allocate( l_w, l_h, 32 );
			PxBufferBaseSPtr l_pBufferRGB = PxBufferBase::create( l_size, PixelFormat::eA8R8G8B8, p_image.GetBuffer(), p_image.GetPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

			SwapComponents( l_pBufferRGB->ptr(), PixelFormat::eA8R8G8B8, l_w, l_h );

#endif

			if ( l_fiImage )
			{

				memcpy( FreeImage_GetBits( l_fiImage ), l_pBufferRGB->const_ptr(), l_pBufferRGB->size() );
				FREE_IMAGE_FORMAT l_fif = FIF_PNG;
				l_result = FreeImage_Save( l_fif, l_fiImage, string::string_cast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( l_fiImage );
			}
		}
		else if ( p_path.GetExtension() == cuT( "hdr" ) )
		{
			if ( PF::HasAlpha( p_image.GetPixelFormat() ) )
			{
				l_fiImage = FreeImage_AllocateT( FIT_RGBAF, l_w, l_h );
				PxBufferBaseSPtr l_pBufferRGB = PxBufferBase::create( l_size, PixelFormat::eRGBA32F, p_image.GetBuffer(), p_image.GetPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				SwapComponents( l_pBufferRGB->ptr(), PixelFormat::eRGBA32F, l_w, l_h );

#endif

				if ( l_fiImage )
				{
					memcpy( FreeImage_GetBits( l_fiImage ), l_pBufferRGB->const_ptr(), l_pBufferRGB->size() );
					FIBITMAP * l_dib = FreeImage_ConvertToRGBF( l_fiImage );
					FreeImage_Unload( l_fiImage );
					l_fiImage = l_dib;
				}
			}
			else
			{
				l_fiImage = FreeImage_AllocateT( FIT_RGBF, l_w, l_h );
				PxBufferBaseSPtr l_pBufferRGB = PxBufferBase::create( l_size, PixelFormat::eRGB32F, p_image.GetBuffer(), p_image.GetPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				SwapComponents( l_pBufferRGB->ptr(), PixelFormat::eRGB32F, l_w, l_h );

#endif

				if ( l_fiImage )
				{
					memcpy( FreeImage_GetBits( l_fiImage ), l_pBufferRGB->const_ptr(), l_pBufferRGB->size() );
				}
			}

			if ( l_fiImage )
			{
				FREE_IMAGE_FORMAT l_fif = FIF_HDR;
				l_result = FreeImage_Save( l_fif, l_fiImage, string::string_cast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( l_fiImage );
			}
		}
		else
		{
			PxBufferBaseSPtr l_pBuffer;

			if ( p_image.GetPixelFormat() != PixelFormat::eL8 )
			{
				l_fiImage = FreeImage_Allocate( l_w, l_h, 24 );
				l_pBuffer = PxBufferBase::create( l_size, PixelFormat::eR8G8B8, p_image.GetBuffer(), p_image.GetPixelFormat() );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

				SwapComponents( l_pBuffer->ptr(), PixelFormat::eR8G8B8, l_w, l_h );

#endif
				l_w *= 3;
			}
			else
			{
				l_fiImage = FreeImage_Allocate( l_w, l_h, 8 );
				l_pBuffer = p_image.GetPixels();
			}

			if ( l_fiImage && l_pBuffer )
			{
				uint32_t l_pitch = FreeImage_GetPitch( l_fiImage );
				uint8_t * l_dst = FreeImage_GetBits( l_fiImage );
				uint8_t const * l_src = l_pBuffer->const_ptr();

				if ( !( l_pitch % l_w ) )
				{
					memcpy( l_dst, l_src, l_pBuffer->size() );
				}
				else
				{
					for ( int32_t i = 0; i < l_h; ++i )
					{
						memcpy( l_dst, l_src, l_w );
						l_dst += l_pitch;
						l_src += l_w;
					}
				}

				l_result = FreeImage_Save( FIF_BMP, l_fiImage, string::string_cast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( l_fiImage );
			}
		}

		return l_result;
	}

	//************************************************************************************************

	Image & Image::Resample( Size const & p_size )
	{
		Size const & l_size = GetDimensions();

		if ( p_size != l_size )
		{
			FIBITMAP * l_fiImage = nullptr;
			int32_t l_w = int32_t( l_size.width() );
			int32_t l_h = int32_t( l_size.height() );
			PixelFormat l_ePF = GetPixelFormat();
			uint32_t l_uiBpp = PF::GetBytesPerPixel( l_ePF );

			switch ( l_ePF )
			{
			case PixelFormat::eR8G8B8:
				l_fiImage = FreeImage_AllocateT( FIT_BITMAP, l_w, l_h, 24 );
				break;

			case PixelFormat::eA8R8G8B8:
				l_fiImage = FreeImage_AllocateT( FIT_BITMAP, l_w, l_h, 32 );
				break;
			}

			if ( l_fiImage )
			{
				memcpy( FreeImage_GetBits( l_fiImage ), m_buffer->const_ptr(), m_buffer->size() );
				uint32_t l_width = p_size.width();
				uint32_t l_height = p_size.height();
				FREE_IMAGE_COLOR_TYPE l_type = FreeImage_GetColorType( l_fiImage );
				FIBITMAP * l_pRescaled = FreeImage_Rescale( l_fiImage, l_width, l_height, FILTER_BICUBIC );

				if ( l_pRescaled )
				{
					FreeImage_Unload( l_fiImage );
					l_fiImage = l_pRescaled;
					l_width = FreeImage_GetWidth( l_fiImage );
					l_height = FreeImage_GetHeight( l_fiImage );
					uint8_t * l_pixels = FreeImage_GetBits( l_fiImage );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR

					SwapComponents( l_pixels, m_buffer->format(), l_width, l_height );

#endif
					m_buffer = PxBufferBase::create( p_size, l_ePF, l_pixels, l_ePF );
				}

				FreeImage_Unload( l_fiImage );
			}
		}

		return *this;
	}

	void Image::InitialiseImageLib()
	{
		FreeImage_Initialise();
		FreeImage_SetOutputMessage( FreeImageErrorHandler );
	}

	void Image::CleanupImageLib()
	{
		FreeImage_DeInitialise();
	}

	//************************************************************************************************
}

#endif
