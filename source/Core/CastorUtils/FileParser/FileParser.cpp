#include "CastorUtils/FileParser/FileParser.hpp"

#include "CastorUtils/FileParser/ParserParameter.hpp"
#include "CastorUtils/Data/ZipArchive.hpp"

namespace castor
{
	namespace fileprs
	{
		static constexpr bool DisplayLines = false;

		static void selectMainFile( Path const & pathFile
			, Path & path
			, PreprocessedFile const & preprocessed )
		{
			if ( PathArray files;
				File::listDirectoryFiles( path, files, true ) )
			{
				if ( auto it = std::find_if( files.begin()
					, files.end()
					, [&pathFile]( Path const & lookup )
					{
						auto fileName = lookup.getFileName( true );
						return fileName == cuT( "main.cscn" )
							|| fileName == cuT( "scene.cscn" )
							|| fileName == pathFile.getFileName() + cuT( ".cscn" );
					} );
					it != files.end() )
				{
					path = *it;
					preprocessed.getContext().file = path;
				}
				else
				{
					auto fileIt = std::find_if( files.begin()
						, files.end()
						, []( Path const & lookup )
						{
								return lookup.getExtension() == cuT( "cscn" );
						} );

					if ( fileIt != files.end() )
					{
						path = *fileIt;
						preprocessed.getContext().file = path;
					}
				}
			}
		}

		static void trimLine( StringView & text
			, StringView seps = cuT( " \t\r" ) )
		{
			if ( !text.empty() )
			{
				auto index = text.find_first_not_of( seps );

				if ( index > 0 )
				{
					if ( index != String::npos )
					{
						text = text.substr( index, String::npos );
					}
					else
					{
						text = text.substr( 0, 0 );
					}
				}

				if ( !text.empty() )
				{
					index = text.find_last_not_of( seps );

					if ( index < text.size() - 1 )
					{
						if ( index != String::npos )
						{
							text = text.substr( 0, index + 1 );
						}
						else
						{
							text = text.substr( 0, 0 );
						}
					}
				}
			}
		}

		static StringView getLine( LoggerInstance & logger
			, String const & content
			, uint64_t & lineIndex
			, size_t & offset )
		{
			if ( offset == content.size() )
			{
				return StringView{};
			}

			auto current = std::next( content.begin(), ptrdiff_t( offset ) );
			auto it = std::find_if( current
				, content.end()
				, []( xchar const & c )
				{
					return c == '\n' || c == '\r';
				} );
			offset = size_t( std::distance( content.begin(), it ) );
			StringView result{ &*current, size_t( std::distance( current, it ) ) };

			if ( it != content.end() && *it == '\r' )
			{
				++offset;
				++it;
			}

			if ( it != content.end() && *it == '\n' )
			{
				++offset;
				++it;
			}

			if constexpr ( DisplayLines )
			{
				auto idx = makeStringStream();
				idx << std::setw( 8 ) << std::left << std::setfill( ' ' ) << ( lineIndex + 1u );
				logger.logDebug( idx.str() + String{ result } );
			}

			trimLine( result );
			// Trim RHS of inline comment start
			result = result.substr( 0, result.find( cuT( "//" ) ) );

			if ( result.empty() && it != content.end() )
			{
				++lineIndex;
				result = getLine( logger, content, lineIndex, offset );
			}

			return result;
		}

		static void splitLineCommentBlock( StringView text
			, Vector< StringView > & work )
		{
			trimLine( text );
			auto commentBegin = text.find( cuT( "/*" ) );
			auto commentEnd = text.find( cuT( "*/" ) );

			if ( commentBegin != StringView::npos
				|| commentEnd != StringView::npos )
			{
				auto min = std::min( commentBegin, commentEnd );
				auto max = std::max( commentBegin, commentEnd );
				auto before = text.substr( 0, min );
				trimLine( before );

				if ( !before.empty() )
				{
					work.emplace_back( before );
				}

				work.emplace_back( text.substr( min, 2 ) );

				if ( max != StringView::npos )
				{
					min += 2;
					auto between = text.substr( min, max - min );
					auto after = text.substr( max + 2 );
					trimLine( between );
					trimLine( after );

					if ( !between.empty() )
					{
						work.emplace_back( between );
					}

					work.emplace_back( text.substr( max, 2 ) );

					if ( !after.empty() )
					{
						work.emplace_back( after );
					}
				}
				else
				{
					auto after = text.substr( min + 2 );
					trimLine( after );

					if ( !after.empty() )
					{
						work.emplace_back( after );
					}
				}
			}
			else if ( !text.empty() )
			{
				work.emplace_back( text );
			}
		}

		static void splitLine( StringView text
			, Vector< StringView > & work )
		{
			std::size_t pos = 0;
			std::size_t start = 0;

			do
			{
				pos = text.find_first_of( cuT( "{}" ), start );

				if ( pos == String::npos )
				{
					splitLineCommentBlock( text.substr( start ), work );
				}
				else
				{
					if ( pos != start )
					{
						splitLineCommentBlock( text.substr( start, pos - start ), work );
					}

					start = pos + 1;
					work.emplace_back( &text[pos], 1u );
				}
			}
			while ( pos != String::npos );
		}
	}

	//*********************************************************************************************

	void addParser( AttributeParsers & parsers
		, uint32_t oldSection
		, uint32_t newSection
		, String const & name
		, ParserFunction function
		, ParserParameterArray params )
	{
		auto nameIt = parsers.find( name );

		if ( nameIt != parsers.end()
			&& nameIt->second.find( oldSection ) != nameIt->second.end() )
		{
			StringStream stream{ makeStringStream() };
			stream << cuT( "Error: Parser " ) << name
				<< cuT( " for section " ) << string::toString( oldSection )
				<< cuT( " already exists." );
			Logger::logError( stream.str() );
		}
		else
		{
			parsers[name][oldSection] = { castor::move( function )
				, newSection
				, castor::move( params ) };
		}
	}

	//*********************************************************************************************

	PreprocessedFile::PreprocessedFile( FileParser & parser )
		: m_parser{ parser }
	{
	}

	PreprocessedFile::PreprocessedFile( FileParser & parser
		, FileParserContextUPtr context )
		: m_parser{ parser }
		, m_context{ castor::move( context ) }
	{
		m_context->preprocessed = this;
	}

	PreprocessedFile::PreprocessedFile( PreprocessedFile && rhs )noexcept
		: m_parser{ rhs.m_parser }
		, m_context{ castor::move( rhs.m_context ) }
		, m_actions{ castor::move( rhs.m_actions ) }
		, m_current{ castor::move( rhs.m_current ) }
		, m_popAction{ castor::move( rhs.m_popAction ) }
	{
	}

	PreprocessedFile & PreprocessedFile::operator=( PreprocessedFile && rhs )noexcept
	{
		m_context = castor::move( rhs.m_context );
		m_actions = castor::move( rhs.m_actions );
		m_current = castor::move( rhs.m_current );
		m_popAction = castor::move( rhs.m_popAction );

		return *this;
	}

	void PreprocessedFile::addParserAction( Path file
		, uint64_t line
		, String name
		, uint32_t section
		, ParserFunctionAndParams function
		, String params
		, bool implicit )
	{
		doAddParserAction( castor::move( file )
			, line
			, castor::move( name )
			, section
			, castor::move( function )
			, castor::move( params )
			, implicit );
	}

	bool PreprocessedFile::parse()
	{
		CU_Require( m_context );
		bool result = !doIsEmpty();

		if ( !result )
		{
			return result;
		}

		m_context->preprocessed = this;
		m_context->pendingSection = 0u;
		m_context->pendingBlock = nullptr;
		m_context->sections.clear();
		m_context->sections.push_back( m_parser.getRootSectionId() );
		m_context->blocks.clear();
		m_context->blocks.push_back( m_parser.getRootBlockContext() );
		auto first = doGetFirstAction();
		Vector< Action > jobs;
		jobs.push_back( *first );

		while ( !jobs.empty() )
		{
			CU_Require( m_context->sections.size() == m_context->blocks.size() );
			auto action = *jobs.begin();
			jobs.erase( jobs.begin() );

			onAction( m_context->sections.back(), action );
			m_context->line = action.line;
			m_context->functionName = action.name;

			if ( !m_context->sections.empty() )
			{
				auto section = m_context->sections.back();
				CU_Require( section == action.section );
				auto block = m_context->blocks.back();
				ParserParameterArray filled;
				doCheckParams( action.params
					, action.function.params
					, filled );

				try
				{
					if constexpr ( fileprs::DisplayLines )
					{
						auto idx = makeStringStream();
						idx << std::setw( 8 ) << std::left << std::setfill( ' ' ) << action.line;
						m_parser.getLogger().logDebug( action.file + cuT( ":" ) + idx.str() + cuT( " (" ) + action.name + cuT( ")" ) );
					}

					action.function.function( *m_context, block, filled );
					CU_Require( m_context->sections.size() == m_context->blocks.size() );
				}
				catch ( Exception & exc )
				{
					parseError( makeString( exc.getDescription() ) );
				}
			}
			else
			{
				parseError( cuT( "Unexpected directive [" ) + action.name + cuT( "]" ) );
			}

			if ( jobs.empty() )
			{
				if ( auto next = doGetNextAction() )
				{
					jobs.push_back( *next );
				}
			}
		}

		if ( m_context->sections.empty() || m_context->sections.back() != m_parser.getRootSectionId() )
		{
			parseError( cuT( "Unexpected end of file" ) );
			m_parser.validate( *this );
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

				if ( !result && !param->isSet() )
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
			auto param = castor::make_shared< ParserParameter< ParameterType::eText > >();
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

			std::for_each( begin + 1
				, m_context->sections.end()
				, [&sections, this]( uint32_t section )
				{
					sections << cuT( "::" ) << m_parser.getSectionName( section );
				} );
		}

		if ( !m_context->functionName.empty() )
		{
			sections << cuT( "::" ) << m_context->functionName;
		}

		return sections.str();
	}

	bool PreprocessedFile::doIsEmpty()const
	{
		return m_actions.empty();
	}

	PreprocessedFile::Action * PreprocessedFile::doGetFirstAction()
	{
		m_current = m_actions.begin();
		return &( *m_current );
	}

	PreprocessedFile::Action * PreprocessedFile::doGetNextAction()
	{
		++m_current;
		return m_current == m_actions.end() ? nullptr : &( *m_current );
	}

	void PreprocessedFile::doAddParserAction( Path file
		, uint64_t line
		, String name
		, uint32_t section
		, ParserFunctionAndParams function
		, String params
		, bool implicit )
	{
		m_actions.emplace_back( castor::move( file )
			, line
			, castor::move( name )
			, section
			, castor::move( function )
			, castor::move( params )
			, implicit );
	}

	//*********************************************************************************************

	FileParser::FileParser( SectionId rootSectionId
		, void * rootBlockContext )
		: FileParser{ *Logger::getSingleton().getInstance(), rootSectionId, rootBlockContext }
	{
	}

	FileParser::FileParser( LoggerInstance & logger
		, SectionId rootSectionId
		, void * rootBlockContext )
		: m_rootSectionId( rootSectionId )
		, m_rootBlockContext{ rootBlockContext }
		, m_logger{ logger }
	{
	}

	void FileParser::registerParsers( String const & name
		, AdditionalParsers const & parsers )
	{
		if ( !m_additionalParsers.try_emplace( name, parsers ).second )
		{
			CU_Exception( cuT( "Additional parsers for name [" ) + name + cuT( "] already exist." ) );
		}
	}

	void FileParser::processFile( String const & appName
		, Path path
		, PreprocessedFile & preprocessed )
	{
		m_ignoreLevel = 0;
		m_ignored = false;

		if ( path.getExtension() == cuT( "zip" ) )
		{
			auto pathFile = path;
			ZipArchive archive{ path, File::OpenMode::eRead };
			path = File::getUserDirectory() / ( cuT( "." ) + appName ) / pathFile.getFileName();
			bool carryOn = true;

			if ( !File::directoryExists( path ) )
			{
				carryOn = archive.inflate( path );
			}

			if ( carryOn )
			{
				fileprs::selectMainFile( pathFile, path, preprocessed );
			}
		}

		String content;

		if ( TextFile file{ path, File::OpenMode::eRead };
			file.isOk() )
		{
			file.copyToString( content );
		}
		else
		{
			m_logger.logError( cuT( "FileParser : Couldn't open file [" ) + path.getFileName( true ) + cuT( "]." ) );
		}

		if ( !content.empty() )
		{
			m_logger.logInfo( cuT( "FileParser : Preprocessing file [" ) + path.getFileName( true ) + cuT( "]." ) );
			processFile( path, content, preprocessed );
			m_logger.logInfo( cuT( "FileParser : Finished preprocessing file [" ) + path.getFileName( true ) + cuT( "]." ) );
		}
	}

	void FileParser::processFile( Path const & path
		, PreprocessedFile & preprocessed )
	{
		processFile( cuT( "Castor" ), path, preprocessed );
	}

	void FileParser::processFile( Path const & path
		, String const & content
		, PreprocessedFile & preprocessed )
	{
		if ( m_sections.empty() )
		{
			m_sections.push_back( m_rootSectionId );
		}

		m_path = path.getPath();
		m_fileName = path.getFileName( true );
		bool isNextOpenBrace = false;
		SectionId pendingSection{};
		bool commented = false;
		uint64_t lineIndex = 0ULL;

		Vector< StringView > work;
		Vector< StringView > nextWork;
		size_t offset = 0u;
		auto nxtLine = fileprs::getLine( m_logger, content, lineIndex, offset );
		fileprs::splitLine( nxtLine, nextWork );

		while ( !nxtLine.empty() )
		{
			if ( work.empty() )
			{
				++lineIndex;
				castor::swap( work, nextWork );
				nxtLine = fileprs::getLine( m_logger, content, lineIndex, offset );
				fileprs::splitLine( nxtLine, nextWork );
			}

			while ( !work.empty() )
			{
				auto curLine = work.front();
				work.erase( work.begin() );
				doProcessLine( curLine, preprocessed, work, nextWork, lineIndex, pendingSection, commented, isNextOpenBrace );
			}
		}
	}

	PreprocessedFile FileParser::processFile( String const & appName
		, Path const & path )
	{
		PreprocessedFile result{ *this
			, doInitialiseParser( path ) };
		processFile( appName, path, result );
		return result;
	}

	PreprocessedFile FileParser::processFile( Path const & path )
	{
		return processFile( cuT( "Castor" ), path );
	}

	PreprocessedFile FileParser::processFile( Path const & path
		, String const & content )
	{
		PreprocessedFile result{ *this
			, doInitialiseParser( path ) };
		processFile( path, content, result );
		return result;
	}

	bool FileParser::parseFile( String const & appName
		, Path const & path )
	{
		auto preprocessed = processFile( appName, path );
		return preprocessed.parse();
	}

	bool FileParser::parseFile( Path const & path )
	{
		return parseFile( cuT( "Castor" ), path );
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

	void FileParser::addParser( SectionId oldSection
		, SectionId newSection
		, String const & name
		, ParserFunction const & function
		, ParserParameterArray const & params )
	{
		auto & parsers = m_parsers.try_emplace( name ).first->second;
		auto [it, res] = parsers.try_emplace( oldSection, function, newSection, params );

		if ( !res )
		{
			m_logger.logError( makeStringStream()
				<< cuT( "Parser " ) << name
				<< cuT( " for section " ) << doGetSectionName( oldSection )
				<< cuT( " (" ) << oldSection
				<< cuT( ") already exists." ) );
		}
	}

	void FileParser::addParser( SectionId section
		, String const & name
		, ParserFunction const & function
		, ParserParameterArray const & params )
	{
		addParser( section, section, name, function, params );
	}

	bool FileParser::doDiscardParser( PreprocessedFile & preprocessed
		, StringView value )
	{
		auto const & context = preprocessed.getContext();
		Logger::logWarning( makeStringStream()
			<< cuT( "Parser not found @ line " ) << context.line
			<< cuT( " : " ) + String{ value } );
		return false;
	}

	void FileParser::doProcessNoBlockLine( StringView curLine
		, PreprocessedFile & preprocessed
		, Vector< StringView > const & work
		, Vector< StringView > const & nextWork
		, uint64_t lineIndex
		, SectionId & pendingSection
		, bool & isNextOpenBrace )
	{
		// No opening brace on the line
		if ( isNextOpenBrace )
		{
			// Optional block not filled, emulate block end.
			doEnterBlock( preprocessed, pendingSection, lineIndex, true );
			doLeaveBlock( preprocessed, lineIndex, true );
			isNextOpenBrace = false;
		}

		if ( curLine == cuT( "}" ) )
		{
			doLeaveBlock( preprocessed, lineIndex, false );
		}
		else
		{
			auto [nextOpenBrace, newSection] = doInvokeParser( preprocessed
				, curLine
				, ( work.empty()
					? nextWork.empty() ? cuT( "" ) : nextWork.front()
					: work.front() )
				, lineIndex );
			isNextOpenBrace = nextOpenBrace;

			if ( isNextOpenBrace )
			{
				pendingSection = newSection;
			}
		}
	}

	void FileParser::doProcessLine( StringView curLine
		, PreprocessedFile & preprocessed
		, Vector< StringView > const & work
		, Vector< StringView > const & nextWork
		, uint64_t lineIndex
		, SectionId & pendingSection
		, bool & commented
		, bool & isNextOpenBrace )
	{
		if constexpr ( fileprs::DisplayLines )
		{
			m_logger.logDebug( cuT( "[" ) + String{ curLine } + cuT( "]" ) );
		}

		if ( !commented )
		{
			commented = curLine == cuT( "/*" );

			if ( !commented )
			{
				if ( curLine == cuT( "{" ) )
				{
					isNextOpenBrace = false;
					doEnterBlock( preprocessed, pendingSection, lineIndex, false );
				}
				else
				{
					doProcessNoBlockLine( curLine, preprocessed, work, nextWork, lineIndex, pendingSection, isNextOpenBrace );
				}
			}
		}
		else if ( curLine == cuT( "*/" ) )
		{
			commented = false;
		}
	}

	void FileParser::doParseScriptBlockBegin( PreprocessedFile & preprocessed
		, uint32_t newSection
		, uint64_t lineIndex
		, bool implicit )
	{
		auto defaultPush = []( FileParserContext & context
			, CU_UnusedParam( void *, block )
			, ParserParameterArray const & )
		{
			context.sections.push_back( context.pendingSection );
			context.blocks.push_back( context.pendingBlock );
			return false;
		};
		auto section = m_sections.back();
		preprocessed.addParserAction( m_path / m_fileName
			, lineIndex
			, cuT( "{" )
			, section
			, ParserFunctionAndParams{ defaultPush, newSection, {} }
			, {}
			, implicit );
		m_sections.push_back( newSection );
	}

	bool FileParser::doParseScriptBlockEnd( PreprocessedFile & preprocessed
		, uint64_t lineIndex
		, bool implicit )
	{
		bool result = false;
		auto it = m_parsers.find( cuT( "}" ) );
		auto curSection = m_sections.back();
		m_sections.pop_back();
		auto nxtSection = m_sections.back();

		if ( it != m_parsers.end() )
		{
			auto itFunc = it->second.find( curSection );

			if ( itFunc != it->second.end() )
			{
				preprocessed.addParserAction( m_path / m_fileName
					, lineIndex
					, cuT( "}" )
					, curSection
					, { itFunc->second.function, nxtSection, itFunc->second.params }
					, {}
					, implicit );
				result = true;
			}
		}

		if ( !result )
		{
			auto defaultPop = []( FileParserContext & context
				, void *
				, ParserParameterArray const & )
			{
				context.sections.pop_back();
				context.blocks.pop_back();
				return false;
			};
			preprocessed.addParserAction( m_path / m_fileName
				, lineIndex
				, cuT( "}" )
				, curSection
				, ParserFunctionAndParams{ defaultPop, nxtSection, {} }
				, {}
				, implicit );
		}

		return result;
	}

	Pair< bool, SectionId > FileParser::doInvokeParser( PreprocessedFile & preprocessed
		, StringView line
		, StringView nextToken
		, uint64_t lineIndex )
	{
		bool result = false;
		auto section = m_sections.back();

		if ( !doIsInIgnoredBlock() )
		{
			m_ignored = false;
			auto splitCmd = string::split( line, cuT( " \t" ), 1, false );
			String functionName{ splitCmd[0] };
			auto iter = m_parsers.find( functionName );
			bool found = false;

			if ( iter != m_parsers.end() )
			{
				auto itFunc = iter->second.find( section );

				if ( itFunc != iter->second.end() )
				{
					String parameters;

					if ( splitCmd.size() > 1u )
					{
						parameters = string::trim( splitCmd[1] );
						doCheckDefines( parameters );
					}

					result = nextToken == cuT( "{" )
						|| ( itFunc->second.resultSection != section
							&& functionName != cuT( "}" ) );
					auto nextSection = itFunc->second.resultSection;

					if ( result && nextToken != cuT( "{" ) )
					{
						nextSection = section;
					}

					preprocessed.addParserAction( m_path / m_fileName
						, lineIndex
						, functionName
						, section
						, { itFunc->second.function, nextSection, itFunc->second.params }
						, parameters
						, false );
					section = itFunc->second.resultSection;
					found = true;
				}
				else
				{
					parseError( functionName, lineIndex, cuT( "Directive [" ) + functionName + cuT( "] not found for section " ) + getSectionName( section ) );
				}
			}

			if ( !found )
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
		}

		return { result, section };
	}

	void FileParser::doEnterBlock( PreprocessedFile & preprocessed
		, SectionId newSection
		, uint64_t lineIndex
		, bool implicit )
	{
		if ( m_ignored )
		{
			m_ignoreLevel++;
		}
		else
		{
			doParseScriptBlockBegin( preprocessed, newSection, lineIndex, implicit );
		}
	}

	void FileParser::doLeaveBlock( PreprocessedFile & preprocessed
		, uint64_t lineIndex
		, bool implicit )
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
			doParseScriptBlockEnd( preprocessed, lineIndex, implicit );
		}
	}

	bool FileParser::doIsInIgnoredBlock()const
	{
		return m_ignored && m_ignoreLevel > 0;
	}

	void FileParser::doIncludeFile( PreprocessedFile & preprocessed
		, CU_UnusedParam( uint64_t, lineIndex )
		, StringView param )
	{
		String params{ param };
		Path path;

		if ( ValueParser< ParameterType::ePath >::parse( m_logger, params, path ) )
		{
			auto subparser = doCreateParser();
			subparser->doInitialiseParser( m_path / path );
			subparser->m_sections = m_sections;
			subparser->processFile( m_path / path, preprocessed );
		}
	}

	void FileParser::doAddDefine( uint64_t lineIndex
		, StringView param )
	{
		auto functionName = cuT( "define" );
		auto values = string::split( param, cuT( " \t" ), 1 );

		if ( values.size() == 2 )
		{
			auto [it, res] = m_defines.emplace( values[0], values[1] );

			if ( !res )
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

	FileParserContextUPtr FileParser::doInitialiseParser( Path const & path )
	{
		for ( auto const & [_, additional] : m_additionalParsers )
		{
			for ( auto const & [name, parsers] : additional.parsers )
			{
				for ( auto const & [section, parser] : parsers )
				{
					addParser( section, parser.resultSection, name, parser.function, parser.params );
				}
			}
		}

		return castor::makeUnique< FileParserContext >( *this, path );
	}

	void FileParser::doCheckDefines( String & text )const
	{
		static String const separators = cuT( " \t,;:?./-*+=(){}[]|" );
		auto textLength = text.size();

		for ( auto const & [src, repl] : m_defines )
		{
			auto index = String::npos;
			auto wordLength = src.size();

			do
			{
				index = text.find( src, index + 1 );

				if ( index != String::npos )
				{
					bool replace = true;

					if ( index > 0
						// Word doesn't start the line.
						&& separators.find( text[index - 1] ) == String::npos )
					{
						// Previous char isn't separator => ignore.
						replace = false;
					}

					if ( replace
						&& index + textLength > wordLength
						// Word doesn't end the line.
						&& separators.find( text[index + wordLength] ) == String::npos )
					{
						// Next char isn't separator => ignore.
						replace = false;
					}

					if ( replace )
					{
						text = text.replace( index, src.size(), repl );
						textLength = text.size();
					}
				}
			}
			while ( index != String::npos );
		}
	}
}
