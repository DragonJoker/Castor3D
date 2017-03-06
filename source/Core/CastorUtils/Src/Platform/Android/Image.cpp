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
			PxBufferBaseSPtr l_result;
			int x = 0;
			int y = 0;
			int n = 0;
			uint8_t * l_data = stbi_load( string::string_cast< char >( p_path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( l_data )
			{
				PixelFormat l_format;

				switch ( n )
				{
				case 1:
					l_format = PixelFormat::eL8;
					break;

				case 2:
					l_format = PixelFormat::eA8L8;
					break;

				case 3:
					l_format = PixelFormat::eR8G8B8;
					break;

				case 4:
					l_format = PixelFormat::eA8R8G8B8;
					break;
				}

				l_result = PxBufferBase::create( Size( x, y ), l_format, l_data, l_format );
				stbi_image_free( l_data );
			}

			return l_result;
		}

		PxBufferBaseSPtr DoLoad32BitsPerChannel( Path const & p_path )
		{
			PxBufferBaseSPtr l_result;
			int x = 0;
			int y = 0;
			int n = 0;
			float * l_data = stbi_loadf( string::string_cast< char >( p_path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( l_data )
			{
				PixelFormat l_format;

				switch ( n )
				{
				case 1:
					l_format = PixelFormat::eL32F;
					break;

				case 2:
					l_format = PixelFormat::eAL32F;
					break;

				case 3:
					l_format = PixelFormat::eRGB32F;
					break;

				case 4:
					l_format = PixelFormat::eRGBA32F;
					break;
				}

				l_result = PxBufferBase::create( Size( x, y )
					, l_format
					, reinterpret_cast< uint8_t * >( l_data )
					, l_format );
				stbi_image_free( l_data );
			}

			return l_result;
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
		auto l_extension = string::upper_case( p_path.GetExtension() );

		if ( l_extension.find( cuT( "hdr" ) ) != String::npos )
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
		bool l_result = false;

		return l_result;
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
