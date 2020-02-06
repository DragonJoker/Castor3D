﻿#include "Image.hpp"
#include "Path.hpp"
#include "Rectangle.hpp"
#include "BinaryFile.hpp"
#include "Logger.hpp"

extern "C"
{
#	include <FreeImage.h>
}

namespace Castor
{
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
			return l_pFile->Seek( p_lOffset, File::eOFFSET_MODE( p_iOrigin ) );
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
	}

	void Image::BinaryLoader::InitialiseImageLib()
	{
		FreeImage_Initialise();
		FreeImage_SetOutputMessage( FreeImageErrorHandler );
	}

	void Image::BinaryLoader::CleanupImageLib()
	{
		FreeImage_DeInitialise();
	}
	
	Image::BinaryLoader::BinaryLoader()
		: Loader< Image, eFILE_TYPE_BINARY, BinaryFile >{ File::eOPEN_MODE_DUMMY }
	{
	}

	bool Image::BinaryLoader::operator()( Image & p_image, Path const & p_path )
	{
		if ( p_path.empty() )
		{
			LOADER_ERROR( "Can't load image : path is empty" );
		}

		p_image.m_pBuffer.reset();
		ePIXEL_FORMAT l_ePF = ePIXEL_FORMAT_R8G8B8;
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
			BinaryFile l_file( p_path, File::eOPEN_MODE_READ | File::eOPEN_MODE_BINARY );
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
			l_ePF = ePIXEL_FORMAT_A8R8G8B8;
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

			if ( l_bpp == pixel_definitions< ePIXEL_FORMAT_RGB16F >::Size )
			{
				l_ePF = ePIXEL_FORMAT_RGB16F;
			}
			else if ( l_bpp == pixel_definitions< ePIXEL_FORMAT_RGB32F >::Size )
			{
				l_ePF = ePIXEL_FORMAT_RGB32F;
			}
			else
			{
				LOADER_ERROR( "Unsupported HDR image format" );
			}
		}
		else
		{
			l_ePF = ePIXEL_FORMAT_R8G8B8;
			FIBITMAP * l_dib = FreeImage_ConvertTo24Bits( l_fiImage );
			FreeImage_Unload( l_fiImage );
			l_fiImage = l_dib;

			if ( !l_fiImage )
			{
				LOADER_ERROR( "Can't convert image to 24 bits : " + string::string_cast< char >( p_path ) );
			}
		}

		if ( !p_image.m_pBuffer )
		{
			uint8_t * l_pixels = FreeImage_GetBits( l_fiImage );
			//0=Blue, 1=Green, 2=Red, 3=Alpha
#if FREEIMAGE_COLORORDER != FREEIMAGE_COLORORDER_BGR
			uint8_t * l_pTmp = l_pixels;
			uint32_t l_uiSize = l_width * l_height;

			for ( uint32_t i = 0; i < l_uiSize; i++ )
			{
				std::swap( l_pTmp[0], l_pTmp[2] );
				l_pTmp += 4;
			}

#endif
			p_image.m_pBuffer = PxBufferBase::create( l_size, l_ePF, l_pixels, l_ePF );
			FreeImage_Unload( l_fiImage );
		}

		return p_image.m_pBuffer != nullptr;
	}

	bool Image::BinaryLoader::operator()( Image const & p_image, Path const & p_path )
	{
		bool l_return = false;
		FIBITMAP * l_fiImage = nullptr;
		Size const & l_size = p_image.GetDimensions();
		int32_t l_w = int32_t( l_size.width() );
		int32_t l_h = int32_t( l_size.height() );

		if ( p_path.GetExtension() == cuT( "png" ) )
		{
			l_fiImage = FreeImage_Allocate( l_w, l_h, 32 );
			PxBufferBaseSPtr l_pBufferRGB = PxBufferBase::create( l_size, ePIXEL_FORMAT_A8R8G8B8, p_image.GetBuffer(), p_image.GetPixelFormat() );

			if ( l_fiImage )
			{
				memcpy( FreeImage_GetBits( l_fiImage ), l_pBufferRGB->const_ptr(), l_pBufferRGB->size() );
				FREE_IMAGE_FORMAT l_fif = FIF_PNG;
				l_return = FreeImage_Save( l_fif, l_fiImage, string::string_cast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( l_fiImage );
			}
		}
		else
		{
			PxBufferBaseSPtr l_pBuffer;

			if ( p_image.GetPixelFormat() != ePIXEL_FORMAT_L8 )
			{
				l_fiImage = FreeImage_Allocate( l_w, l_h, 24 );
				l_pBuffer = PxBufferBase::create( l_size, ePIXEL_FORMAT_R8G8B8, p_image.GetBuffer(), p_image.GetPixelFormat() );
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

				l_return = FreeImage_Save( FIF_BMP, l_fiImage, string::string_cast< char >( p_path ).c_str(), 0 ) != 0;
				FreeImage_Unload( l_fiImage );
			}
		}

		return l_return;
	}

	Image::Image( String const & p_name, Size const & p_size, ePIXEL_FORMAT p_format, ByteArray const & p_buffer, ePIXEL_FORMAT p_eBufferFormat )
		: Resource< Image >( p_name )
		, m_pBuffer( PxBufferBase::create( p_size, p_format, &p_buffer[0], p_eBufferFormat ) )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, Size const & p_size, ePIXEL_FORMAT p_format, uint8_t const * p_buffer, ePIXEL_FORMAT p_eBufferFormat )
		: Resource< Image >( p_name )
		, m_pBuffer( PxBufferBase::create( p_size, p_format, p_buffer, p_eBufferFormat ) )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, PxBufferBase const & p_buffer )
		: Resource< Image >( p_name )
		, m_pBuffer( p_buffer.clone() )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, Path const & p_pathFile )
		: Resource< Image >( p_name )
		, m_pathFile( p_pathFile )
	{
		Image::BinaryLoader()( *this, p_pathFile );
	}

	Image::Image( Image const & p_image )
		: Resource< Image >( p_image )
		, m_pathFile( p_image.m_pathFile )
		, m_pBuffer( p_image.m_pBuffer->clone() )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( Image && p_image )
		: Resource< Image >( std::move( p_image ) )
		, m_pathFile( std::move( p_image.m_pathFile ) )
		, m_pBuffer( std::move( p_image.m_pBuffer ) )
	{
		p_image.m_pathFile.clear();
		p_image.m_pBuffer.reset();
		CHECK_INVARIANTS();
	}

	Image & Image::operator=( Image const & p_image )
	{
		Resource< Image >::operator=( p_image );
		m_pathFile = p_image.m_pathFile;
		m_pBuffer = p_image.m_pBuffer->clone();
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::operator =( Image && p_image )
	{
		Resource< Image >::operator=( std::move( p_image ) );

		if ( this != & p_image )
		{
			m_pathFile = std::move( p_image.m_pathFile );
			m_pBuffer = std::move( p_image.m_pBuffer );
			p_image.m_pathFile.clear();
			p_image.m_pBuffer.reset();
			CHECK_INVARIANTS();
		}

		return * this;
	}

	Image::~Image()
	{
	}

	BEGIN_INVARIANT_BLOCK( Image )
	CHECK_INVARIANT( m_pBuffer->count() > 0 );
	END_INVARIANT_BLOCK()

	Image & Image::Resample( Size const & p_size )
	{
		Size const & l_size = GetDimensions();

		if ( p_size != l_size )
		{
			FIBITMAP * l_fiImage = nullptr;
			int32_t l_w = int32_t( l_size.width() );
			int32_t l_h = int32_t( l_size.height() );
			ePIXEL_FORMAT l_ePF = GetPixelFormat();
			uint32_t l_uiBpp = PF::GetBytesPerPixel( l_ePF );

			switch ( l_ePF )
			{
			case ePIXEL_FORMAT_R8G8B8:
				l_fiImage = FreeImage_AllocateT( FIT_BITMAP, l_w, l_h, 24 );
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_fiImage = FreeImage_AllocateT( FIT_BITMAP, l_w, l_h, 32 );
				break;
			}

			if ( l_fiImage )
			{
				memcpy( FreeImage_GetBits( l_fiImage ), m_pBuffer->const_ptr(), m_pBuffer->size() );
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
					//0=Blue, 1=Green, 2=Red, 3=Alpha
#if FREEIMAGE_COLORORDER != FREEIMAGE_COLORORDER_BGR
					uint8_t * l_pTmp = l_pixels;
					uint32_t l_uiSize = l_width * l_height;

					for ( uint32_t i = 0; i < l_uiSize; i++ )
					{
						std::swap( l_pTmp[0], l_pTmp[2] );
						l_pTmp += l_uiBpp;
					}

#endif
					m_pBuffer = PxBufferBase::create( l_size, l_ePF, l_pixels, l_ePF );
				}

				FreeImage_Unload( l_fiImage );
			}
		}

		return *this;
	}

	Image & Image::Fill( Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		SetPixel( 0, 0, p_clrColour );
		uint32_t l_uiBpp = PF::GetBytesPerPixel( GetPixelFormat() );

		for ( uint32_t i = l_uiBpp; i < m_pBuffer->size(); i += l_uiBpp )
		{
			memcpy( &m_pBuffer->ptr()[i], &m_pBuffer->const_ptr()[0], l_uiBpp );
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::SetPixel( uint32_t x, uint32_t y, uint8_t const * p_pPixel )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() && p_pPixel );
		memcpy( &m_pBuffer->ptr()[( x + y * GetWidth() ) * PF::GetBytesPerPixel( GetPixelFormat() )], p_pPixel, PF::GetBytesPerPixel( GetPixelFormat() ) );
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::SetPixel( uint32_t x, uint32_t y, Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() );
		Point4ub l_ptComponents = bgra_byte( p_clrColour );
		uint8_t const * l_pSrc = l_ptComponents.const_ptr();
		uint8_t * l_pDst = &( *m_pBuffer->get_at( x, y ) );
		PF::ConvertPixel( ePIXEL_FORMAT_A8R8G8B8, l_pSrc, GetPixelFormat(), l_pDst );
		CHECK_INVARIANTS();
		return * this;
	}

	void Image::GetPixel( uint32_t x, uint32_t y, uint8_t * p_pPixel, ePIXEL_FORMAT p_format )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() && p_pPixel );
		uint8_t const * l_pSrc = &( *m_pBuffer->get_at( x, y ) );
		uint8_t * l_pDst = p_pPixel;
		PF::ConvertPixel( GetPixelFormat(), l_pSrc, p_format, l_pDst );
		CHECK_INVARIANTS();
	}

	Colour Image::GetPixel( uint32_t x, uint32_t y )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() );
		Point4ub l_ptComponents;
		uint8_t const * l_pSrc = &( *m_pBuffer->get_at( x, y ) );
		uint8_t * l_pDst = l_ptComponents.ptr();
		PF::ConvertPixel( GetPixelFormat(), l_pSrc, ePIXEL_FORMAT_A8R8G8B8, l_pDst );
		CHECK_INVARIANTS();
		return Colour::from_bgra( l_ptComponents );
	}

	Image & Image::CopyImage( Image const & p_src )
	{
		CHECK_INVARIANTS();

		if ( GetDimensions() == p_src.GetDimensions() )
		{
			if ( GetPixelFormat() == p_src.GetPixelFormat() )
			{
				m_pBuffer = p_src.m_pBuffer->clone();
			}
			else
			{
				for ( uint32_t i = 0; i < GetWidth(); ++i )
				{
					for ( uint32_t j = 0; j < GetHeight(); ++j )
					{
						uint8_t const * l_pSrc = &( *p_src.m_pBuffer->get_at( i, j ) );
						uint8_t * l_pDst = &( *m_pBuffer->get_at( i, j ) );
						PF::ConvertPixel( GetPixelFormat(), l_pSrc, GetPixelFormat(), l_pDst );
					}
				}
			}
		}
		else
		{
			Point2d l_ptSrcStep( static_cast< double >( p_src.GetWidth() ) / GetWidth(), static_cast< double >( p_src.GetHeight() ) / GetHeight() );

			if ( GetPixelFormat() == p_src.GetPixelFormat() )
			{
				uint8_t * l_pSrcPix	= new uint8_t[PF::GetBytesPerPixel( m_pBuffer->format() )];

				for ( uint32_t i = 0; i < GetWidth(); ++i )
				{
					for ( uint32_t j = 0; j < GetHeight(); ++j )
					{
						p_src.GetPixel( static_cast< uint32_t >( i * l_ptSrcStep[0] ), static_cast< uint32_t >( j * l_ptSrcStep[1] ), l_pSrcPix, GetPixelFormat() );
						SetPixel( i, j, l_pSrcPix );
					}
				}
			}
			else
			{
				// Parcours des pixels de l'image source et conversion / copie dans l'image courante
				ByteArray l_pSrcPix( PF::GetBytesPerPixel( p_src.GetPixelFormat() ), 0 );
				ByteArray l_pDestPix( PF::GetBytesPerPixel( GetPixelFormat() ), 0 );

				for ( uint32_t i = 0; i < GetWidth(); ++i )
				{
					for ( uint32_t j = 0; j < GetHeight(); ++j )
					{
						p_src.GetPixel( static_cast< uint32_t >( i * l_ptSrcStep[0] ), static_cast< uint32_t >( j * l_ptSrcStep[1] ), l_pSrcPix.data(), p_src.GetPixelFormat() );
						uint8_t const * l_pSrc = l_pSrcPix.data();
						uint8_t * l_pDst = l_pDestPix.data();
						PF::ConvertPixel( p_src.GetPixelFormat(), l_pSrc, GetPixelFormat(), l_pDst );
						SetPixel( i, j, l_pDestPix.data() );
					}
				}
			}
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image Image::SubImage( Rectangle const & l_rcRect )const
	{
		CHECK_INVARIANTS();
		REQUIRE( Rectangle( 0, 0 , GetWidth(), GetHeight() ).intersects( l_rcRect ) == eINTERSECTION_IN );
		Size l_ptSize( l_rcRect.width(), l_rcRect.height() );
		// Création de la sous-image à remplir
		Image l_img( m_name + cuT( "_Sub" ) + string::to_string( l_rcRect[0] ) + cuT( "x" ) + string::to_string( l_rcRect[1] ) + cuT( ":" ) + string::to_string( l_ptSize.width() ) + cuT( "x" ) + string::to_string( l_ptSize.height() ), l_ptSize, GetPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * l_pSrc = &( *m_pBuffer->get_at( l_rcRect.left(), l_rcRect.top() ) );
		uint8_t * l_pDest = &( *l_img.m_pBuffer->get_at( 0, 0 ) );
		uint32_t l_uiSrcPitch = GetWidth() * PF::GetBytesPerPixel( GetPixelFormat() );
		uint32_t l_uiDestPitch = l_img.GetWidth() * PF::GetBytesPerPixel( l_img.GetPixelFormat() );

		// Copie des pixels de l'image originale dans la sous-image
		for ( int i = l_rcRect.left(); i < l_rcRect.right(); ++i )
		{
			std::memcpy( l_pDest, l_pSrc, l_uiDestPitch );
			l_pSrc  += l_uiSrcPitch;
			l_pDest += l_uiDestPitch;
		}

		CHECK_INVARIANTS();
		return l_img;
	}

	Image & Image::Flip()
	{
		CHECK_INVARIANTS();
		m_pBuffer->flip();
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::Mirror()
	{
		CHECK_INVARIANTS();
		m_pBuffer->mirror();
		CHECK_INVARIANTS();
		return * this;
	}
}
