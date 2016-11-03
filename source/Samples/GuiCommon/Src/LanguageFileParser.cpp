#include "LanguageFileParser.hpp"
#include "LanguageFileContext.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace GuiCommon
{
	LanguageFileParser::LanguageFileParser( StcContext * p_pStcContext )
		: FileParser( eSECTION_ROOT )
		, m_pStcContext( p_pStcContext )
	{
	}

	LanguageFileParser::~LanguageFileParser()
	{
	}

	void LanguageFileParser::DoInitialiseParser( TextFile & p_file )
	{
		LanguageFileContextPtr l_pContext = std::make_shared< LanguageFileContext >( &p_file );
		m_context = l_pContext;
		AddParser( eSECTION_ROOT, cuT( "language" ), Root_Language, { MakeParameter< ParameterType::Name >() } );
		AddParser( eSECTION_LANGUAGE, cuT( "pattern" ), Language_Pattern, { MakeParameter< ParameterType::Text >() } );
		AddParser( eSECTION_LANGUAGE, cuT( "lexer" ), Language_Lexer, { MakeParameter< ParameterType::CheckedText>( l_pContext->mapLexers ) } );
		AddParser( eSECTION_LANGUAGE, cuT( "fold_flags" ), Language_FoldFlags, { MakeParameter< ParameterType::Text >() } );
		AddParser( eSECTION_LANGUAGE, cuT( "section" ), Language_Section );
		AddParser( eSECTION_LANGUAGE, cuT( "style" ), Language_Style );
		AddParser( eSECTION_STYLE, cuT( "type" ), Style_Type, { MakeParameter< ParameterType::CheckedText>( l_pContext->mapTypes ) } );
		AddParser( eSECTION_STYLE, cuT( "fg_colour" ), Style_FgColour, { MakeParameter< ParameterType::Colour >() } );
		AddParser( eSECTION_STYLE, cuT( "bg_colour" ), Style_BgColour, { MakeParameter< ParameterType::Colour >() } );
		AddParser( eSECTION_STYLE, cuT( "font_name" ), Style_FontName, { MakeParameter< ParameterType::Text >() } );
		AddParser( eSECTION_STYLE, cuT( "font_style" ), Style_FontStyle, { MakeParameter< ParameterType::Text >() } );
		AddParser( eSECTION_STYLE, cuT( "font_size" ), Style_FontSize, { MakeParameter< ParameterType::Int32 >() } );
		AddParser( eSECTION_SECTION, cuT( "type" ), Section_Type, { MakeParameter< ParameterType::CheckedText >( l_pContext->mapTypes ) } );
		AddParser( eSECTION_SECTION, cuT( "list" ), Section_List );
		AddParser( eSECTION_SECTION, cuT( "}" ), Section_End );
		l_pContext->pCurrentLanguage.reset( new LanguageInfo );
	}

	void LanguageFileParser::DoCleanupParser()
	{
		std::static_pointer_cast< LanguageFileContext >( m_context )->pCurrentLanguage.reset();
	}

	bool LanguageFileParser::DoDiscardParser( String const & p_line )
	{
		bool l_return = false;

		if ( m_context->m_sections.back() == eSECTION_LIST )
		{
			String l_strWords( p_line );
			string::replace( l_strWords, cuT( "\\" ), cuT( "" ) );
			StringArray l_arrayWords = string::split( string::trim( l_strWords ), cuT( "\t " ), 1000, false );
			LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( m_context );
			l_pContext->arrayWords.insert( l_pContext->arrayWords.end(), l_arrayWords.begin(), l_arrayWords.end() );
			l_return = true;
		}
		else
		{
			Logger::LogWarning( cuT( "Parser not found @ line " ) + string::to_string( m_context->m_line ) + cuT( " : " ) + p_line );
		}

		return l_return;
	}

	void LanguageFileParser::DoValidate()
	{
		m_pStcContext->AddLanguage( std::static_pointer_cast< LanguageFileContext >( m_context )->pCurrentLanguage );
	}

	String LanguageFileParser::DoGetSectionName( uint32_t p_section )
	{
		String l_return;

		switch ( p_section )
		{
		case eSECTION_ROOT:
			break;

		case eSECTION_LANGUAGE:
			l_return = cuT( "language" );
			break;

		case eSECTION_SECTION:
			l_return = cuT( "section" );
			break;

		case eSECTION_STYLE:
			l_return = cuT( "style" );
			break;

		case eSECTION_LIST:
			l_return = cuT( "list" );
			break;

		default:
			assert( false );
			break;
		}

		return l_return;
	}

	IMPLEMENT_ATTRIBUTE_PARSER( Root_Language )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );
		l_pContext->pCurrentLanguage->SetName( l_name );
	}
	END_ATTRIBUTE_PUSH( eSECTION_LANGUAGE )

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Pattern )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_strParams;
		p_params[0]->Get( l_strParams );

		if ( !l_strParams.empty() )
		{
			StringArray l_array = string::split( l_strParams, cuT( "\t ,;" ), 100, false );
			String l_strPatterns;
			std::for_each( l_array.begin(), l_array.end(), [&]( String const & p_strPattern )
			{
				if ( !l_strPatterns.empty() )
				{
					l_strPatterns += cuT( ";" );
				}

				l_strPatterns += p_strPattern;
			} );
			l_pContext->pCurrentLanguage->SetFilePattern( l_strPatterns );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of file patterns : pattern *.glsl [*.frag ...]" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Lexer )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t l_uiLexer;
		p_params[0]->Get( l_uiLexer );
		l_pContext->pCurrentLanguage->SetLexer( eSTC_LEX( l_uiLexer ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_FoldFlags )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_strParams;
		p_params[0]->Get( l_strParams );

		if ( !l_strParams.empty() )
		{
			StringArray l_array = string::split( l_strParams, cuT( "\t ,;" ), 100, false );
			unsigned long l_ulFoldFlags = 0;
			std::for_each( l_array.begin(), l_array.end(), [&]( String const & p_strFoldFlag )
			{
				l_ulFoldFlags |= l_pContext->mapFoldFlags[p_strFoldFlag];
			} );
			l_pContext->pCurrentLanguage->SetFoldFlags( l_ulFoldFlags );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of parameters : fold_flags <param1> <param2> ..." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Section )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		l_pContext->arrayWords.clear();
		l_pContext->eStyle = eSTC_TYPE_COUNT;
		l_pContext->eType = eSTC_TYPE_COUNT;
	}
	END_ATTRIBUTE_PUSH( eSECTION_SECTION )

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Style )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		l_pContext->arrayWords.clear();
		l_pContext->eStyle = eSTC_TYPE_COUNT;
		l_pContext->eType = eSTC_TYPE_COUNT;
	}
	END_ATTRIBUTE_PUSH( eSECTION_STYLE )

	IMPLEMENT_ATTRIBUTE_PARSER( Style_Type )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t l_uiType;
		p_params[0]->Get( l_uiType );
		l_pContext->pCurrentStyle = l_pContext->pCurrentLanguage->GetStyle( eSTC_TYPE( l_uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FgColour )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );
		l_pContext->pCurrentStyle->SetForeground( l_name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_BgColour )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );
		l_pContext->pCurrentStyle->SetBackground( l_name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontName )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );
		l_pContext->pCurrentStyle->SetFontName( l_name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontStyle )
	{
		String l_strParams;
		p_params[0]->Get( l_strParams );

		if ( !l_strParams.empty() )
		{
			LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
			StringArray l_arrayStyles = string::split( string::lower_case( string::trim( l_strParams ) ), cuT( "\t " ), 10, false );
			int l_iStyle = 0;
			std::for_each( l_arrayStyles.begin(), l_arrayStyles.end(), [&]( String const & p_strStyle )
			{
				if ( p_strStyle == cuT( "bold" ) )
				{
					l_iStyle |= eSTC_STYLE_BOLD;
				}
				else if ( p_strStyle == cuT( "italic" ) )
				{
					l_iStyle |= eSTC_STYLE_ITALIC;
				}
				else if ( p_strStyle == cuT( "underlined" ) )
				{
					l_iStyle |= eSTC_STYLE_UNDERL;
				}
				else if ( p_strStyle == cuT( "hidden" ) )
				{
					l_iStyle |= eSTC_STYLE_HIDDEN;
				}
			} );
			l_pContext->pCurrentStyle->SetFontStyle( l_iStyle );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontSize )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		int32_t l_size;
		p_params[0]->Get( l_size );
		l_pContext->pCurrentStyle->SetFontSize( l_size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Section_Type )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t l_uiType;
		p_params[0]->Get( l_uiType );
		l_pContext->eType = eSTC_TYPE( l_uiType );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Section_End )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( l_pContext->eType != eSTC_TYPE_COUNT )
		{
			l_pContext->pCurrentLanguage->SetWords( l_pContext->eType, l_pContext->arrayWords );
		}
		else
		{
			PARSING_ERROR( cuT( "Must contain a directive <type>" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Section_List )
	{
	}
	END_ATTRIBUTE_PUSH( eSECTION_LIST )
}
