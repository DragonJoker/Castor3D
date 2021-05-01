#include "Aria/Editor/LanguageFileParser.hpp"

#include "Aria/Prerequisites.hpp"
#include "Aria/Editor/LanguageFileContext.hpp"
#include "Aria/Editor/LanguageInfo.hpp"
#include "Aria/Editor/StyleInfo.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

namespace aria
{
	LanguageFileParser::LanguageFileParser( StcContext & stcContext )
		: FileParser{ uint32_t( LANGSection::eRoot ) }
		, m_stcContext{ stcContext }
	{
	}

	LanguageFileParser::~LanguageFileParser()
	{
	}

	void LanguageFileParser::doInitialiseParser( castor::Path const & path )
	{
		auto langContext = std::make_shared< LanguageFileContext >( path, m_stcContext.language );
		m_context = langContext;
		addParser( uint32_t( LANGSection::eRoot ), cuT( "pattern" ), Language_Pattern, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "fold_flags" ), Language_FoldFlags, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "keywords" ), Language_Keywords, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "font_name" ), Language_FontName, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "font_size" ), Language_FontSize, { castor::makeParameter< castor::ParameterType::eInt32 >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "is_c_like" ), Language_CLike, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "style" ), Language_Style );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { castor::makeParameter< castor::ParameterType::eCheckedText>( langContext->mapTypes ) } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { castor::makeParameter< castor::ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eKeywords ), cuT( "}" ), Keywords_End );
	}

	void LanguageFileParser::doCleanupParser()
	{
	}

	bool LanguageFileParser::doDiscardParser( castor::String const & p_line )
	{
		bool result = false;

		if ( m_context->m_sections.back() == uint32_t( LANGSection::eKeywords ) )
		{
			castor::String strWords( p_line );
			castor::string::replace( strWords, cuT( "\\" ), cuT( "" ) );
			castor::StringArray arrayWords = castor::string::split( castor::string::trim( strWords ), cuT( "\t " ), 1000, false );
			LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( m_context );

			for ( auto & keyword : arrayWords )
			{
				langContext->keywords.push_back( makeWxString( keyword ) );
			}

			result = true;
		}
		else
		{
			castor::Logger::logWarning( cuT( "Parser not found @ line " ) + castor::string::toString( m_context->m_line ) + cuT( " : " ) + p_line );
		}

		return result;
	}

	void LanguageFileParser::doValidate()
	{
	}

	castor::String LanguageFileParser::doGetSectionName( uint32_t p_section )
	{
		castor::String result;

		switch ( LANGSection( p_section ) )
		{
		case LANGSection::eRoot:
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

	CU_ImplementAttributeParser( Language_Pattern )
	{
		LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String strParams;
			params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				auto array = castor::string::split( strParams, cuT( "\t ,;" ), 100, false );
				langContext->currentLanguage.filePattern.clear();

				for ( auto & pattern : array )
				{
					if ( !langContext->currentLanguage.filePattern.empty() )
					{
						langContext->currentLanguage.filePattern += cuT( ";" );
					}

					langContext->currentLanguage.filePattern += pattern;
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
			castor::String strParams;
			params[0]->get( strParams );

			if ( !strParams.empty() )
			{
				auto array = castor::string::split( strParams, cuT( "\t ,;" ), 100, false );
				langContext->currentLanguage.foldFlags = 0u;

				for ( auto & flag : array )
				{
					langContext->currentLanguage.foldFlags |= langContext->mapFoldFlags[flag];
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
			castor::String name;
			params[0]->get( name );
			langContext->currentLanguage.fontName = makeWxString( name );
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
			params[0]->get( langContext->currentLanguage.fontSize );
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
			params[0]->get( langContext->currentLanguage.isCLike );
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
			langContext->currentStyle = &langContext->currentLanguage.getStyle( type );
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
			castor::String name;
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
			castor::String name;
			params[0]->get( name );
			langContext->currentStyle->background = wxColour( name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( Style_FontStyle )
	{
		castor::String langParams;
		params[0]->get( langParams );

		if ( !langParams.empty() )
		{
			LanguageFileContextPtr langContext = std::static_pointer_cast< LanguageFileContext >( context );
			auto styles = castor::string::split( castor::string::lowerCase( castor::string::trim( langParams ) ), cuT( "\t " ), 10, false );
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
		langContext->currentLanguage.setKeywords( langContext->index - 1, langContext->keywords );
	}
	CU_EndAttributePop()
}
