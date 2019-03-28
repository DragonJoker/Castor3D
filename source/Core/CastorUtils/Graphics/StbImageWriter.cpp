#include "CastorUtils/Graphics/StbImageWriter.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Log/Logger.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined( CU_CompilerMSVC )
#	define STBI_MSC_SECURE_CRT
#endif
#include "stb_image_write.h"

namespace castor
{
	//************************************************************************************************

	namespace
	{
		int getComponentsCount( castor::PixelFormat format )
		{
			int result = 0;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
			case PixelFormat::eR16_SFLOAT:
			case PixelFormat::eR32_SFLOAT:
				result = 1;
				break;

			case PixelFormat::eR8A8_UNORM:
			case PixelFormat::eR16A16_SFLOAT:
			case PixelFormat::eR32A32_SFLOAT:
				result = 2;
				break;

			case PixelFormat::eR8G8B8_UNORM:
			case PixelFormat::eR8G8B8_SRGB:
			case PixelFormat::eB8G8R8_UNORM:
			case PixelFormat::eB8G8R8_SRGB:
			case PixelFormat::eR16G16B16_SFLOAT:
			case PixelFormat::eR32G32B32_SFLOAT:
				result = 3;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
			case PixelFormat::eA8B8G8R8_SRGB:
			case PixelFormat::eR8G8B8A8_UNORM:
			case PixelFormat::eR8G8B8A8_SRGB:
			case PixelFormat::eR16G16B16A16_SFLOAT:
			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = 4;
				break;

			default:
				CU_LoaderError( "Unsupported image format for writing." );
				break;
			}

			return result;
		}

		bool doSave8BitsPerChannel( Path const & path
			, PxBufferBase const & buffer )
		{
			bool result = false;
			int n = getComponentsCount( buffer.getFormat() );

			if ( !n )
			{
				CU_LoaderError( cuT( "Couldn't write file " ) + path + cuT( ":\nUnsupported pixel format." ) );
			}

			int x = int( buffer.getWidth() );
			int y = int( buffer.getHeight() );
			stbi_flip_vertically_on_write( 1u );
			auto extension = string::upperCase( path.getExtension() );

			if ( extension == "PNG" )
			{
				result = stbi_write_png( string::stringCast< char >( path ).c_str()
					, x
					, y
					, n
					, buffer.getConstPtr()
					, int( buffer.getSize() / buffer.getHeight() ) ) != 0;
			}
			else if ( extension == "BMP" )
			{
				result = stbi_write_bmp( string::stringCast< char >( path ).c_str()
					, x
					, y
					, n
					, buffer.getConstPtr() ) != 0;
			}
			else if ( extension == "TGA" )
			{
				result = stbi_write_tga( string::stringCast< char >( path ).c_str()
					, x
					, y
					, n
					, buffer.getConstPtr() ) != 0;
			}
			else if ( extension == "JPG"
				|| extension == "JPEG" )
			{
				result = stbi_write_jpg( string::stringCast< char >( path ).c_str()
					, x
					, y
					, n
					, buffer.getConstPtr()
					, 90 ) != 0;
			}
			else
			{
				CU_LoaderError( cuT( "Couldn't write file " ) + path + cuT( ":\nUnsupported image format." ) );
			}

			if ( !result )
			{
				CU_LoaderError( cuT( "Couldn't write file [" ) + path + cuT( "]:\nInternal error" ) );
			}

			return result;
		}

		bool doSave32BitsPerChannel( Path const & path
			, PxBufferBase const & buffer )
		{
			bool result = false;
			int n = getComponentsCount( buffer.getFormat() );

			if ( !n )
			{
				CU_LoaderError( cuT( "Couldn't write file " ) + path + cuT( ":\nUnsupported pixel format." ) );
			}

			int x = int( buffer.getWidth() );
			int y = int( buffer.getHeight() );
			Path realPath = path.getPath() / ( path.getFileName() + ".hdr" );
			stbi_flip_vertically_on_write( 1u );
			auto data = reinterpret_cast< float const * >( buffer.getConstPtr() );
			result = stbi_write_hdr( string::stringCast< char >( realPath ).c_str()
				, x
				, y
				, n
				, data ) != 0;

			if ( !result )
			{
				CU_LoaderError( cuT( "Couldn't write file [" ) + path + cuT( "]:\nInternal error" ) );
			}

			return result;
		}

		StringArray const & listExtensions()
		{
			static StringArray const list
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
			};
			return list;
		}
	}

	//************************************************************************************************

	void StbImageWriter::registerWriter( ImageWriter & reg )
	{
		reg.registerWriter( listExtensions()
			, std::make_unique< StbImageWriter >() );
	}

	void StbImageWriter::unregisterWriter( ImageWriter & reg )
	{
		reg.unregisterWriter( listExtensions() );
	}

	bool StbImageWriter::write( Path const & path
		, PxBufferBase const & buffer )const
	{
		bool result = false;

		if ( buffer.getFormat() == PixelFormat::eR16A16_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR32A32_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR16_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR32_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR16G16B16_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR32G32B32_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR16G16B16A16_SFLOAT
			|| buffer.getFormat() == PixelFormat::eR32G32B32A32_SFLOAT )
		{
			result = doSave32BitsPerChannel( path, buffer );
		}
		else
		{
			result = doSave8BitsPerChannel( path, buffer );
		}

		return result;
	}
}
