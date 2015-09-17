#include "FileParser.hpp"
#include "FileParserContext.hpp"
#include "Colour.hpp"
#include "Pixel.hpp"
#include "Logger.hpp"

#include <cstdarg>

namespace Castor
{
	FileParser::FileParser( int p_iRootSectionNumber, int p_iSectionCount )
		:	m_iRootSectionNumber( p_iRootSectionNumber	)
		,	m_iSectionCount( p_iSectionCount	)
		,	m_pParsingContext(	)
		,	m_iIgnoreLevel( 0	)
		,	m_bIgnored( false	)
	{
	}

	FileParser::~FileParser()
	{
	}

	bool FileParser::ParseFile( String const & p_strFileName )
	{
		m_iIgnoreLevel = 0;
		m_bIgnored = false;
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
			m_pParsingContext->stackSections.push( m_iRootSectionNumber );
			m_pParsingContext->ui64Line = 0;
			bool l_bReuse = false;
			String l_strLine;
			String l_strLine2;
			String l_file;
			p_file.CopyToString( l_file );
			str_utils::replace( l_file, cuT( "\r\n" ), cuT( "\n" ) );
			str_utils::replace( l_file, cuT( "\n\n" ), cuT( "\n" ) );
			StringArray l_lines = str_utils::split( l_file, cuT( "\n" ), std::count( l_file.begin(), l_file.end(), '\n' ) + 1, true );
			auto l_it = l_lines.begin();

			while ( l_it != l_lines.end() )
			{
				if ( !l_bReuse )
				{
					l_strLine = *l_it++;
					m_pParsingContext->ui64Line++;
				}
				else
				{
					l_bReuse = false;
				}

				//Logger::LogDebug( str_utils::to_string( m_pParsingContext->ui64Line ) + cuT( " - " ) + l_strLine.c_str() );
				str_utils::trim( l_strLine );

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
									str_utils::trim( l_strLine2 );
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

											if ( m_bIgnored )
											{
												m_iIgnoreLevel++;
											}
										}
									}
									else
									{
										l_bNextIsOpenBrace = DoParseScriptLine( l_strLine );
									}

									if ( m_iIgnoreLevel > 0 )
									{
										if ( l_strLine.find( cuT( "}" ) ) == l_strLine.size() - 1 )
										{
											m_iIgnoreLevel--;

											if ( m_iIgnoreLevel <= 0 )
											{
												m_bIgnored = false;
												m_iIgnoreLevel = 0;
											}
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

			if ( m_pParsingContext->stackSections.top() != m_iRootSectionNumber )
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
		l_strError << cuT( "Error, line #" ) << m_pParsingContext->ui64Line << cuT( ": " ) << p_strError;
		Logger::LogError( l_strError.str() );
	}

	void FileParser::ParseWarning( String const & p_strWarning )
	{
		StringStream l_strError;
		l_strError << cuT( "Warning, line #" ) << m_pParsingContext->ui64Line << cuT( ": " ) << p_strWarning;
		Logger::LogWarning( l_strError.str() );
	}

	bool FileParser::CheckParams( String const & p_strParams, ParserParameterArrayConstIt p_itBegin, ParserParameterArrayConstIt p_itEnd )
	{
		bool l_return = true;
		String l_strParams( p_strParams );
		str_utils::trim( l_strParams );
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
			ParseError( cuT( "Directive <" ) + m_pParsingContext->strFunctionName + cuT( "> needs a <" ) + l_strMissingParam + cuT( "> param that is currently missing" ) );
		}

		return l_return;
	}

	void FileParser::AddParser( int p_iSection, String const & p_strName, PParserFunction p_pfnFunction, int p_iCount, ... )
	{
		ParserParameterArray l_arrayParams;
		va_list l_valist;
		va_start( l_valist, p_iCount );

		for ( int i = 0; i < p_iCount; ++i )
		{
			ePARAMETER_TYPE l_eParamType =  ePARAMETER_TYPE( va_arg( l_valist, int ) );

			switch ( l_eParamType )
			{
			case ePARAMETER_TYPE_NAME:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_NAME > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_TEXT:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_TEXT > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_PATH:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PATH > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_CHECKED_TEXT:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > >( *m_pParsingContext, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
				break;

			case ePARAMETER_TYPE_BOOL:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BOOL > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_INT8:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT8 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_INT16:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT16 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_INT32:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT32 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_INT64:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT64 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_UINT8:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT8 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_UINT16:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT16 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_UINT32:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT32 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_UINT64:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT64 > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_FLOAT:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_FLOAT > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_DOUBLE:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_DOUBLE > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_LONGDOUBLE:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_LONGDOUBLE > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_PIXELFORMAT:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PIXELFORMAT > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT2I:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2I > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT3I:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3I > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT4I:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4I > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT2F:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2F > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT3F:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3F > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT4F:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4F > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT2D:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2D > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT3D:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3D > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POINT4D:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4D > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_SIZE:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_SIZE > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_POSITION:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POSITION > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_RECTANGLE:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_RECTANGLE > >( *m_pParsingContext ) );
				break;

			case ePARAMETER_TYPE_COLOUR:
				l_arrayParams.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_COLOUR > >( *m_pParsingContext ) );
				break;
			}
		}

		va_end( l_valist );
		m_mapParsers[p_iSection][p_strName] = std::make_pair( p_pfnFunction, l_arrayParams );
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
				str_utils::trim( p_strLine );
				DoParseScriptBlockEnd();

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
				str_utils::trim( p_strLine );

				if ( !p_strLine.empty() )
				{
					l_return = DoParseScriptLine( p_strLine );
				}
				else
				{
					l_return = false;
				}

				DoParseScriptBlockEnd();
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
			if ( m_pParsingContext->stackSections.top() >= 0 && m_pParsingContext->stackSections.top() < m_iSectionCount )
			{
				l_return = DoInvokeParser( p_strLine, m_mapParsers[m_pParsingContext->stackSections.top()] );
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

		if ( m_pParsingContext->stackSections.top() >= 0 && m_pParsingContext->stackSections.top() < m_iSectionCount )
		{
			AttributeParserMap::const_iterator const & l_iter = m_mapParsers[m_pParsingContext->stackSections.top()].find( cuT( "}" ) );

			if ( l_iter == m_mapParsers[m_pParsingContext->stackSections.top()].end() )
			{
				m_pParsingContext->stackSections.pop();
				l_return = false;
			}
			else
			{
				l_return = l_iter->second.first( this, l_iter->second.second );
			}
		}

		return l_return;
	}

	bool FileParser::DoInvokeParser( String & p_strLine, AttributeParserMap const & p_parsers )
	{
		bool l_return = false;
		StringArray l_splitCmd = str_utils::split( p_strLine, cuT( " \t" ), 1, false );
		m_pParsingContext->strFunctionName = l_splitCmd[0];
		AttributeParserMap::const_iterator const & l_iter = p_parsers.find( l_splitCmd[0] );

		if ( l_iter == p_parsers.end() )
		{
			DoDiscardParser( p_strLine );
		}
		else
		{
			String l_strParameters;

			if ( l_splitCmd.size() >= 2 )
			{
				l_strParameters = str_utils::trim( l_splitCmd[1] );
			}

			if ( !CheckParams( l_strParameters, l_iter->second.second.begin(), l_iter->second.second.end() ) )
			{
				bool l_ignored = true;
				std::swap( l_ignored, m_bIgnored );
				l_return = l_iter->second.first( this, l_iter->second.second );
				std::swap( l_ignored, m_bIgnored );
			}
			else
			{
				l_return = l_iter->second.first( this, l_iter->second.second );
			}
		}

		return l_return;
	}
}
