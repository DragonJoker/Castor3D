#include "GuiCommon/Shader/LanguageFileParser.hpp"

#include "GuiCommon/Shader/LanguageFileContext.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Log/Logger.hpp>

namespace GuiCommon
{
	//*********************************************************************************************

	namespace
	{
		static castor::String const ParsersName{ "gc.language" };

		LanguageFileContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< LanguageFileContext * >( context.getUserContext( ParsersName ) );
		}

		CU_ImplementAttributeParser( Root_Language )
		{
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );
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

		CU_ImplementAttributeParser( Language_CPrimaryKeywords )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 1;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_CSecondaryKeywords )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 2;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_CDocumentationKeywords )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 3;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_CGlobalClasses )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 4;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_CPreprocessorDefinitions )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 5;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_CTaskErrorMarkers )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 6;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_AsmCpuInstructions )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 1;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_AsmRegisters )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 2;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_AsmDirectives )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 3;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Language_AsmExtInstructions )
		{
			auto & langContext = getParserContext( context );
			langContext.keywords.clear();
			langContext.index = 4;
		}
		CU_EndAttributePush( LANGSection::eKeywords )

		CU_ImplementAttributeParser( Keywords_Add )
		{
			auto & langContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				langContext.keywords.push_back( params[0]->get< castor::String >() );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Language_Style )
		{
		}
		CU_EndAttributePush( LANGSection::eStyle )

		CU_ImplementAttributeParser( Language_FontName )
		{
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( langContext.currentLanguage->isCLike );
				langContext.currentLanguage->updateStyles();
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_Type )
		{
			auto & langContext = getParserContext( context );

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
			auto & langContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				params[0]->get( name );
				langContext.currentStyle->foreground = wxColour( name );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_BgColour )
		{
			auto & langContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				params[0]->get( name );
				langContext.currentStyle->background = wxColour( name );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( Style_FontStyle )
		{
			castor::String langParams;
			params[0]->get( langParams );

			if ( !langParams.empty() )
			{
				auto & langContext = getParserContext( context );
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
			auto & langContext = getParserContext( context );
			langContext.currentLanguage->setKeywords( langContext.index - 1, langContext.keywords );
		}
		CU_EndAttributePop()

		castor::AttributeParsers registerParsers()
		{
			using namespace castor;
			static UInt32StrMap mapTypes;

			if ( mapTypes.empty() )
			{
				mapTypes[cuT( "default" )] = uint32_t( wxSTC_C_DEFAULT );
				mapTypes[cuT( "word1" )] = uint32_t( wxSTC_C_WORD );
				mapTypes[cuT( "word2" )] = uint32_t( wxSTC_C_WORD2 );
				mapTypes[cuT( "comment" )] = uint32_t( wxSTC_C_COMMENT );
				mapTypes[cuT( "comment_doc" )] = uint32_t( wxSTC_C_COMMENTDOC );
				mapTypes[cuT( "comment_line" )] = uint32_t( wxSTC_C_COMMENTLINE );
				mapTypes[cuT( "comment_line_doc" )] = uint32_t( wxSTC_C_COMMENTLINEDOC );
				mapTypes[cuT( "comment_line_doc" )] = uint32_t( wxSTC_C_COMMENTLINEDOC );
				mapTypes[cuT( "comment_doc_keyword" )] = uint32_t( wxSTC_C_COMMENTDOCKEYWORD );
				mapTypes[cuT( "comment_doc_keyword_error" )] = uint32_t( wxSTC_C_COMMENTDOCKEYWORDERROR );
				mapTypes[cuT( "char" )] = uint32_t( wxSTC_C_CHARACTER );
				mapTypes[cuT( "string" )] = uint32_t( wxSTC_C_STRING );
				mapTypes[cuT( "string_eol" )] = uint32_t( wxSTC_C_STRINGEOL );
				mapTypes[cuT( "operator" )] = uint32_t( wxSTC_C_OPERATOR );
				mapTypes[cuT( "identifier" )] = uint32_t( wxSTC_C_IDENTIFIER );
				mapTypes[cuT( "number" )] = uint32_t( wxSTC_C_NUMBER );
				mapTypes[cuT( "preprocessor" )] = uint32_t( wxSTC_C_PREPROCESSOR );

				mapTypes[cuT( "asm_comment" )] = uint32_t( wxSTC_A68K_COMMENT );
				mapTypes[cuT( "asm_dec_number" )] = uint32_t( wxSTC_A68K_NUMBER_DEC );
				mapTypes[cuT( "asm_bin_number" )] = uint32_t( wxSTC_A68K_NUMBER_BIN );
				mapTypes[cuT( "asm_hex_number" )] = uint32_t( wxSTC_A68K_NUMBER_HEX );
				mapTypes[cuT( "asm_string1" )] = uint32_t( wxSTC_A68K_STRING1 );
				mapTypes[cuT( "asm_operator" )] = uint32_t( wxSTC_A68K_OPERATOR );
				mapTypes[cuT( "asm_cpuinstruction" )] = uint32_t( wxSTC_A68K_CPUINSTRUCTION );
				mapTypes[cuT( "asm_extinstruction" )] = uint32_t( wxSTC_A68K_EXTINSTRUCTION );
				mapTypes[cuT( "asm_register" )] = uint32_t( wxSTC_A68K_REGISTER );
				mapTypes[cuT( "asm_directive" )] = uint32_t( wxSTC_A68K_DIRECTIVE );
				mapTypes[cuT( "asm_macroarg" )] = uint32_t( wxSTC_A68K_MACRO_ARG );
				mapTypes[cuT( "asm_label" )] = uint32_t( wxSTC_A68K_LABEL );
				mapTypes[cuT( "asm_string2" )] = uint32_t( wxSTC_A68K_STRING2 );
				mapTypes[cuT( "asm_identifier" )] = uint32_t( wxSTC_A68K_IDENTIFIER );
				mapTypes[cuT( "asm_macro_declaration" )] = uint32_t( wxSTC_A68K_MACRO_DECLARATION );
				mapTypes[cuT( "asm_comment_word" )] =  uint32_t( wxSTC_A68K_COMMENT_WORD );
				mapTypes[cuT( "asm_comment_special" )] =  uint32_t( wxSTC_A68K_COMMENT_SPECIAL );
				mapTypes[cuT( "asm_comment_doxygen" )] =  uint32_t( wxSTC_A68K_COMMENT_DOXYGEN );
			}


			castor::AttributeParsers result;

			addParser( result, uint32_t( LANGSection::eRoot ), cuT( "language" ), Root_Language, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "pattern" ), Language_Pattern, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "fold_flags" ), Language_FoldFlags, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "keywords" ), Language_Keywords, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_primarykeywords" ), Language_CPrimaryKeywords );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_secondarykeywords" ), Language_CSecondaryKeywords );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_documentationkeywords" ), Language_CDocumentationKeywords );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_globalclasses" ), Language_CGlobalClasses );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_preprocessordefinitions" ), Language_CPreprocessorDefinitions );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "c_taskerrormarkers" ), Language_CTaskErrorMarkers );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "asm_cpuinstructions" ), Language_AsmCpuInstructions );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "asm_registers" ), Language_AsmRegisters );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "asm_directives" ), Language_AsmDirectives );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "asm_extinstructions" ), Language_AsmExtInstructions );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "font_name" ), Language_FontName, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "font_size" ), Language_FontSize, { makeParameter< ParameterType::eInt32 >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "is_c_like" ), Language_CLike, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( LANGSection::eLanguage ), cuT( "style" ), Language_Style );
			addParser( result, uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { makeParameter< ParameterType::eCheckedText>( "StyleType", mapTypes ) } );
			addParser( result, uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eKeywords ), cuT( "keyword" ), Keywords_Add, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( LANGSection::eKeywords ), cuT( "}" ), Keywords_End );

			return result;
		}

		castor::StrUInt32Map registerSections()
		{
			return { { uint32_t( LANGSection::eRoot ), castor::String{} }
				, { uint32_t( LANGSection::eLanguage ), cuT( "language" ) }
				, { uint32_t( LANGSection::eKeywords ), cuT( "keywords" ) }
				, { uint32_t( LANGSection::eStyle ), cuT( "style" ) } };
		}

		void * createContext( castor::FileParserContext & context )
		{
			LanguageFileContext * userContext = new LanguageFileContext;
			userContext->currentLanguage.reset( new LanguageInfo );
			return userContext;
		}

		castor::AdditionalParsers createParsers()
		{
			return { registerParsers()
				, registerSections()
				, &createContext };
		}
	}

	//*********************************************************************************************

	LanguageFileParser::LanguageFileParser( StcContext * stcContext )
		: FileParser{ castor::SectionId( LANGSection::eRoot ) }
		, m_stcContext{ stcContext }
	{
		registerParsers( ParsersName, createParsers() );
	}

	void LanguageFileParser::doCleanupParser( castor::PreprocessedFile & preprocessed )
	{
		getParserContext( preprocessed.getContext() ).currentLanguage.reset();
	}

	void LanguageFileParser::doValidate( castor::PreprocessedFile & preprocessed )
	{
		auto & context = getParserContext( preprocessed.getContext() );
		m_stcContext->push_back( std::move( context.currentLanguage ) );
	}

	castor::String LanguageFileParser::doGetSectionName( castor::SectionId section )const
	{
		castor::String result;

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
