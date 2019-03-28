#include "CastorUtils/FileParser/FileParserContext.hpp"

namespace castor
{
	FileParserContext::FileParserContext( Path const & path )
		: m_file{ path }
		, m_line{ 0 }
	{
	}

	FileParserContext::~FileParserContext()
	{
	}

	void FileParserContext::registerUserContext( String const & name, void * data )
	{
		if ( m_userContexts.find( name ) != m_userContexts.end() )
		{
			CU_Exception( "A user context with name [" + string::stringCast< char >( name ) + "] already exists." );
		}

		m_userContexts.insert( std::make_pair( name, data ) );
	}

	void * FileParserContext::unregisterUserContext( String const & name )
	{
		auto it = m_userContexts.find( name );

		if ( it == m_userContexts.end() )
		{
			CU_Exception( "No user context with name [" + string::stringCast< char >( name ) + "]." );
		}

		void * result = it->second;
		m_userContexts.erase( it );
		return result;
	}

	void * FileParserContext::getUserContext( String const & name )
	{
		auto it = m_userContexts.find( name );

		if ( it == m_userContexts.end() )
		{
			CU_Exception( "No user context with name [" + string::stringCast< char >( name ) + "]." );
		}

		return it->second;
	}
}
