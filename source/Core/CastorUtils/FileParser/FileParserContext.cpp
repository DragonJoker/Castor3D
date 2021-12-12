#include "CastorUtils/FileParser/FileParserContext.hpp"

namespace castor
{
	FileParserContext::FileParserContext( LoggerInstance & logger
		, Path const & path )
		: file{ path }
		, logger{ &logger }
	{
	}

	void FileParserContext::registerUserContext( String const & name, void * data )
	{
		if ( userContexts.find( name ) != userContexts.end() )
		{
			CU_Exception( "A user context with name [" + string::stringCast< char >( name ) + "] already exists." );
		}

		userContexts.insert( std::make_pair( name, data ) );
	}

	void * FileParserContext::unregisterUserContext( String const & name )
	{
		auto it = userContexts.find( name );

		if ( it == userContexts.end() )
		{
			CU_Exception( "No user context with name [" + string::stringCast< char >( name ) + "]." );
		}

		void * result = it->second;
		userContexts.erase( it );
		return result;
	}

	bool FileParserContext::hasUserContext( String const & name )
	{
		return userContexts.find( name ) != userContexts.end();
	}

	void * FileParserContext::getUserContext( String const & name )
	{
		auto it = userContexts.find( name );

		if ( it == userContexts.end() )
		{
			CU_Exception( "No user context with name [" + string::stringCast< char >( name ) + "]." );
		}

		return it->second;
	}
}
