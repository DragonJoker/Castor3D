#include "FontManager.hpp"

#include "Font.hpp"
#include "Logger.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace Castor
{
	namespace
	{
		static const xchar * INFO_MANAGER_CREATED_OBJECT = cuT( "Manager::Create - Created " );
		static const xchar * WARNING_MANAGER_DUPLICATE_OBJECT = cuT( "Manager::Create - Duplicate " );
		static const xchar * WARNING_MANAGER_NULL_OBJECT = cuT( "Manager::Insert - NULL " );
	}

	FontManager::BinaryLoader::BinaryLoader()
		: Loader< FontManager, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool FontManager::BinaryLoader::operator()( FontManager & p_manager, BinaryFile & p_file )
	{
		return true;
	}

	//*********************************************************************************************

	FontManager::FontManager()
	{
	}

	FontManager::~FontManager()
	{
	}

	FontSPtr FontManager::create( Castor::Path const & p_path, Castor::String const & p_name, uint32_t p_height )
	{
		auto l_lock = make_unique_lock( *this );
		FontSPtr l_return;

		if ( Collection< Font, String >::has( p_name ) )
		{
			l_return = Collection< Font, String >::find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << cuT( "Font: " ) << p_name );
		}
		else
		{
			String l_name = p_path.GetFileName() + cuT( "." ) + p_path.GetExtension();

			if ( File::FileExists( p_path ) )
			{
				l_return = std::make_shared< Font >( p_path, p_name, p_height );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << cuT( "Font: " ) << p_name );
				Collection< Font, String >::insert( p_name, l_return );

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
					l_return = std::make_shared< Font >( l_it->second, p_name, p_height );
					Collection< Font, String >::insert( p_name, l_return );
				}
				else
				{
					CASTOR_EXCEPTION( "Can't create the font, invalid name: " + string::string_cast< char >( p_name ) + ", path: " + string::string_cast< char >( p_path ) );
				}
			}
		}

		return l_return;
	}

	FontSPtr FontManager::get( Castor::String const & p_name )
	{
		return Collection< Font, String >::find( p_name );
	}

	void FontManager::clear()
	{
		Collection< Font, String >::lock();
		Collection< Font, String >::clear();
		m_paths.clear();
		Collection< Font, String >::unlock();
	}
}
