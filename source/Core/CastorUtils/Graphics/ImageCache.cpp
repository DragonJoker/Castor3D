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

		inline void doReportCreation( LoggerInstance & logger
			, castor::String const & name )
		{
			logger.logTrace( castor::makeStringStream()
				<< InfoCacheCreatedObject
				<< cuT( "Image: " )
				<< name );
		}

		inline void doReportDuplicate( LoggerInstance & logger
			, castor::String const & name )
		{
			logger.logWarning( castor::makeStringStream()
				<< WarningCacheDuplicateObject
				<< cuT( "Image: " )
				<< name );
		}

		inline void doReportNull( LoggerInstance & logger )
		{
			logger.logWarning( castor::makeStringStream()
				<< WarningCacheNullObject
				<< cuT( "Image" ) );
		}
	}

	//*********************************************************************************************

	ImageCache::ImageCache( LoggerInstance & logger
		, ImageLoader const & loader )
		: Collection< Image, String >{ logger }
		, m_loader{ loader }
	{
	}

	ImageCache::~ImageCache()
	{
	}

	ImageSPtr ImageCache::add( String const & name
		, Path const & path
		, bool allowCompression )
	{
		using LockType = std::unique_lock< ImageCache >;
		LockType lock{ makeUniqueLock( *this ) };
		ImageSPtr result;

		if ( Collection< Image, String >::has( name ) )
		{
			result = Collection< Image, String >::find( name );

			if ( !result->hasBuffer() )
			{
				*result = m_loader.load( name, path, allowCompression );
			}
			else
			{
				doReportDuplicate( getLogger(), name );
			}
		}
		else
		{
			if ( File::fileExists( path ) )
			{
				result = std::make_shared< Image >( m_loader.load( name, path, allowCompression ) );
				Collection< Image, String >::insert( name, result );
				doReportCreation( getLogger(), name );
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

			if ( !result->hasBuffer() )
			{
				*result = std::move( Image( name, Path{}, size, format ) );
			}
			else
			{
				doReportDuplicate( getLogger(), name );
			}
		}
		else
		{
			result = std::make_shared< Image >( name, Path{}, size, format );
			Collection< Image, String >::insert( name, result );
			doReportCreation( getLogger(), name );
		}

		return result;
	}
}
