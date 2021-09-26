#include "GuiCommon/Shader/LanguageFileParser.hpp"

#include "GuiCommon/Shader/LanguageFileContext.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

using namespace castor;

namespace GuiCommon
{
	//*********************************************************************************************

	namespace
	{
		CU_ImplementAttributeParser( Root_Language )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.currentLanguage->name );
			}
		}
		CU_EndAttributePush( LANGSection::eLanguage )

		CU_ImplementAttributeParser( Language_Pattern )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

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
					langContext.currentLanguage->filePattern.clear();

					for ( auto & pattern : array )
					{
						if ( !langContext.currentLanguage->filePattern.empty() )
						{
							langContext.currentLanguage->filePattern += cuT( ";" );
						}

						langContext.currentLanguage->filePattern += pattern;
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
			auto & langContext = static_cast< LanguageFileContext & >( context );

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
					langContext.currentLanguage->foldFlags = 0u;

					for ( auto & flag : array )
					{
						langContext.currentLanguage->foldFlags |= langContext.mapFoldFlags[flag];
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
			auto & langContext = static_cast< LanguageFileContext & >( context );
			langContext.keywords.clear();

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.index );
			}
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_Style )
		{
		}
		CU_EndAttributePush( LANGSection::eStyle )

		CU_ImplementAttributeParser( Language_FontName )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.currentLanguage->fontName );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Language_FontSize )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.currentLanguage->fontSize );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Language_CLike )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.currentLanguage->isCLike );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_Type )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t type;
				params[0]->get( type );
				langContext.currentStyle = &langContext.currentLanguage->getStyle( int( type ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_FgColour )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				String name;
				params[0]->get( name );
				langContext.currentStyle->foreground = wxColour( name );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_BgColour )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				String name;
				params[0]->get( name );
				langContext.currentStyle->background = wxColour( name );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_FontStyle )
		{
			String langParams;
			params[0]->get( langParams );

			if ( !langParams.empty() )
			{
				auto & langContext = static_cast< LanguageFileContext & >( context );
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

				langContext.currentStyle->fontStyle = style;
			}
			else
			{
				CU_ParsingError( cuT( "Must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Keywords_End )
		{
			auto & langContext = static_cast< LanguageFileContext & >( context );
			langContext.currentLanguage->setKeywords( langContext.index - 1, langContext.keywords );
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	LanguageFileParser::LanguageFileParser( StcContext * stcContext )
		: FileParser{ castor::SectionId( LANGSection::eRoot ) }
		, m_stcContext{ stcContext }
	{
	}

	castor::FileParserContextSPtr LanguageFileParser::doInitialiseParser( Path const & path )
	{
		LanguageFileContextPtr result = std::make_shared< LanguageFileContext >( path );
		addParser( uint32_t( LANGSection::eRoot ), cuT( "language" ), Root_Language, { makeParameter< ParameterType::eName >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "pattern" ), Language_Pattern, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "fold_flags" ), Language_FoldFlags, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "keywords" ), Language_Keywords, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_name" ), Language_FontName, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "font_size" ), Language_FontSize, { makeParameter< ParameterType::eInt32 >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "is_c_like" ), Language_CLike, { makeParameter< ParameterType::eBool >() } );
		addParser( uint32_t( LANGSection::eLanguage ), cuT( "style" ), Language_Style );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { makeParameter< ParameterType::eCheckedText>( result->mapTypes ) } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { makeParameter< ParameterType::eText >() } );
		addParser( uint32_t( LANGSection::eKeywords ), cuT( "}" ), Keywords_End );
		result->currentLanguage.reset( new LanguageInfo );
		return std::static_pointer_cast< castor::FileParserContext >( result );
	}

	void LanguageFileParser::doCleanupParser( castor::PreprocessedFile & preprocessed )
	{
		static_cast< LanguageFileContext & >( preprocessed.getContext() ).currentLanguage.reset();
	}

	bool LanguageFileParser::doDiscardParser( castor::PreprocessedFile & preprocessed
		, castor::String const & line )
	{
		auto & context = preprocessed.getContext();

		if ( context.sections.back() != uint32_t( LANGSection::eKeywords ) )
		{
			return castor::FileParser::doDiscardParser( preprocessed, line );
		}

		String strWords( line );
		string::replace( strWords, cuT( "\\" ), cuT( "" ) );
		StringArray arrayWords = string::split( string::trim( strWords ), cuT( "\t " ), 1000, false );
		auto & langContext = static_cast< LanguageFileContext & >( context );
		langContext.keywords.insert( langContext.keywords.end(), arrayWords.begin(), arrayWords.end() );
		return true;
	}

	void LanguageFileParser::doValidate()
	{
		auto & context = static_cast< LanguageFileContext & >( getPreprocessed().getContext() );
		m_stcContext->push_back( context.currentLanguage );
	}

	castor::String LanguageFileParser::doGetSectionName( castor::SectionId section )const
	{
		String result;

		switch ( LANGSection( section ) )
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

	std::unique_ptr< castor::FileParser > LanguageFileParser::doCreateParser()const
	{
		return std::make_unique< LanguageFileParser >( m_stcContext );
	}

	//*********************************************************************************************
}
