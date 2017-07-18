#include "ImageCache.hpp"

#include "Image.hpp"

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
		auto lock = make_unique_lock( *this );
		ImageSPtr result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			result = Collection< Image, String >::find( p_name );

			if ( !result->GetBuffer() )
			{
				Image::BinaryLoader()( *result, p_path );
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			String name = p_path.GetFileName() + cuT( "." ) + p_path.GetExtension();

			if ( File::FileExists( p_path ) )
			{
				result = std::make_shared< Image >( p_name, p_path );
				Collection< Image, String >::insert( p_name, result );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
			}
			else
			{
				CASTOR_EXCEPTION( "Can't create the image [" + string::string_cast< char >( p_name ) + "], invalid path: " + string::string_cast< char >( p_path ) );
			}
		}

		return result;
	}

	ImageSPtr ImageCache::Add( String const & p_name, Size const & p_size, PixelFormat p_format )
	{
		auto lock = make_unique_lock( *this );
		ImageSPtr result;

		if ( Collection< Image, String >::has( p_name ) )
		{
			result = Collection< Image, String >::find( p_name );

			if ( !result->GetBuffer() )
			{
				*result = std::move( Image( p_name, p_size, p_format ) );
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Image: " ) << p_name );
			}
		}
		else
		{
			result = std::make_shared< Image >( p_name, p_size, p_format );
			Collection< Image, String >::insert( p_name, result );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Image: " ) << p_name );
		}

		return result;
	}
}
