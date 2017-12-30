#include "FontCache.hpp"

#include "Font.hpp"
#include "Log/Logger.hpp"

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

	FontCache::FontCache()
	{
	}

	FontCache::~FontCache()
	{
	}

	FontSPtr FontCache::create( String const & p_name, uint32_t p_height, Path const & p_path )
	{
		String name = p_path.getFileName() + cuT( "." ) + p_path.getExtension();
		FontSPtr result;

		if ( File::fileExists( p_path ) )
		{
			result = std::make_shared< Font >( p_name, p_height, p_path );
		}
		else
		{
			auto it = m_paths.find( name );

			if ( it != m_paths.end() )
			{
				result = std::make_shared< Font >( p_name, p_height, it->second );
			}
			else
			{
				CASTOR_EXCEPTION( "Can't create the font, invalid name: " + string::stringCast< char >( p_name ) + ", path: " + string::stringCast< char >( p_path ) );
			}
		}

		return result;
	}

	FontSPtr FontCache::add( String const & p_name, uint32_t p_height, Path const & p_path )
	{
		auto lock = makeUniqueLock( *this );
		FontSPtr result;

		if ( Collection< Font, String >::has( p_name ) )
		{
			result = Collection< Font, String >::find( p_name );
			Logger::logWarning( StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Font: " ) << p_name );
		}
		else
		{
			String name = p_path.getFileName() + cuT( "." ) + p_path.getExtension();

			if ( File::fileExists( p_path ) )
			{
				result = std::make_shared< Font >( p_name, p_height, p_path );
				Logger::logDebug( StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Font: " ) << p_name );
				Collection< Font, String >::insert( p_name, result );

				if ( m_paths.find( name ) == m_paths.end() )
				{
					m_paths.insert( std::make_pair( name, p_path ) );
				}
			}
			else
			{
				auto it = m_paths.find( name );

				if ( it != m_paths.end() )
				{
					result = std::make_shared< Font >( p_name, p_height, it->second );
					Collection< Font, String >::insert( p_name, result );
				}
				else
				{
					CASTOR_EXCEPTION( "Can't create the font, invalid name: " + string::stringCast< char >( p_name ) + ", path: " + string::stringCast< char >( p_path ) );
				}
			}
		}

		return result;
	}

	FontSPtr FontCache::add( castor::String const & p_name, FontSPtr p_font )
	{
		auto lock = makeUniqueLock( *this );
		FontSPtr result{ p_font };

		if ( Collection< Font, String >::has( p_name ) )
		{
			result = Collection< Font, String >::find( p_name );
			Logger::logWarning( StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Font: " ) << p_name );
		}
		else
		{
			auto path = p_font->getFilePath();
			String name = path.getFileName() + cuT( "." ) + path.getExtension();
			Collection< Font, String >::insert( p_name, result );

			if ( m_paths.find( name ) == m_paths.end() )
			{
				m_paths.insert( std::make_pair( name, path ) );
			}
		}

		return result;
	}

	FontSPtr FontCache::find( String const & p_name )
	{
		return Collection< Font, String >::find( p_name );
	}

	bool FontCache::has( String const & p_name )
	{
		return Collection< Font, String >::has( p_name );
	}

	void FontCache::remove( String const & p_name )
	{
		Collection< Font, String >::erase( p_name );
	}

	void FontCache::clear()
	{
		Collection< Font, String >::lock();
		Collection< Font, String >::clear();
		m_paths.clear();
		Collection< Font, String >::unlock();
	}
}
