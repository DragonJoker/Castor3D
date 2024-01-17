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
#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include "stb_image_write.h"
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	//************************************************************************************************

	namespace stbiw
	{
		static bool doSave8BitsPerChannel( Path const & path
			, PxBufferBase const & buffer )
		{
			bool result = false;
			int n = getComponentsCount( buffer.getFormat() );

			if ( !n )
			{
				CU_LoaderError( cuT( "Couldn't write file " ) + path + cuT( ":\nUnsupported pixel format." ) );
			}

			auto x = int( buffer.getWidth() );
			auto y = int( buffer.getHeight() );
			stbi_flip_vertically_on_write( 1u );

			if ( auto extension = string::upperCase( path.getExtension() );
				extension == "PNG" )
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

		static bool doSave32BitsPerChannel( Path const & path
			, PxBufferBase const & buffer )
		{
			bool result = false;
			int n = getComponentsCount( buffer.getFormat() );

			if ( !n )
			{
				CU_LoaderError( cuT( "Couldn't write file " ) + path + cuT( ":\nUnsupported pixel format." ) );
			}

			auto x = int( buffer.getWidth() );
			auto y = int( buffer.getHeight() );
			Path realPath = path.getPath() / ( path.getFileName() + ".hdr" );
			stbi_flip_vertically_on_write( 1u );
			using FloatCPtr = float const *;
			auto data = FloatCPtr( buffer.getConstPtr() );
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

		static StringArray const StbExtensions
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

		static StringArray const & listExtensions()
		{
			return StbExtensions;
		}
	}

	//************************************************************************************************

	void StbImageWriter::registerWriter( ImageWriter & reg )
	{
		reg.registerWriter( stbiw::listExtensions()
			, std::make_unique< StbImageWriter >() );
	}

	void StbImageWriter::unregisterWriter( ImageWriter & reg )
	{
		reg.unregisterWriter( stbiw::listExtensions() );
	}

	bool StbImageWriter::write( Path const & path
		, PxBufferBase const & buffer )const
	{
		bool result = false;

		if ( isFloatingPoint( buffer.getFormat() ) )
		{
			result = stbiw::doSave32BitsPerChannel( path, buffer );
		}
		else
		{
			result = stbiw::doSave8BitsPerChannel( path, buffer );
		}

		return result;
	}
}
