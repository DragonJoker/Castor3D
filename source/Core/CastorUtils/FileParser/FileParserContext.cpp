#include "CastorUtils/FileParser/FileParserContext.hpp"

#include "CastorUtils/FileParser/FileParser.hpp"

namespace castor
{
	FileParserContext::FileParserContext( FileParser & pparser
		, Path const & ppath )
		: file{ ppath }
		, parser{ &pparser }
		, logger{ &parser->getLogger() }
	{
		for ( auto & parsers : parser->getAdditionalParsers() )
		{
			if ( parsers.second.contextCreator )
			{
				registerUserContext( parsers.first
					, parsers.second.contextCreator( *this ) );
			}
		}
	}

	void FileParserContext::registerUserContext( String const & name, void * data )
	{
		auto ires = userContexts.insert( std::make_pair( name, data ) );

		if ( !ires.second )
		{
			CU_Exception( "A user context with name [" + string::stringCast< char >( name ) + "] already exists." );
		}
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
