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

	FileParser::FileParser( uint32_t rootSectionId )
		: FileParser{ *Logger::getSingleton().getInstance(), rootSectionId }
	{
	}

	FileParser::FileParser( LoggerInstance & logger
		, uint32_t rootSectionId )
		: m_logger{ logger }
		, m_rootSectionId( rootSectionId )
		, m_context()
		, m_ignoreLevel( 0 )
		, m_ignored( false )
	{
	}

	FileParser::~FileParser()
	{
	}

	bool FileParser::parseFile( Path const & path )
	{
		m_ignoreLevel = 0;
		m_ignored = false;
		String content;
		bool result = false;

		{
			TextFile file( path, File::OpenMode::eRead );
			result = file.isOk();

			if ( result )
			{
				file.copyToString( content );
			}
		}

		if ( result )
		{
			m_logger.logInfo( cuT( "FileParser : Parsing file [" ) + path.getFileName( true ) + cuT( "]." ) );
			result = parseFile( path, content );
			m_logger.logInfo( cuT( "FileParser : Finished parsing file [" ) + path.getFileName( true ) + cuT( "]." ) );
		}
		else
		{
			m_logger.logError( cuT( "FileParser : Couldn't parse file [" ) + path.getFileName( true ) + cuT( "], file does not exist." ) );
		}

		return result;
	}

	bool FileParser::parseFile( Path const & path
		, String const & content )
	{
		bool result = false;
		bool bNextIsOpenBrace = false;
		bool bCommented = false;
		doInitialiseParser( path );
		auto save = 0ull;

		if ( m_context->m_sections.empty() )
		{
			m_context->m_sections.push_back( m_rootSectionId );
		}

		std::swap( m_context->m_line, save );
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
				m_context->m_line++;
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
								bNextIsOpenBrace = doParseScriptLine( strLine2 );
								strLine = cuT( "{" );
								bReuse = true;
							}
							else
							{
								if ( bNextIsOpenBrace )
								{
									if ( strLine != cuT( "{" ) )
									{
										bNextIsOpenBrace = doParseScriptBlockEnd();
										bReuse = true;
									}
									else
									{
										bNextIsOpenBrace = false;
										doEnterBlock();
									}
								}
								else if ( strLine == cuT( "{" ) )
								{
									doEnterBlock();
								}
								else
								{
									bNextIsOpenBrace = doParseScriptLine( strLine );
								}

								if ( m_ignoreLevel > 0 )
								{
									if ( !strLine.empty() && strLine.find( cuT( "}" ) ) == strLine.size() - 1 )
									{
										doLeaveBlock();
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

		if ( m_context->m_sections.empty() || m_context->m_sections.back() != m_rootSectionId )
		{
			if ( m_context.use_count() == 1 )
			{
				parseError( cuT( "Unexpected end of file" ) );
			}
			else
			{
				doValidate();
				result = true;
			}
		}
		else
		{
			doValidate();
			result = true;
		}

		std::swap( m_context->m_line, save );
		doCleanupParser();
		return result;
	}

	void FileParser::parseError( String const & text )
	{
		StringStream stream{ makeStringStream() };
		stream << cuT( "Error, line #" ) << m_context->m_line << cuT( ": Directive <" ) << doGetSectionsStack() << cuT( ">: " ) << text;
		m_logger.logError( stream.str() );
	}

	void FileParser::parseWarning( String const & text )
	{
		StringStream stream{ makeStringStream() };
		stream << cuT( "Warning, line #" ) << m_context->m_line << cuT( ": Directive <" ) << doGetSectionsStack() << cuT( ">: " ) << text;
		m_logger.logWarning( stream.str() );
	}

	bool FileParser::checkParams( String params
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
				result = filled->parse( m_logger, params );

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
			parseError( cuT( "Directive <" ) + m_context->m_functionName + cuT( "> needs a <" ) + missingParam + cuT( "> parameter that is currently missing" ) );
		}

		return result;
	}

	void FileParser::addParser( uint32_t p_section, String const & p_name, ParserFunction p_function, ParserParameterArray && p_params )
	{
		auto sectionIt = m_parsers.find( p_section );

		if ( sectionIt != m_parsers.end() && sectionIt->second.find( p_name ) != sectionIt->second.end() )
		{
			parseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::toString( p_section ) + cuT( " already exists." ) );
		}
		else
		{
			m_parsers[p_section][p_name] = { p_function, p_params };
		}
	}

	bool FileParser::doParseScriptLine( String & p_line )
	{
		bool bContinue = true;
		bool result = false;
		std::size_t uiBlockEndIndex = p_line.find( cuT( "}" ) );
		p_line = doStripComments( p_line );

		if ( uiBlockEndIndex != String::npos )
		{
			m_context->m_functionName = cuT( "}" );

			if ( uiBlockEndIndex == 0 )
			{
				// Block end at the beginning of the line, we treat it then we parse the line
				p_line = p_line.substr( 1 );
				string::trim( p_line );
				doLeaveBlock();

				if ( !p_line.empty() )
				{
					result = doParseScriptLine( p_line );
				}
				else
				{
					result = false;
				}

				bContinue = false;
			}
			else if ( uiBlockEndIndex == p_line.size() - 1 )
			{
				// Block end at the end of the line : we treat the line then the block end
				p_line = p_line.substr( 0, uiBlockEndIndex );
				string::trim( p_line );

				if ( !p_line.empty() )
				{
					doParseScriptLine( p_line );
				}

				doLeaveBlock();
				result = false;
				bContinue = false;
			}
			else
			{
				// Block end in the middle of the line, we don't treat it, we parse the line
				bContinue = true;
			}
		}
		else
		{
			// No block end, we parse the line
			bContinue = true;
		}

		if ( bContinue )
		{
			if ( !m_context->m_sections.empty() )
			{
				result = doInvokeParser( p_line, m_parsers[m_context->m_sections.back()] );
			}
			else
			{
				result = doDelegateParser( p_line );
			}
		}

		return result;
	}

	bool FileParser::doParseScriptBlockEnd()
	{
		bool result = false;

		if ( !m_context->m_sections.empty() )
		{
			AttributeParserMap::const_iterator const & iter = m_parsers[m_context->m_sections.back()].find( cuT( "}" ) );

			if ( iter == m_parsers[m_context->m_sections.back()].end() )
			{
				m_context->m_sections.pop_back();
				result = false;
			}
			else
			{
				result = iter->second.m_function( this, iter->second.m_params );
			}
		}
		else
		{
			parseError( "Unexpected '}'" );
		}

		return result;
	}

	bool FileParser::doInvokeParser( String & p_line, AttributeParserMap const & p_parsers )
	{
		bool result = false;
		StringArray splitCmd = string::split( p_line, cuT( " \t" ), 1, false );
		m_context->m_functionName = splitCmd[0];
		AttributeParserMap::const_iterator const & iter = p_parsers.find( splitCmd[0] );

		if ( !doIsInIgnoredBlock() )
		{
			if ( iter == p_parsers.end() )
			{
				if ( splitCmd[0] == cuT( "define" ) && splitCmd.size() > 1 )
				{
					doAddDefine( splitCmd[1] );
				}
				else if ( !doDiscardParser( p_line ) )
				{
					ignore();
				}
			}
			else
			{
				String strParameters;

				if ( splitCmd.size() >= 2 )
				{
					strParameters = string::trim( splitCmd[1] );
					doCheckDefines( strParameters );
				}

				ParserParameterArray filled;

				if ( !checkParams( strParameters, iter->second.m_params, filled ) )
				{
					bool ignored = true;
					std::swap( ignored, m_ignored );

					try
					{
						result = iter->second.m_function( this, filled );
					}
					catch ( Exception & p_exc )
					{
						parseError( p_exc.getFullDescription() );
					}

					std::swap( ignored, m_ignored );
				}
				else
				{
					result = iter->second.m_function( this, filled );
				}
			}
		}

		return result;
	}

	void FileParser::doEnterBlock()
	{
		if ( m_ignored )
		{
			m_ignoreLevel++;
		}
	}

	void FileParser::doLeaveBlock()
	{
		if ( doIsInIgnoredBlock() )
		{
			m_ignoreLevel--;

			if ( m_ignoreLevel <= 0 )
			{
				m_ignored = false;
				m_ignoreLevel = 0;
				m_context->m_sections.pop_back();
			}
		}
		else
		{
			m_ignored = false;
			doParseScriptBlockEnd();
		}
	}

	bool FileParser::doIsInIgnoredBlock()
	{
		return m_ignored && m_ignoreLevel > 0;
	}

	String FileParser::doGetSectionsStack()
	{
		StringStream sections{ makeStringStream() };

		if ( m_context && m_context->m_sections.size() > 1 )
		{
			auto begin = m_context->m_sections.begin() + 1;
			sections << doGetSectionName( *begin );

			std::for_each( begin + 1, m_context->m_sections.end(), [&sections, this]( uint32_t p_section )
			{
				sections << cuT( "::" ) << doGetSectionName( p_section );
			} );
		}

		if ( !m_context->m_functionName.empty() )
		{
			sections << cuT( "::" ) << m_context->m_functionName;
		}

		return sections.str();
	}

	void FileParser::doAddDefine( String const & param )
	{
		m_context->m_functionName = "define";
		auto values = string::split( param, cuT( " \t" ), 1 );

		if ( values.size() == 2 )
		{
			auto iresult = m_defines.emplace( values[0], values[1] );

			if ( !iresult.second )
			{
				parseError( cuT( "Replacing an already existing value." ) );
			}
		}
		else
		{
			parseWarning( cuT( "Missing parameters." ) );
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
