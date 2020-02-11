#include "CastorUtils/Graphics/ImageCache.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace castor
{
	namespace
	{
		static const xchar * InfoCacheCreatedObject = cuT( "Cache::create - Created " );
		static const xchar * WarningCacheDuplicateObject = cuT( "Cache::create - Duplicate " );
		static const xchar * WarningCacheNullObject = cuT( "Cache::Insert - nullptr " );

		inline void doReportCreation( castor::String const & name )
		{
			castor::Logger::logTrace( castor::makeStringStream()
				<< InfoCacheCreatedObject
				<< cuT( "Image: " )
				<< name );
		}

		inline void doReportDuplicate( castor::String const & name )
		{
			castor::Logger::logWarning( castor::makeStringStream()
				<< WarningCacheDuplicateObject
				<< cuT( "Image: " )
				<< name );
		}

		inline void doReportNull()
		{
			castor::Logger::logWarning( castor::makeStringStream()
				<< WarningCacheNullObject
				<< cuT( "Image" ) );
		}
	}

	//*********************************************************************************************

	ImageCache::ImageCache( ImageLoader const & loader )
		: m_loader{ loader }
	{
	}

	ImageCache::~ImageCache()
	{
	}

	ImageSPtr ImageCache::add( String const & name
		, Path const & path )
	{
		using LockType = std::unique_lock< ImageCache >;
		LockType lock{ makeUniqueLock( *this ) };
		ImageSPtr result;

		if ( Collection< Image, String >::has( name ) )
		{
			result = Collection< Image, String >::find( name );

			if ( !result->getBuffer() )
			{
				*result = std::move( Image( name, m_loader.load( path ) ) );
			}
			else
			{
				doReportDuplicate( name );
			}
		}
		else
		{
			if ( File::fileExists( path ) )
			{
				result = std::make_shared< Image >( name, path, m_loader );
				Collection< Image, String >::insert( name, result );
				doReportCreation( name );
			}
			else
			{
				CU_Exception( "Can't create the image [" + string::stringCast< char >( name ) + "], invalid path: " + string::stringCast< char >( path ) );
			}
		}

		return result;
	}

	ImageSPtr ImageCache::add( String const & name
		, Size const & size
		, PixelFormat format )
	{
		using LockType = std::unique_lock< ImageCache >;
		LockType lock{ makeUniqueLock( *this ) };
		ImageSPtr result;

		if ( Collection< Image, String >::has( name ) )
		{
			result = Collection< Image, String >::find( name );

			if ( !result->getBuffer() )
			{
				*result = std::move( Image( name, size, format ) );
			}
			else
			{
				doReportDuplicate( name );
			}
		}
		else
		{
			result = std::make_shared< Image >( name, size, format );
			Collection< Image, String >::insert( name, result );
			doReportCreation( name );
		}

		return result;
	}
}
