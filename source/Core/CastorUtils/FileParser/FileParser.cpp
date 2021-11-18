#include "CastorUtils/FileParser/FileParser.hpp"

#include "CastorUtils/FileParser/ParserParameter.hpp"

namespace castor
{
	namespace
	{
		String doStripComments( String const & p_line )
		{
			String result = p_line;
			auto index = result.find( "//" );

			if ( index != String::npos )
			{
				result = result.substr( 0, index );
			}

			index = result.find( "/*" );

			if ( index != String::npos )
			{
				result = result.substr( index, result.find( "*/", index ) - index );
			}

			return result;
		}
	}

	//*********************************************************************************************

	PreprocessedFile::PreprocessedFile( FileParser & parser )
		: m_parser{ parser }
	{
	}

	PreprocessedFile::PreprocessedFile( FileParser & parser
		, FileParserContextSPtr context )
		: m_parser{ parser }
		, m_context{ std::move( context ) }
	{
		m_context->preprocessed = this;
	}

	void PreprocessedFile::addParser( Path file
		, uint64_t line
		, String name
		, SectionAttributeParsers functions
		, String params )
	{
		if ( name == "}" )
		{
			m_popAction = { file
				, line
				, name
				, functions
				, params };
		}

		m_actions.push_back( { std::move( file )
			, line
			, std::move( name )
			, std::move( functions )
			, std::move( params ) } );
	}

	bool PreprocessedFile::parse()
	{
		CU_Require( m_context );
		bool result = !m_actions.empty();
		m_context->pendingSection = 0u;
		m_context->sections.clear();
		m_context->sections.push_back( m_parser.getRootSectionId() );
		bool isNextOpenBrace = false;
		uint32_t ignoreActionSignal = 0u;
		auto it = m_actions.begin();
		std::vector< Action > jobs;
		jobs.push_back( *it );

		while ( !jobs.empty() )
		{
			auto action = *jobs.begin();
			jobs.erase( jobs.begin() );

			if ( !ignoreActionSignal )
			{
				onAction( action );
			}
			else
			{
				--ignoreActionSignal;
			}

			m_context->line = action.line;
			m_context->functionName = action.name;

			if ( !m_context->sections.empty() )
			{
				auto section = m_context->sections.back();
				auto funcsIt = action.functions.find( section );

				if ( funcsIt == action.functions.end() )
				{
					if ( action.name == "}" )
					{
						m_context->sections.pop_back();
					}
					else if ( action.name == "{" )
					{
						m_context->sections.push_back( m_context->pendingSection );
						m_context->pendingSection = 0u;
						isNextOpenBrace = false;
					}
					else
					{
						parseError( "Section " + m_parser.getSectionName( section ) + " not found for directive [" + action.name + "]" );
					}
				}
				else if ( isNextOpenBrace && action.name != "{" )
				{
					// Optional block not filled, emulate block begin and block end.
					m_context->sections.push_back( m_context->pendingSection );
					m_context->pendingSection = 0u;
					isNextOpenBrace = false;
					ignoreActionSignal = 2u;
					jobs.push_back( m_popAction );
					jobs.push_back( action );
				}
				else
				{
					ParserParameterArray filled;
					doCheckParams( action.params
						, funcsIt->second.params
						, filled );

					try
					{
						m_parser.getLogger().logTrace( action.file + cuT( ":" ) + string::toString( action.line ) + cuT( " (" ) + action.name + cuT( ")" ) );
						isNextOpenBrace = funcsIt->second.function( *m_context, filled );
					}
					catch ( Exception & exc )
					{
						parseError( exc.getFullDescription() );
					}
				}
			}
			else
			{
				parseError( "Unexpected directive [" + action.name + "]" );
			}

			if ( jobs.empty() )
			{
				++it;

				if ( it != m_actions.end() )
				{
					jobs.push_back( *it );
				}
			}
		}

		if ( m_context->sections.empty() || m_context->sections.back() != m_parser.getRootSectionId() )
		{
			if ( m_context.use_count() == 1 )
			{
				parseError( cuT( "Unexpected end of file" ) );
			}
			else
			{
				m_parser.validate( *this );
			}
		}
		else
		{
			m_parser.validate( *this );
		}

		m_parser.cleanupParser( *this );
		return result;
	}

	void PreprocessedFile::parseError( String const & text )
	{
		m_parser.parseError( doGetSectionsStack(), m_context->line, text );
	}

	void PreprocessedFile::parseWarning( String const & text )
	{
		m_parser.parseError( doGetSectionsStack(), m_context->line, text );
	}

	bool PreprocessedFile::doCheckParams( String params
		, ParserParameterArray const & expected
		, ParserParameterArray & received )
	{
		bool result = true;
		string::trim( params );
		String missingParam;

		for ( auto param : expected )
		{
			if ( result )
			{
				auto filled = param->clone();
				result = filled->parse( m_parser.getLogger(), params );

				if ( !result )
				{
					missingParam = param->getStrType();
				}
				else
				{
					received.push_back( filled );
				}
			}
		}

		if ( !params.empty() )
		{
			auto param = std::make_shared< ParserParameter< ParameterType::eText > >();
			param->m_value = params;
			received.push_back( param );
		}

		if ( !result )
		{
			parseError( cuT( "Directive <" ) + m_context->functionName + cuT( "> needs a <" ) + missingParam + cuT( "> parameter that is currently missing" ) );
		}

		return result;
	}

	String PreprocessedFile::doGetSectionsStack()
	{
		StringStream sections{ makeStringStream() };

		if ( m_context && m_context->sections.size() > 1 )
		{
			auto begin = m_context->sections.begin() + 1;
			sections << m_parser.getSectionName( *begin );

			std::for_each( begin + 1, m_context->sections.end(), [&sections, this]( uint32_t p_section )
				{
					sections << cuT( "::" ) << m_parser.getSectionName( p_section );
				} );
		}

		if ( !m_context->functionName.empty() )
		{
			sections << cuT( "::" ) << m_context->functionName;
		}

		return sections.str();
	}

	//*********************************************************************************************

	FileParser::FileParser( SectionId rootSectionId )
		: FileParser{ *Logger::getSingleton().getInstance(), rootSectionId }
	{
	}

	FileParser::FileParser( LoggerInstance & logger
		, SectionId rootSectionId )
		: m_rootSectionId( rootSectionId )
		, m_logger{ logger }
	{
	}

	FileParser::~FileParser()
	{
	}

	void FileParser::processFile( Path const & path
		, PreprocessedFile & preprocessed )
	{
		m_ignoreLevel = 0;
		m_ignored = false;

		if ( TextFile file{ path, File::OpenMode::eRead } )
		{
			String content;
			file.copyToString( content );

			if ( !content.empty() )
			{
				m_logger.logInfo( cuT( "FileParser : Parsing file [" ) + path.getFileName( true ) + cuT( "]." ) );
				processFile( path, content, preprocessed );
				m_logger.logInfo( cuT( "FileParser : Finished parsing file [" ) + path.getFileName( true ) + cuT( "]." ) );
			}
		}
		else
		{
			m_logger.logError( cuT( "FileParser : Couldn't open file [" ) + path.getFileName( true ) + cuT( "]." ) );
		}
	}

	void FileParser::processFile( Path const & path
		, String const & content
		, PreprocessedFile & preprocessed )
	{
		m_path = path.getPath();
		m_fileName = path.getFileName( true );
		m_preprocessed = &preprocessed;
		bool isNextOpenBrace = false;
		bool bCommented = false;
		auto lineIndex = 0ull;

		bool bReuse = false;
		String strLine;
		String strLine2;
		String file = content;
		string::replace( file, cuT( "\r\n" ), cuT( "\n" ) );
		StringArray lines = string::split( file, cuT( "\n" ), uint32_t( std::count( file.begin(), file.end(), '\n' ) + 1 ), true );
		auto it = lines.begin();

		while ( it != lines.end() )
		{
			if ( !bReuse )
			{
				strLine = *it++;
				lineIndex++;
			}
			else
			{
				bReuse = false;
			}

			//m_logger.logDebug( string::toString( m_context->m_line ) + cuT( " - " ) + strLine.c_str() );
			string::trim( strLine );

			if ( !strLine.empty() )
			{
				if ( strLine.size() < 2 || strLine.substr( 0, 2 ) != cuT( "//" ) )
				{
					if ( !bCommented )
					{
						std::size_t uiCommentBegin = strLine.find( cuT( "/*" ) );

						if ( uiCommentBegin != String::npos )
						{
							std::size_t uiCommentEnd = strLine.find( cuT( "*/" ) );

							if ( uiCommentEnd < strLine.size() - 3 )
							{
								if ( uiCommentBegin > 0 )
								{
									strLine = strLine.substr( 0, uiCommentBegin ) + strLine.substr( uiCommentEnd + 2 );
								}
								else
								{
									strLine = strLine.substr( uiCommentEnd + 2 );
								}
							}
							else if ( uiCommentEnd != String::npos )
							{
								if ( uiCommentBegin > 0 )
								{
									strLine = strLine.substr( 0, uiCommentBegin );
								}
								else
								{
									strLine.clear();
								}
							}
							else if ( uiCommentBegin > 0 )
							{
								strLine = strLine.substr( 0, uiCommentBegin );
								bCommented = true;
							}
							else
							{
								bCommented = true;
							}

							if ( !strLine.empty() && !bCommented )
							{
								bReuse = true;
							}
						}
						else
						{
							if ( strLine.find( cuT( "{" ) ) == strLine.size() - 1 && strLine.size() > 1 )
							{
								// We got a "{" at the end of the line, so we split the line in two and reuse the line
								strLine2 = strLine.substr( 0, strLine.size() - 1 );
								string::trim( strLine2 );
								isNextOpenBrace = doParseScriptLine( preprocessed, strLine2, lineIndex );
								strLine = cuT( "{" );
								bReuse = true;
							}
							else
							{
								if ( isNextOpenBrace )
								{
									if ( strLine != cuT( "{" ) )
									{
										isNextOpenBrace = doParseScriptBlockEnd( preprocessed, lineIndex );
										bReuse = true;
									}
									else
									{
										isNextOpenBrace = false;
										doEnterBlock( preprocessed, lineIndex );
									}
								}
								else if ( strLine == cuT( "{" ) )
								{
									doEnterBlock( preprocessed, lineIndex );
								}
								else
								{
									isNextOpenBrace = doParseScriptLine( preprocessed, strLine, lineIndex );
								}

								if ( m_ignoreLevel > 0 )
								{
									if ( !strLine.empty() && strLine.find( cuT( "}" ) ) == strLine.size() - 1 )
									{
										doLeaveBlock( preprocessed, lineIndex );
									}
								}
							}
						}
					}
					else
					{
						std::size_t uiCommentEnd = strLine.find( cuT( "*/" ) );

						if ( uiCommentEnd < strLine.size() - 3 )
						{
							strLine = strLine.substr( uiCommentEnd + 2 );
							bCommented = false;
							bReuse = true;
						}
						else if ( uiCommentEnd != String::npos )
						{
							strLine.clear();
							bCommented = false;
						}
					}
				}
			}
		}

		m_preprocessed = nullptr;
	}

	PreprocessedFile FileParser::processFile( Path const & path )
	{
		PreprocessedFile result{ *this
			, doInitialiseParser( path ) };
		processFile( path, result );
		return result;
	}

	PreprocessedFile FileParser::processFile( Path const & path
		, String const & content )
	{
		PreprocessedFile result{ *this
			, doInitialiseParser( path ) };
		processFile( path, content, result );
		return result;
	}

	bool FileParser::parseFile( Path const & path )
	{
		auto preprocessed = processFile( path );
		return preprocessed.parse();
	}

	bool FileParser::parseFile( Path const & path
		, String const & content )
	{
		auto preprocessed = processFile( path, content );
		return preprocessed.parse();
	}

	void FileParser::parseError( String const & functionName
		, uint64_t lineIndex
		, String const & text )
	{
		m_logger.logError( makeStringStream()
			<< cuT( "Error, line #" ) << lineIndex
			<< cuT( ": Directive <" ) << functionName
			<< cuT( ">: " ) << text );
	}

	void FileParser::parseWarning( String const & functionName
		, uint64_t lineIndex
		, String const & text )
	{
		m_logger.logWarning( makeStringStream()
			<< cuT( "Warning, line #" ) << lineIndex
			<< cuT( ": Directive <" ) << functionName
			<< cuT( ">: " ) << text );
	}

	void FileParser::addParser( SectionId section
		, String const & name
		, ParserFunction function
		, ParserParameterArray params )
	{
		auto & parsers = m_parsers.insert( { name, {} } ).first->second;
		auto ires = parsers.insert( { section, { function, params } } );

		if ( !ires.second )
		{
			m_logger.logError( makeStringStream()
				<< cuT( "Parser " ) << name
				<< cuT( " for section " ) << doGetSectionName( section )
				<< cuT( " (" ) << section
				<< cuT( ") already exists." ) );
		}
	}

	bool FileParser::doParseScriptLine( PreprocessedFile & preprocessed
		, String & line
		, uint64_t lineIndex )
	{
		bool carryOn = true;
		bool result = false;
		std::size_t blockEndIndex = line.find( cuT( "}" ) );
		line = doStripComments( line );

		if ( blockEndIndex != String::npos )
		{
			m_functionName = cuT( "}" );

			if ( blockEndIndex == 0 )
			{
				// Block end at the beginning of the line, we treat it then we parse the line
				line = line.substr( 1 );
				string::trim( line );
				doLeaveBlock( preprocessed, lineIndex );

				if ( !line.empty() )
				{
					result = doParseScriptLine( preprocessed, line, lineIndex );
				}
				else
				{
					result = false;
				}

				carryOn = false;
			}
			else if ( blockEndIndex == line.size() - 1 )
			{
				// Block end at the end of the line : we treat the line then the block end
				line = line.substr( 0, blockEndIndex );
				string::trim( line );

				if ( !line.empty() )
				{
					doParseScriptLine( preprocessed, line, lineIndex );
				}

				doLeaveBlock( preprocessed, lineIndex );
				result = false;
				carryOn = false;
			}
			else
			{
				// Block end in the middle of the line, we don't treat it, we parse the line
				carryOn = true;
			}
		}
		else
		{
			// No block end, we parse the line
			carryOn = true;
		}

		if ( carryOn )
		{
			result = doInvokeParser( preprocessed, line, lineIndex );
		}

		return result;
	}

	bool FileParser::doDiscardParser( PreprocessedFile & preprocessed
		, String const & value )
	{
		auto & context = preprocessed.getContext();
		Logger::logWarning( makeStringStream()
			<< cuT( "Parser not found @ line " ) << context.line
			<< cuT( " : " ) + value );
		return false;
	}

	void FileParser::doParseScriptBlockBegin( PreprocessedFile & preprocessed
		, uint64_t lineIndex )
	{
		auto defaultPush = []( FileParserContext & context
			, ParserParameterArray const & )
		{
			context.sections.push_back( context.pendingSection );
			return false;
		};
		preprocessed.addParser( m_path / m_fileName
			, lineIndex
			, "{"
			, { { 0u, ParserFunctionAndParams{ defaultPush, {} } } }
			, {} );
	}

	bool FileParser::doParseScriptBlockEnd( PreprocessedFile & preprocessed
		, uint64_t lineIndex )
	{
		bool result = false;
		auto it = m_parsers.find( "}" );

		if ( it != m_parsers.end() )
		{
			preprocessed.addParser( m_path / m_fileName
				, lineIndex
				, "}"
				, it->second
				, {} );
			result = true;
		}
		else
		{
			auto defaultPop = []( FileParserContext & context
				, ParserParameterArray const & )
			{
				context.sections.pop_back();
				return false;
			};
			preprocessed.addParser( m_path / m_fileName
				, lineIndex
				, "}"
				, { { 0u, ParserFunctionAndParams{ defaultPop, {} } } }
				, {} );
			result = false;
		}

		return result;
	}

	bool FileParser::doInvokeParser( PreprocessedFile & preprocessed
		, String & line
		, uint64_t lineIndex )
	{
		bool result = false;

		if ( !doIsInIgnoredBlock() )
		{
			StringArray splitCmd = string::split( line, cuT( " \t" ), 1, false );
			auto functionName = splitCmd[0];
			auto iter = m_parsers.find( functionName );

			if ( iter == m_parsers.end() )
			{
				if ( functionName == cuT( "define" ) && splitCmd.size() > 1 )
				{
					doAddDefine( lineIndex, splitCmd[1] );
				}
				else if ( functionName == cuT( "include" ) && splitCmd.size() > 1 )
				{
					doIncludeFile( preprocessed, lineIndex, splitCmd[1] );
				}
				else if ( !doDiscardParser( preprocessed, line ) )
				{
					ignore();
				}
			}
			else
			{
				String parameters;

				if ( splitCmd.size() > 1u )
				{
					parameters = string::trim( splitCmd[1] );
					doCheckDefines( parameters );
				}

				preprocessed.addParser( m_path / m_fileName
					, lineIndex
					, functionName
					, iter->second
					, parameters );
			}
		}

		return result;
	}

	void FileParser::doEnterBlock( PreprocessedFile & preprocessed
		, uint64_t lineIndex )
	{
		if ( m_ignored )
		{
			m_ignoreLevel++;
		}
		else
		{
			doParseScriptBlockBegin( preprocessed, lineIndex );
		}
	}

	void FileParser::doLeaveBlock( PreprocessedFile & preprocessed
		, uint64_t lineIndex )
	{
		if ( doIsInIgnoredBlock() )
		{
			m_ignoreLevel--;

			if ( m_ignoreLevel <= 0 )
			{
				m_ignored = false;
				m_ignoreLevel = 0;
			}
		}
		else
		{
			m_ignored = false;
			doParseScriptBlockEnd( preprocessed, lineIndex );
		}
	}

	bool FileParser::doIsInIgnoredBlock()
	{
		return m_ignored && m_ignoreLevel > 0;
	}

	void FileParser::doIncludeFile( PreprocessedFile & preprocessed
		, uint64_t CU_UnusedParam( lineIndex )
		, String const & param )
	{
		String params = param;
		Path path;
		if ( ValueParser< ParameterType::ePath >::parse( m_logger
			, params
			, path ) )
		{
			auto subparser = doCreateParser();
			subparser->doInitialiseParser( m_path / path );
			subparser->processFile( m_path / path, preprocessed );
		}
	}

	void FileParser::doAddDefine( uint64_t lineIndex
		, String const & param )
	{
		auto functionName = "define";
		auto values = string::split( param, cuT( " \t" ), 1 );

		if ( values.size() == 2 )
		{
			auto iresult = m_defines.emplace( values[0], values[1] );

			if ( !iresult.second )
			{
				parseError( functionName
					, lineIndex
					, cuT( "Replacing an already existing value." ) );
			}
		}
		else
		{
			parseWarning( functionName
				, lineIndex
				, cuT( "Missing parameters." ) );
		}
	}

	void FileParser::doCheckDefines( String & text )
	{
		static String const separators = cuT( " \t,;:?./-*+=(){}[]|" );
		auto textLength = text.size();

		for ( auto & define : m_defines )
		{
			auto index = String::npos;
			auto wordLength = define.first.size();

			do
			{
				index = text.find( define.first, index + 1 );

				if ( index != String::npos )
				{
					bool replace = true;

					if ( index > 0 )
					{
						// Word doesn't start the line.
						if ( separators.find( text[index - 1] ) == String::npos )
						{
							// Previous char isn't separator => ignore.
							replace = false;
						}
					}

					if ( replace )
					{
						if ( index + textLength > wordLength )
						{
							// Word doesn't end the line.
							if ( separators.find( text[index + wordLength] ) == String::npos )
							{
								// Next char isn't separator => ignore.
								replace = false;
							}
						}
					}

					if ( replace )
					{
						text = text.replace( index, define.first.size(), define.second );
						textLength = text.size();
					}
				}
			}
			while ( index != String::npos );
		}
	}
}
