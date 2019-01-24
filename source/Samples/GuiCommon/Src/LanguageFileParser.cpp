#include "LanguageFileParser.hpp"
#include "LanguageFileContext.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#include <Log/Logger.hpp>

using namespace castor;

namespace GuiCommon
{
	LanguageFileParser::LanguageFileParser( StcContext * p_pStcContext )
		: FileParser( uint32_t( LANGSection::eRoot ) )
		, m_pStcContext( p_pStcContext )
	{
	}

	LanguageFileParser::~LanguageFileParser()
	{
	}

	void LanguageFileParser::doInitialiseParser( Path const & path )
	{
		LanguageFileContextPtr context = std::make_shared< LanguageFileContext >( path );
		m_context = context;
		addParser( uint32_t( LANGSection::eRoot ), cuT( "language" ), Root_Language, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "pattern" ), Language_Pattern, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "fold_flags" ), Language_FoldFlags, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "keywords" ), Language_Keywords, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_name" ), Language_FontName, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_size" ), Language_FontSize, { makeParameter< ParameterType::eInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "style" ), Language_Style );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { makeParameter< ParameterType::eCheckedText>( context->mapTypes ) } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eKeywords ), cuT( "}" ), Keywords_End );
		context->currentLanguage.reset( new LanguageInfo );
	}

	void LanguageFileParser::doCleanupParser()
	{
		std::static_pointer_cast< LanguageFileContext >( m_context )->currentLanguage.reset();
	}

	bool LanguageFileParser::doDiscardParser( String const & p_line )
	{
		bool result = false;

		if ( m_context->m_sections.back() == uint32_t( LANGSection::eKeywords ) )
		{
			String strWords( p_line );
			string::replace( strWords, cuT( "\\" ), cuT( "" ) );
			StringArray arrayWords = string::split( string::trim( strWords ), cuT( "\t " ), 1000, false );
			LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( m_context );
			context->keywords.insert( context->keywords.end(), arrayWords.begin(), arrayWords.end() );
			result = true;
		}
		else
		{
			Logger::logWarning( cuT( "Parser not found @ line " ) + string::toString( m_context->m_line ) + cuT( " : " ) + p_line );
		}

		return result;
	}

	void LanguageFileParser::doValidate()
	{
		m_pStcContext->push_back( std::static_pointer_cast< LanguageFileContext >( m_context )->currentLanguage );
	}

	String LanguageFileParser::doGetSectionName( uint32_t p_section )
	{
		String result;

		switch ( LANGSection( p_section ) )
		{
		case LANGSection::eRoot:
			break;

		case LANGSection::eLanguage:
			result = cuT( "language" );
			break;

		case LANGSection::eKeywords:
			result = cuT( "keywords" );
			break;

		case LANGSection::eStyle:
			result = cuT( "style" );
			break;

		default:
			assert( false );
			break;
		}

		return result;
	}

	CU_ImplementAttributeParser( Root_Language )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			context->currentLanguage->setName( name );
		}
	}
	CU_EndAttributePush( LANGSection::eLanguage )

	CU_ImplementAttributeParser( Language_Pattern )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String strParams;
			p_params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				StringArray array = string::split( strParams, cuT( "\t ,;" ), 100, false );
				String strPatterns;
				std::for_each( array.begin(), array.end(), [&]( String const & p_strPattern )
					{
						if ( !strPatterns.empty() )
						{
							strPatterns += cuT( ";" );
						}

						strPatterns += p_strPattern;
					} );
				context->currentLanguage->setFilePattern( strPatterns );
			}
			else
			{
				CU_ParsingError( cuT( "Must be followed by a list of file patterns : pattern *.glsl [*.frag ...]" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Language_FoldFlags )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String strParams;
			p_params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				StringArray array = string::split( strParams, cuT( "\t ,;" ), 100, false );
				unsigned long ulFoldFlags = 0;
				std::for_each( array.begin(), array.end(), [&]( String const & p_strFoldFlag )
					{
						ulFoldFlags |= context->mapFoldFlags[p_strFoldFlag];
					} );
				context->currentLanguage->setFoldFlags( ulFoldFlags );
			}
			else
			{
				CU_ParsingError( cuT( "Must be followed by a list of parameters : fold_flags <param1> <param2> ..." ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Language_Keywords )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );
		context->keywords.clear();

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( context->index );
		}
	}
	CU_EndAttributePush( LANGSection::eKeywords )

	CU_ImplementAttributeParser( Language_Style )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );
	}
	CU_EndAttributePush( LANGSection::eStyle )

	CU_ImplementAttributeParser( Language_FontName )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			context->currentLanguage->setFontName( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Language_FontSize )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			int32_t size;
			p_params[0]->get( size );
			context->currentLanguage->setFontSize( size );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_Type )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t type;
			p_params[0]->get( type );
			context->currentStyle = &context->currentLanguage->getStyle( type );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_FgColour )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			context->currentStyle->foreground = wxColour( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_BgColour )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			context->currentStyle->background = wxColour( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_FontStyle )
	{
		String params;
		p_params[0]->get( params );

		if ( !params.empty() )
		{
			LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );
			StringArray styles = string::split( string::lowerCase( string::trim( params ) ), cuT( "\t " ), 10, false );
			int style = 0;

			for ( auto name : styles )
			{
				if ( name == cuT( "bold" ) )
				{
					style |= eSTC_STYLE_BOLD;
				}
				else if ( name == cuT( "italic" ) )
				{
					style |= eSTC_STYLE_ITALIC;
				}
				else if ( name == cuT( "underlined" ) )
				{
					style |= eSTC_STYLE_UNDERL;
				}
				else if ( name == cuT( "hidden" ) )
				{
					style |= eSTC_STYLE_HIDDEN;
				}
			}

			context->currentStyle->fontStyle = style;
		}
		else
		{
			CU_ParsingError( cuT( "Must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Keywords_End )
	{
		LanguageFileContextPtr context = std::static_pointer_cast< LanguageFileContext >( p_context );
		context->currentLanguage->setKeywords( context->index - 1, context->keywords );
	}
	CU_EndAttributePop()
}
