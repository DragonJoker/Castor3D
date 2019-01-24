#include "Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

#include "Data/Path.hpp"
#include "Graphics/Image.hpp"
#include "Graphics/Rectangle.hpp"
#include "Log/Logger.hpp"
#include "Miscellaneous/StringUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace castor
{
	namespace
	{
		PxBufferBaseSPtr doLoad8BitsPerChannel( Path const & path )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			uint8_t * data = stbi_load( string::stringCast< char >( path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( n )
				{
				case 1:
					format = PixelFormat::eL8;
					break;

				case 2:
					format = PixelFormat::eA8L8;
					break;

				case 3:
					format = PixelFormat::eR8G8B8;
					break;

				case 4:
					format = PixelFormat::eA8R8G8B8;
					break;
				}

				result = PxBufferBase::create( Size( x, y ), format, data, format );
				stbi_image_free( data );
			}

			return result;
		}

		PxBufferBaseSPtr doLoad32BitsPerChannel( Path const & path )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			float * data = stbi_loadf( string::stringCast< char >( path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( n )
				{
				case 1:
					format = PixelFormat::eL32F;
					break;

				case 2:
					format = PixelFormat::eAL32F;
					break;

				case 3:
					format = PixelFormat::eRGB32F;
					break;

				case 4:
					format = PixelFormat::eRGBA32F;
					break;
				}

				result = PxBufferBase::create( Size( x, y )
					, format
					, reinterpret_cast< uint8_t * >( data )
					, format );
				stbi_image_free( data );
			}

			return result;
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
		auto extension = string::upperCase( path.getExtension() );

		if ( extension.find( cuT( "hdr" ) ) != String::npos )
		{
			image.m_buffer = doLoad32BitsPerChannel( path );
		}
		else
		{
			image.m_buffer = doLoad8BitsPerChannel( path );
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

		return result;
	}

	//************************************************************************************************

	Image & Image::resample( Size const & p_size )
	{
		return *this;
	}

	void Image::initialiseImageLib()
	{
	}

	void Image::cleanupImageLib()
	{
	}

	//************************************************************************************************
}

#endif
