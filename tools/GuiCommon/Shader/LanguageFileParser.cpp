#include "GuiCommon/Shader/LanguageFileParser.hpp"

#include "GuiCommon/Shader/LanguageFileContext.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

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
		LanguageFileContextPtr langContext = std::make_shared< LanguageFileContext >( path );
		m_context = langContext;
		addParser( uint32_t( LANGSection::eRoot ), cuT( "language" ), Root_Language, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "pattern" ), Language_Pattern, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "fold_flags" ), Language_FoldFlags, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "keywords" ), Language_Keywords, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_name" ), Language_FontName, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_size" ), Language_FontSize, { makeParameter< ParameterType::eInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "is_c_like" ), Language_CLike, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "style" ), Language_Style );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { makeParameter< ParameterType::eCheckedText>( langContext->mapTypes ) } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eKeywords ), cuT( "}" ), Keywords_End );
		langContext->currentLanguage.reset( new LanguageInfo );
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
			LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( m_context );
			langContext->keywords.insert( langContext->keywords.end(), arrayWords.begin(), arrayWords.end() );
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
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( langContext->currentLanguage->name );
		}
	}
	CU_EndAttributePush( LANGSection::eLanguage )

	CU_ImplementAttributeParser( Language_Pattern )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String strParams;
			params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				StringArray array = string::split( strParams, cuT( "\t ,;" ), 100, false );
				langContext->currentLanguage->filePattern.clear();

				for ( auto & pattern : array )
				{
					if ( !langContext->currentLanguage->filePattern.empty() )
					{
						langContext->currentLanguage->filePattern += cuT( ";" );
					}

					langContext->currentLanguage->filePattern += pattern;
				}
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
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String strParams;
			params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				StringArray array = string::split( strParams, cuT( "\t ,;" ), 100, false );
				langContext->currentLanguage->foldFlags = 0u;

				for ( auto & flag : array )
				{
					langContext->currentLanguage->foldFlags |= langContext->mapFoldFlags[flag];
				}
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
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );
		langContext->keywords.clear();

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( langContext->index );
		}
	}
	CU_EndAttributePush( LANGSection::eKeywords )

	CU_ImplementAttributeParser( Language_Style )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );
	}
	CU_EndAttributePush( LANGSection::eStyle )

	CU_ImplementAttributeParser( Language_FontName )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( langContext->currentLanguage->fontName );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Language_FontSize )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( langContext->currentLanguage->fontSize );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Language_CLike )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( langContext->currentLanguage->isCLike );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_Type )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t type;
			params[0]->get( type );
			langContext->currentStyle = &langContext->currentLanguage->getStyle( type );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_FgColour )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			langContext->currentStyle->foreground = wxColour( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_BgColour )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			langContext->currentStyle->background = wxColour( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_FontStyle )
	{
		String langParams;
		params[0]->get( langParams );

		if ( !langParams.empty() )
		{
			LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );
			StringArray styles = string::split( string::lowerCase( string::trim( langParams ) ), cuT( "\t " ), 10, false );
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

			langContext->currentStyle->fontStyle = style;
		}
		else
		{
			CU_ParsingError( cuT( "Must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Keywords_End )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );
		langContext->currentLanguage->setKeywords( langContext->index - 1, langContext->keywords );
	}
	CU_EndAttributePop()
}
