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
#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wnull-dereference"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include "stb_image_write.h"
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
#pragma warning( pop )

namespace castor
{
	//************************************************************************************************

	namespace
	{
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

		if ( isFloatingPoint( buffer.getFormat() ) )
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
