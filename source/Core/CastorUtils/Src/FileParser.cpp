#include "FileParser.hpp"
#include "FileParserContext.hpp"
#include "Colour.hpp"
#include "Pixel.hpp"
#include "Logger.hpp"

#include <cstdarg>

namespace Castor
{
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

	bool FileParser::ParseFile( String const & p_strFileName )
	{
		m_ignoreLevel = 0;
		m_ignored = false;
		TextFile l_file( p_strFileName, File::eOPEN_MODE_READ );
		return ParseFile( l_file );
	}

	bool FileParser::ParseFile( TextFile & p_file )
	{
		bool l_return = false;

		if ( p_file.IsOk() )
		{
			bool l_bNextIsOpenBrace = false;
			bool l_bCommented = false;
			Logger::LogInfo( cuT( "FileParser : Parsing file [" ) + p_file.GetFileName() + cuT( "]" ) );
			DoInitialiseParser( p_file );
			m_context->m_sections.push( m_rootSectionId );
			m_context->m_line = 0;
			bool l_bReuse = false;
			String l_strLine;
			String l_strLine2;
			String l_file;
			p_file.CopyToString( l_file );
			string::replace( l_file, cuT( "\r\n" ), cuT( "\n" ) );
			StringArray l_lines = string::split( l_file, cuT( "\n" ), std::count( l_file.begin(), l_file.end(), '\n' ) + 1, true );
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

			if ( m_context->m_sections.empty() || m_context->m_sections.top() != m_rootSectionId )
			{
				ParseError( cuT( "Unexpected end of file" ) );
			}
			else
			{
				DoValidate();
				l_return = true;
			}

			Logger::LogInfo( cuT( "FileParser : Finished parsing file [" ) + p_file.GetFileName() + cuT( "]" ) );
		}

		DoCleanupParser();
		return l_return;
	}

	void FileParser::ParseError( String const & p_strError )
	{
		StringStream l_strError;
		l_strError << cuT( "Error, line #" ) << m_context->m_line << cuT( ": " ) << p_strError;
		Logger::LogError( l_strError.str() );
	}

	void FileParser::ParseWarning( String const & p_strWarning )
	{
		StringStream l_strError;
		l_strError << cuT( "Warning, line #" ) << m_context->m_line << cuT( ": " ) << p_strWarning;
		Logger::LogWarning( l_strError.str() );
	}

	bool FileParser::CheckParams( String const & p_strParams, ParserParameterArrayConstIt p_itBegin, ParserParameterArrayConstIt p_itEnd )
	{
		bool l_return = true;
		String l_strParams( p_strParams );
		string::trim( l_strParams );
		String l_strMissingParam;
		std::for_each( p_itBegin, p_itEnd, [&]( ParserParameterBaseSPtr p_pParam )
		{
			if ( l_return )
			{
				l_return = p_pParam->Parse( l_strParams );

				if ( !l_return )
				{
					l_strMissingParam = p_pParam->GetStrType();
				}
			}
		} );

		if ( !l_return )
		{
			ParseError( cuT( "Directive <" ) + m_context->m_functionName + cuT( "> needs a <" ) + l_strMissingParam + cuT( "> param that is currently missing" ) );
		}

		return l_return;
	}

	void FileParser::AddParser( uint32_t p_section, String const & p_name, ParserFunction p_function, uint32_t p_count, ... )
	{
		ParserParameterArray l_params;
		va_list l_valist;
		va_start( l_valist, p_count );

		for ( uint32_t i = 0; i < p_count; ++i )
		{
			ePARAMETER_TYPE l_eParamType =  ePARAMETER_TYPE( va_arg( l_valist, int ) );

			switch ( l_eParamType )
			{
			case ePARAMETER_TYPE_NAME:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_NAME > >( p_name ) );
				break;

			case ePARAMETER_TYPE_TEXT:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_TEXT > >( p_name ) );
				break;

			case ePARAMETER_TYPE_PATH:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PATH > >( p_name ) );
				break;

			case ePARAMETER_TYPE_CHECKED_TEXT:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
				break;

			case ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
				break;

			case ePARAMETER_TYPE_BOOL:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BOOL > >( p_name ) );
				break;

			case ePARAMETER_TYPE_INT8:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT8 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_INT16:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT16 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_INT32:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT32 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_INT64:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT64 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_UINT8:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT8 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_UINT16:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT16 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_UINT32:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT32 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_UINT64:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT64 > >( p_name ) );
				break;

			case ePARAMETER_TYPE_FLOAT:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_FLOAT > >( p_name ) );
				break;

			case ePARAMETER_TYPE_DOUBLE:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_DOUBLE > >( p_name ) );
				break;

			case ePARAMETER_TYPE_LONGDOUBLE:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_LONGDOUBLE > >( p_name ) );
				break;

			case ePARAMETER_TYPE_PIXELFORMAT:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PIXELFORMAT > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT2I:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2I > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT3I:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3I > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT4I:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4I > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT2F:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2F > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT3F:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3F > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT4F:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4F > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT2D:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2D > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT3D:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3D > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POINT4D:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4D > >( p_name ) );
				break;

			case ePARAMETER_TYPE_SIZE:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_SIZE > >( p_name ) );
				break;

			case ePARAMETER_TYPE_POSITION:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POSITION > >( p_name ) );
				break;

			case ePARAMETER_TYPE_RECTANGLE:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_RECTANGLE > >( p_name ) );
				break;

			case ePARAMETER_TYPE_COLOUR:
				l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_COLOUR > >( p_name ) );
				break;
			}
		}

		va_end( l_valist );
		AddParser( p_section, p_name, p_function, std::move( l_params ) );
	}

	void FileParser::AddParser( uint32_t p_section, String const & p_name, ParserFunction p_function, ParserParameterArray && p_params )
	{
		auto && l_sectionIt = m_parsers.find( p_section );

		if ( l_sectionIt != m_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
		{
			ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
		}
		else
		{
			m_parsers[p_section][p_name] = { p_function, p_params };
		}
	}

	bool FileParser::DoParseScriptLine( String & p_strLine )
	{
		bool l_bContinue = true;
		bool l_return = false;
		std::size_t l_uiBlockEndIndex = p_strLine.find( cuT( "}" ) );

		if ( l_uiBlockEndIndex != String::npos )
		{
			if ( l_uiBlockEndIndex == 0 )
			{
				// Block end at the beginning of the line, we treat it then we parse the line
				p_strLine = p_strLine.substr( 1 );
				string::trim( p_strLine );
				DoLeaveBlock();

				if ( !p_strLine.empty() )
				{
					l_return = DoParseScriptLine( p_strLine );
				}
				else
				{
					l_return = false;
				}

				l_bContinue = false;
			}
			else if ( l_uiBlockEndIndex == p_strLine.size() - 1 )
			{
				// Block end at the end of the line : we treat the line then the block end
				p_strLine = p_strLine.substr( 0, l_uiBlockEndIndex );
				string::trim( p_strLine );

				if ( !p_strLine.empty() )
				{
					l_return = DoParseScriptLine( p_strLine );
				}
				else
				{
					l_return = false;
				}

				DoLeaveBlock();
				l_return = false;
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
				l_return = DoInvokeParser( p_strLine, m_parsers[m_context->m_sections.top()] );
			}
			else
			{
				l_return = DoDelegateParser( p_strLine );
			}
		}

		return l_return;
	}

	bool FileParser::DoParseScriptBlockEnd()
	{
		bool l_return = false;

		if ( !m_context->m_sections.empty() )
		{
			AttributeParserMap::const_iterator const & l_iter = m_parsers[m_context->m_sections.top()].find( cuT( "}" ) );

			if ( l_iter == m_parsers[m_context->m_sections.top()].end() )
			{
				m_context->m_sections.pop();
				l_return = false;
			}
			else
			{
				l_return = l_iter->second.m_function( this, l_iter->second.m_params );
			}
		}

		return l_return;
	}

	bool FileParser::DoInvokeParser( String & p_strLine, AttributeParserMap const & p_parsers )
	{
		bool l_return = false;
		StringArray l_splitCmd = string::split( p_strLine, cuT( " \t" ), 1, false );
		m_context->m_functionName = l_splitCmd[0];
		AttributeParserMap::const_iterator const & l_iter = p_parsers.find( l_splitCmd[0] );

		if ( !DoIsInIgnoredBlock() )
		{
			if ( l_iter == p_parsers.end() )
			{
				if ( !DoDiscardParser( p_strLine ) )
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

				if ( !CheckParams( l_strParameters, l_iter->second.m_params.begin(), l_iter->second.m_params.end() ) )
				{
					bool l_ignored = true;
					std::swap( l_ignored, m_ignored );
					l_return = l_iter->second.m_function( this, l_iter->second.m_params );
					std::swap( l_ignored, m_ignored );
				}
				else
				{
					l_return = l_iter->second.m_function( this, l_iter->second.m_params );
				}
			}
		}

		return l_return;
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
		if ( m_ignored )
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
			DoParseScriptBlockEnd();
		}
	}

	bool FileParser::DoIsInIgnoredBlock()
	{
		return m_ignored && m_ignoreLevel > 0;
	}
}
