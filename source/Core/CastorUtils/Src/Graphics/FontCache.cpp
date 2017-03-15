#include "FontCache.hpp"

#include "Font.hpp"
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

	FontCache::FontCache()
	{
	}

	FontCache::~FontCache()
	{
	}

	FontSPtr FontCache::Add( String const & p_name, uint32_t p_height, Path const & p_path )
	{
		auto l_lock = make_unique_lock( *this );
		FontSPtr l_result;

		if ( Collection< Font, String >::has( p_name ) )
		{
			l_result = Collection< Font, String >::find( p_name );
			Logger::LogWarning( StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << cuT( "Font: " ) << p_name );
		}
		else
		{
			String l_name = p_path.GetFileName() + cuT( "." ) + p_path.GetExtension();

			if ( File::FileExists( p_path ) )
			{
				l_result = std::make_shared< Font >( p_name, p_height, p_path );
				Logger::LogInfo( StringStream() << INFO_CACHE_CREATED_OBJECT << cuT( "Font: " ) << p_name );
				Collection< Font, String >::insert( p_name, l_result );

				if ( m_paths.find( l_name ) == m_paths.end() )
				{
					m_paths.insert( std::make_pair( l_name, p_path ) );
				}
			}
			else
			{
				auto l_it = m_paths.find( l_name );

				if ( l_it != m_paths.end() )
				{
					l_result = std::make_shared< Font >( p_name, p_height, l_it->second );
					Collection< Font, String >::insert( p_name, l_result );
				}
				else
				{
					CASTOR_EXCEPTION( "Can't create the font, invalid name: " + string::string_cast< char >( p_name ) + ", path: " + string::string_cast< char >( p_path ) );
				}
			}
		}

		return l_result;
	}

	FontSPtr FontCache::Find( String const & p_name )
	{
		return Collection< Font, String >::find( p_name );
	}

	void FontCache::Remove( String const & p_name )
	{
		Collection< Font, String >::erase( p_name );
	}

	void FontCache::Clear()
	{
		Collection< Font, String >::lock();
		Collection< Font, String >::clear();
		m_paths.clear();
		Collection< Font, String >::unlock();
	}
}
