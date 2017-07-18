#include "FileParserContext.hpp"

namespace Castor
{
	FileParserContext::FileParserContext( TextFile * p_pFile )
		: m_file( p_pFile )
		, m_line( 0 )
	{
	}

	FileParserContext::~FileParserContext()
	{
	}

	void FileParserContext::RegisterUserContext( String const & p_name, void * p_data )
	{
		if ( m_userContexts.find( p_name ) != m_userContexts.end() )
		{
			CASTOR_EXCEPTION( "A user context with name [" + string::string_cast< char >( p_name ) + "] already exists." );
		}

		m_userContexts.insert( std::make_pair( p_name, p_data ) );
	}

	void * FileParserContext::UnregisterUserContext( String const & p_name )
	{
		auto it = m_userContexts.find( p_name );

		if ( it == m_userContexts.end() )
		{
			CASTOR_EXCEPTION( "No user context with name [" + string::string_cast< char >( p_name ) + "]." );
		}

		void * result = it->second;
		m_userContexts.erase( it );
		return result;
	}

	void * FileParserContext::GetUserContext( String const & p_name )
	{
		auto it = m_userContexts.find( p_name );

		if ( it == m_userContexts.end() )
		{
			CASTOR_EXCEPTION( "No user context with name [" + string::string_cast< char >( p_name ) + "]." );
		}

		return it->second;
	}
}
