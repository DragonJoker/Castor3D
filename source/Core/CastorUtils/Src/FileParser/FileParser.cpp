#include "FileParser.hpp"

namespace Castor
{
	namespace
	{
		String DoStripComments( String const & p_line )
		{
			String l_result = p_line;
			auto l_index = l_result.find( "//" );

			if ( l_index != String::npos )
			{
				l_result = l_result.substr( 0, l_index );
			}

			l_index = l_result.find( "/*" );

			if ( l_index != String::npos )
			{
				l_result = l_result.substr( l_index, l_result.find( "*/", l_index ) - l_index );
			}

			return l_result;
		}
	}

	FileParser::FileParser( uint32_t p_rootSectionId )
		: m_rootSectionId( p_rootSectionId )
		, m_context()
		, m_ignoreLevel( 0 )
		, m_ignored( false )
	{
	}

	FileParser::~FileParser()
	{
	}

	bool FileParser::ParseFile( Path const & p_strFileName )
	{
		m_ignoreLevel = 0;
		m_ignored = false;
		TextFile l_file( p_strFileName, File::OpenMode::eRead );
		return ParseFile( l_file );
	}

	bool FileParser::ParseFile( TextFile & p_file )
	{
		bool l_result = false;

		if ( p_file.IsOk() )
		{
			bool l_bNextIsOpenBrace = false;
			bool l_bCommented = false;
			Logger::LogInfo( cuT( "FileParser : Parsing file [" ) + p_file.GetFileName() + cuT( "]" ) );
			DoInitialiseParser( p_file );
			auto l_save = 0ull;

			if ( m_context->m_sections.empty() )
			{
				m_context->m_sections.push_back( m_rootSectionId );
			}

			std::swap( m_context->m_line, l_save );
			bool l_bReuse = false;
			String l_strLine;
			String l_strLine2;
			String l_file;
			p_file.CopyToString( l_file );
			string::replace( l_file, cuT( "\r\n" ), cuT( "\n" ) );
			StringArray l_lines = string::split( l_file, cuT( "\n" ), uint32_t( std::count( l_file.begin(), l_file.end(), '\n' ) + 1 ), true );
			auto l_it = l_lines.begin();

			while ( l_it != l_lines.end() )
			{
				if ( !l_bReuse )
				{
					l_strLine = *l_it++;
					m_context->m_line++;
				}
				else
				{
					l_bReuse = false;
				}

				//Logger::LogDebug( string::to_string( m_context->m_line ) + cuT( " - " ) + l_strLine.c_str() );
				string::trim( l_strLine );

				if ( !l_strLine.empty() )
				{
					if ( l_strLine.size() < 2 || l_strLine.substr( 0, 2 ) != cuT( "//" ) )
					{
						if ( !l_bCommented )
						{
							std::size_t l_uiCommentBegin = l_strLine.find( cuT( "/*" ) );

							if ( l_uiCommentBegin != String::npos )
							{
								std::size_t l_uiCommentEnd = l_strLine.find( cuT( "*/" ) );

								if ( l_uiCommentEnd < l_strLine.size() - 3 )
								{
									if ( l_uiCommentBegin > 0 )
									{
										l_strLine = l_strLine.substr( 0, l_uiCommentBegin ) + l_strLine.substr( l_uiCommentEnd + 2 );
									}
									else
									{
										l_strLine = l_strLine.substr( l_uiCommentEnd + 2 );
									}
								}
								else if ( l_uiCommentEnd != String::npos )
								{
									if ( l_uiCommentBegin > 0 )
									{
										l_strLine = l_strLine.substr( 0, l_uiCommentBegin );
									}
									else
									{
										l_strLine.clear();
									}
								}
								else if ( l_uiCommentBegin > 0 )
								{
									l_strLine = l_strLine.substr( 0, l_uiCommentBegin );
									l_bCommented = true;
								}
								else
								{
									l_bCommented = true;
								}

								if ( !l_strLine.empty() && !l_bCommented )
								{
									l_bReuse = true;
								}
							}
							else
							{
								if ( l_strLine.find( cuT( "{" ) ) == l_strLine.size() - 1 && l_strLine.size() > 1 )
								{
									// We got a "{" at the end of the line, so we split the line in two and reuse the line
									l_strLine2 = l_strLine.substr( 0, l_strLine.size() - 1 );
									string::trim( l_strLine2 );
									l_bNextIsOpenBrace = DoParseScriptLine( l_strLine2 );
									l_strLine = cuT( "{" );
									l_bReuse = true;
								}
								else
								{
									if ( l_bNextIsOpenBrace )
									{
										if ( l_strLine != cuT( "{" ) )
										{
											l_bNextIsOpenBrace = DoParseScriptBlockEnd();
											l_bReuse = true;
										}
										else
										{
											l_bNextIsOpenBrace = false;
											DoEnterBlock();
										}
									}
									else if ( l_strLine == cuT( "{" ) )
									{
										DoEnterBlock();
									}
									else
									{
										l_bNextIsOpenBrace = DoParseScriptLine( l_strLine );
									}

									if ( m_ignoreLevel > 0 )
									{
										if ( !l_strLine.empty() && l_strLine.find( cuT( "}" ) ) == l_strLine.size() - 1 )
										{
											DoLeaveBlock();
										}
									}
								}
							}
						}
						else
						{
							std::size_t l_uiCommentEnd = l_strLine.find( cuT( "*/" ) );

							if ( l_uiCommentEnd < l_strLine.size() - 3 )
							{
								l_strLine = l_strLine.substr( l_uiCommentEnd + 2 );
								l_bCommented = false;
								l_bReuse = true;
							}
							else if ( l_uiCommentEnd != String::npos )
							{
								l_strLine.clear();
								l_bCommented = false;
							}
						}
					}
				}
			}

			if ( m_context->m_sections.empty() || m_context->m_sections.back() != m_rootSectionId )
			{
				if ( m_context.use_count() == 1 )
				{
					ParseError( cuT( "Unexpected end of file" ) );
				}
				else
				{
					DoValidate();
					l_result = true;
				}
			}
			else
			{
				DoValidate();
				l_result = true;
			}

			Logger::LogInfo( cuT( "FileParser : Finished parsing file [" ) + p_file.GetFileName() + cuT( "]" ) );
			std::swap( m_context->m_line, l_save );
		}

		DoCleanupParser();
		return l_result;
	}

	void FileParser::ParseError( String const & p_error )
	{
		StringStream l_error;
		l_error << cuT( "Error, line #" ) << m_context->m_line << cuT( ": Directive <" ) << DoGetSectionsStack() << cuT( ">: " ) << p_error;
		Logger::LogError( l_error.str() );
	}

	void FileParser::ParseWarning( String const & p_warning )
	{
		StringStream l_error;
		l_error << cuT( "Warning, line #" ) << m_context->m_line << cuT( ": Directive <" ) << DoGetSectionsStack() << cuT( ">: " ) << p_warning;
		Logger::LogWarning( l_error.str() );
	}

	bool FileParser::CheckParams( String const & p_params, ParserParameterArray const & p_expected, ParserParameterArray & p_received )
	{
		bool l_result = true;
		String l_params( p_params );
		string::trim( l_params );
		String l_missingParam;

		for ( auto l_param : p_expected )
		{
			if ( l_result )
			{
				auto l_filled = l_param->Clone();
				l_result = l_filled->Parse( l_params );

				if ( !l_result )
				{
					l_missingParam = l_param->GetStrType();
				}
				else
				{
					p_received.push_back( l_filled );
				}
			}
		}

		if ( !l_params.empty() )
		{
			auto l_param = std::make_shared< ParserParameter< ParameterType::eText > >();
			l_param->m_value = l_params;
			p_received.push_back( l_param );
		}

		if ( !l_result )
		{
			ParseError( cuT( "Directive <" ) + m_context->m_functionName + cuT( "> needs a <" ) + l_missingParam + cuT( "> parameter that is currently missing" ) );
		}

		return l_result;
	}

	void FileParser::AddParser( uint32_t p_section, String const & p_name, ParserFunction p_function, ParserParameterArray && p_params )
	{
		auto l_sectionIt = m_parsers.find( p_section );

		if ( l_sectionIt != m_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
		{
			ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
		}
		else
		{
			m_parsers[p_section][p_name] = { p_function, p_params };
		}
	}

	bool FileParser::DoParseScriptLine( String & p_line )
	{
		bool l_bContinue = true;
		bool l_result = false;
		std::size_t l_uiBlockEndIndex = p_line.find( cuT( "}" ) );
		p_line = DoStripComments( p_line );

		if ( l_uiBlockEndIndex != String::npos )
		{
			m_context->m_functionName = cuT( "}" );

			if ( l_uiBlockEndIndex == 0 )
			{
				// Block end at the beginning of the line, we treat it then we parse the line
				p_line = p_line.substr( 1 );
				string::trim( p_line );
				DoLeaveBlock();

				if ( !p_line.empty() )
				{
					l_result = DoParseScriptLine( p_line );
				}
				else
				{
					l_result = false;
				}

				l_bContinue = false;
			}
			else if ( l_uiBlockEndIndex == p_line.size() - 1 )
			{
				// Block end at the end of the line : we treat the line then the block end
				p_line = p_line.substr( 0, l_uiBlockEndIndex );
				string::trim( p_line );

				if ( !p_line.empty() )
				{
					l_result = DoParseScriptLine( p_line );
				}
				else
				{
					l_result = false;
				}

				DoLeaveBlock();
				l_result = false;
				l_bContinue = false;
			}
			else
			{
				// Block end in the middle of the line, we don't treat it, we parse the line
				l_bContinue = true;
			}
		}
		else
		{
			// No block end, we parse the line
			l_bContinue = true;
		}

		if ( l_bContinue )
		{
			if ( !m_context->m_sections.empty() )
			{
				l_result = DoInvokeParser( p_line, m_parsers[m_context->m_sections.back()] );
			}
			else
			{
				l_result = DoDelegateParser( p_line );
			}
		}

		return l_result;
	}

	bool FileParser::DoParseScriptBlockEnd()
	{
		bool l_result = false;

		if ( !m_context->m_sections.empty() )
		{
			AttributeParserMap::const_iterator const & l_iter = m_parsers[m_context->m_sections.back()].find( cuT( "}" ) );

			if ( l_iter == m_parsers[m_context->m_sections.back()].end() )
			{
				m_context->m_sections.pop_back();
				l_result = false;
			}
			else
			{
				l_result = l_iter->second.m_function( this, l_iter->second.m_params );
			}
		}

		return l_result;
	}

	bool FileParser::DoInvokeParser( String & p_line, AttributeParserMap const & p_parsers )
	{
		bool l_result = false;
		StringArray l_splitCmd = string::split( p_line, cuT( " \t" ), 1, false );
		m_context->m_functionName = l_splitCmd[0];
		AttributeParserMap::const_iterator const & l_iter = p_parsers.find( l_splitCmd[0] );

		if ( !DoIsInIgnoredBlock() )
		{
			if ( l_iter == p_parsers.end() )
			{
				if ( !DoDiscardParser( p_line ) )
				{
					Ignore();
				}
			}
			else
			{
				String l_strParameters;

				if ( l_splitCmd.size() >= 2 )
				{
					l_strParameters = string::trim( l_splitCmd[1] );
				}

				ParserParameterArray l_filled;

				if ( !CheckParams( l_strParameters, l_iter->second.m_params, l_filled ) )
				{
					bool l_ignored = true;
					std::swap( l_ignored, m_ignored );

					try
					{
						l_result = l_iter->second.m_function( this, l_filled );
					}
					catch ( Exception & p_exc )
					{
						ParseError( p_exc.GetFullDescription() );
					}

					std::swap( l_ignored, m_ignored );
				}
				else
				{
					l_result = l_iter->second.m_function( this, l_filled );
				}
			}
		}

		return l_result;
	}

	void FileParser::DoEnterBlock()
	{
		if ( m_ignored )
		{
			m_ignoreLevel++;
		}
	}

	void FileParser::DoLeaveBlock()
	{
		if ( DoIsInIgnoredBlock() )
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
			DoParseScriptBlockEnd();
		}
	}

	bool FileParser::DoIsInIgnoredBlock()
	{
		return m_ignored && m_ignoreLevel > 0;
	}

	String FileParser::DoGetSectionsStack()
	{
		StringStream l_sections;

		if ( m_context && m_context->m_sections.size() > 1 )
		{
			auto l_begin = m_context->m_sections.begin() + 1;
			l_sections << DoGetSectionName( *l_begin );

			std::for_each( l_begin + 1, m_context->m_sections.end(), [&l_sections, this]( uint32_t p_section )
			{
				l_sections << cuT( "::" ) << DoGetSectionName( p_section );
			} );
		}

		if ( !m_context->m_functionName.empty() )
		{
			l_sections << cuT( "::" ) << m_context->m_functionName;
		}

		return l_sections.str();
	}
}
