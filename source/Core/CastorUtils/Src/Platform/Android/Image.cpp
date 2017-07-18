#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Data/Path.hpp"
#include "Graphics/Image.hpp"
#include "Graphics/Rectangle.hpp"
#include "Log/Logger.hpp"
#include "Miscellaneous/StringUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Castor
{
	namespace
	{
		PxBufferBaseSPtr DoLoad8BitsPerChannel( Path const & p_path )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			uint8_t * data = stbi_load( string::string_cast< char >( p_path ).c_str()
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

		PxBufferBaseSPtr DoLoad32BitsPerChannel( Path const & p_path )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			float * data = stbi_loadf( string::string_cast< char >( p_path ).c_str()
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

	bool Image::BinaryLoader::operator()( Image & p_image, Path const & p_path )
	{
		if ( p_path.empty() )
		{
			LOADER_ERROR( "Can't load image : path is empty" );
		}

		p_image.m_buffer.reset();
		auto extension = string::upper_case( p_path.GetExtension() );

		if ( extension.find( cuT( "hdr" ) ) != String::npos )
		{
			p_image.m_buffer = DoLoad32BitsPerChannel( p_path );
		}
		else
		{
			p_image.m_buffer = DoLoad8BitsPerChannel( p_path );
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

		return result;
	}

	//************************************************************************************************

	Image & Image::Resample( Size const & p_size )
	{
		return *this;
	}

	void Image::InitialiseImageLib()
	{
	}

	void Image::CleanupImageLib()
	{
	}

	//************************************************************************************************
}

#endif
