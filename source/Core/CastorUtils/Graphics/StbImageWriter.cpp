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
			auto asciiPath = toUtf8( path );

			if ( !n )
			{
				CU_LoaderError( "Couldn't write file " + asciiPath + ":\nUnsupported pixel format." );
			}

			auto x = int( buffer.getWidth() );
			auto y = int( buffer.getHeight() );
			stbi_flip_vertically_on_write( 1u );

			if ( auto extension = string::upperCase( path.getExtension() );
				extension == cuT( "PNG" ) )
			{
				result = stbi_write_png( asciiPath.c_str()
					, x
					, y
					, n
					, buffer.getConstPtr()
					, int( buffer.getSize() / buffer.getHeight() ) ) != 0;
			}
			else if ( extension == cuT( "BMP" ) )
			{
				result = stbi_write_bmp( asciiPath.c_str()
					, x
					, y
					, n
					, buffer.getConstPtr() ) != 0;
			}
			else if ( extension == cuT( "TGA" ) )
			{
				result = stbi_write_tga( asciiPath.c_str()
					, x
					, y
					, n
					, buffer.getConstPtr() ) != 0;
			}
			else if ( extension == cuT( "JPG" )
				|| extension == cuT( "JPEG" ) )
			{
				result = stbi_write_jpg( asciiPath.c_str()
					, x
					, y
					, n
					, buffer.getConstPtr()
					, 90 ) != 0;
			}
			else
			{
				CU_LoaderError( "Couldn't write file " + asciiPath + ":\nUnsupported image format." );
			}

			if ( !result )
			{
				CU_LoaderError( "Couldn't write file [" + asciiPath + "]:\nInternal error" );
			}

			return result;
		}

		static bool doSave32BitsPerChannel( Path const & path
			, PxBufferBase const & buffer )
		{
			bool result = false;
			int n = getComponentsCount( buffer.getFormat() );
			auto asciiPath = toUtf8( path );

			if ( !n )
			{
				CU_LoaderError( "Couldn't write file " + asciiPath + ":\nUnsupported pixel format." );
			}

			auto x = int( buffer.getWidth() );
			auto y = int( buffer.getHeight() );
			Path realPath = path.getPath() / ( path.getFileName() + cuT( ".hdr" ) );
			stbi_flip_vertically_on_write( 1u );
			using FloatCPtr = float const *;
			auto data = reinterpret_cast< FloatCPtr >( buffer.getConstPtr() );
			result = stbi_write_hdr( toUtf8( realPath ).c_str()
				, x
				, y
				, n
				, data ) != 0;

			if ( !result )
			{
				CU_LoaderError( "Couldn't write file [" + asciiPath + "]:\nInternal error" );
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
			, castor::make_unique< StbImageWriter >() );
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
