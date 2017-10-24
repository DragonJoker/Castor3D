#include "ImageCache.hpp"

#include "Image.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace castor
{
	namespace
	{
		static const xchar * INFO_CACHE_CREATED_OBJECT = cuT( "Cache::create - Created " );
		static const xchar * WARNING_CACHE_DUPLICATE_OBJECT = cuT( "Cache::create - Duplicate " );
		static const xchar * WARNING_CACHE_NULL_OBJECT = cuT( "Cache::Insert - nullptr " );
	}

	//*********************************************************************************************

	ImageCache::ImageCache()
	{
	}

	ImageCache::~ImageCache()
	{
	}

	ImageSPtr ImageCache::add( String const & p_name, Path const & p_path )
	{
		auto lock = makeUniqueLock( *this );
		ImageSPtr result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			result = Collection< Image, String >::find( p_name );

			if ( !result->getBuffer() )
			{
				Image::BinaryLoader()( *result, p_path );
			}
			else
			{
				castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			String name = p_path.getFileName() + cuT( "." ) + p_path.getExtension();

			if ( File::fileExists( p_path ) )
			{
				result = std::make_shared< Image >( p_name, p_path );
				Collection< Image, String >::insert( p_name, result );
				castor::Logger::logDebug( castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
			}
			else
			{
				CASTOR_EXCEPTION( "Can't create the image [" + string::stringCast< char >( p_name ) + "], invalid path: " + string::stringCast< char >( p_path ) );
			}
		}

		return result;
	}

	ImageSPtr ImageCache::add( String const & p_name, Size const & p_size, PixelFormat p_format )
	{
		auto lock = makeUniqueLock( *this );
		ImageSPtr result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			result = Collection< Image, String >::find( p_name );

			if ( !result->getBuffer() )
			{
				*result = std::move( Image( p_name, p_size, p_format ) );
			}
			else
			{
				castor::Logger::logWarning( castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			result = std::make_shared< Image >( p_name, p_size, p_format );
			Collection< Image, String >::insert( p_name, result );
			castor::Logger::logDebug( castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
		}

		return result;
	}
}
