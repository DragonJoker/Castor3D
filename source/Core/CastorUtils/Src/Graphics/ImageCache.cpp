#include "ImageCache.hpp"

#include "Image.hpp"
#include "Log/Logger.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace Castor
{
	namespace
	{
		static const xchar * INFO_CACHE_CREATED_OBJECT = cuT( "Cache::Create - Created " );
		static const xchar * WARNING_CACHE_DUPLICATE_OBJECT = cuT( "Cache::Create - Duplicate " );
		static const xchar * WARNING_CACHE_NULL_OBJECT = cuT( "Cache::Insert - nullptr " );
	}

	//*********************************************************************************************

	ImageCache::ImageCache()
	{
	}

	ImageCache::~ImageCache()
	{
	}

	ImageSPtr ImageCache::Add( String const & p_name, Path const & p_path )
	{
		auto l_lock = make_unique_lock( *this );
		ImageSPtr l_result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			l_result = Collection< Image, String >::find( p_name );

			if ( !l_result->GetBuffer() )
			{
				Image::BinaryLoader()( *l_result, p_path );
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			String l_name = p_path.GetFileName() + cuT( "." ) + p_path.GetExtension();

			if ( File::FileExists( p_path ) )
			{
				l_result = std::make_shared< Image >( p_name, p_path );
				Collection< Image, String >::insert( p_name, l_result );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
			}
			else
			{
				CASTOR_EXCEPTION( "Can't create the image [" + string::string_cast< char >( p_name ) + "], invalid path: " + string::string_cast< char >( p_path ) );
			}
		}

		return l_result;
	}

	ImageSPtr ImageCache::Add( String const & p_name, Size const & p_size, PixelFormat p_format )
	{
		auto l_lock = make_unique_lock( *this );
		ImageSPtr l_result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			l_result = Collection< Image, String >::find( p_name );

			if ( !l_result->GetBuffer() )
			{
				*l_result = std::move( Image( p_name, p_size, p_format ) );
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			l_result = std::make_shared< Image >( p_name, p_size, p_format );
			Collection< Image, String >::insert( p_name, l_result );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
		}

		return l_result;
	}
}
