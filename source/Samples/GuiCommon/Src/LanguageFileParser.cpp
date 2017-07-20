#include "LanguageFileParser.hpp"
#include "LanguageFileContext.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

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

	void LanguageFileParser::DoInitialiseParser( TextFile & p_file )
	{
		LanguageFileContextPtr pContext = std::make_shared< LanguageFileContext >( &p_file );
		m_context = pContext;
		AddParser( uint32_t( LANGSection::eRoot ), cuT( "language" ), Root_Language, { MakeParameter< ParameterType::eName >() } );
		AddParser( uint32_t( LANGSection::eLanguage ), cuT( "pattern" ), Language_Pattern, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eLanguage ), cuT( "lexer" ), Language_Lexer, { MakeParameter< ParameterType::eCheckedText>( pContext->mapLexers ) } );
		AddParser( uint32_t( LANGSection::eLanguage ), cuT( "fold_flags" ), Language_FoldFlags, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eLanguage ), cuT( "section" ), Language_Section );
		AddParser( uint32_t( LANGSection::eLanguage ), cuT( "style" ), Language_Style );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "type" ), Style_Type, { MakeParameter< ParameterType::eCheckedText>( pContext->mapTypes ) } );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "fg_colour" ), Style_FgColour, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "bg_colour" ), Style_BgColour, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "font_name" ), Style_FontName, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "font_style" ), Style_FontStyle, { MakeParameter< ParameterType::eText >() } );
		AddParser( uint32_t( LANGSection::eStyle ), cuT( "font_size" ), Style_FontSize, { MakeParameter< ParameterType::eInt32 >() } );
		AddParser( uint32_t( LANGSection::eSection ), cuT( "type" ), Section_Type, { MakeParameter< ParameterType::eCheckedText >( pContext->mapTypes ) } );
		AddParser( uint32_t( LANGSection::eSection ), cuT( "list" ), Section_List );
		AddParser( uint32_t( LANGSection::eSection ), cuT( "}" ), Section_End );
		pContext->pCurrentLanguage.reset( new LanguageInfo );
	}

	void LanguageFileParser::DoCleanupParser()
	{
		std::static_pointer_cast< LanguageFileContext >( m_context )->pCurrentLanguage.reset();
	}

	bool LanguageFileParser::DoDiscardParser( String const & p_line )
	{
		bool result = false;

		if ( m_context->m_sections.back() == uint32_t( LANGSection::eList ) )
		{
			String strWords( p_line );
			string::replace( strWords, cuT( "\\" ), cuT( "" ) );
			StringArray arrayWords = string::split( string::trim( strWords ), cuT( "\t " ), 1000, false );
			LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( m_context );
			pContext->arrayWords.insert( pContext->arrayWords.end(), arrayWords.begin(), arrayWords.end() );
			result = true;
		}
		else
		{
			Logger::LogWarning( cuT( "Parser not found @ line " ) + string::to_string( m_context->m_line ) + cuT( " : " ) + p_line );
		}

		return result;
	}

	void LanguageFileParser::DoValidate()
	{
		m_pStcContext->AddLanguage( std::static_pointer_cast< LanguageFileContext >( m_context )->pCurrentLanguage );
	}

	String LanguageFileParser::DoGetSectionName( uint32_t p_section )
	{
		String result;

		switch ( LANGSection( p_section ) )
		{
		case LANGSection::eRoot:
			break;

		case LANGSection::eLanguage:
			result = cuT( "language" );
			break;

		case LANGSection::eSection:
			result = cuT( "section" );
			break;

		case LANGSection::eStyle:
			result = cuT( "style" );
			break;

		case LANGSection::eList:
			result = cuT( "list" );
			break;

		default:
			assert( false );
			break;
		}

		return result;
	}

	IMPLEMENT_ATTRIBUTE_PARSER( Root_Language )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String name;
		p_params[0]->Get( name );
		pContext->pCurrentLanguage->SetName( name );
	}
	END_ATTRIBUTE_PUSH( LANGSection::eLanguage )

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Pattern )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String strParams;
		p_params[0]->Get( strParams );

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
			pContext->pCurrentLanguage->SetFilePattern( strPatterns );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of file patterns : pattern *.glsl [*.frag ...]" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Lexer )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t uiLexer;
		p_params[0]->Get( uiLexer );
		pContext->pCurrentLanguage->SetLexer( eSTC_LEX( uiLexer ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_FoldFlags )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String strParams;
		p_params[0]->Get( strParams );

		if ( !strParams.empty() )
		{
			StringArray array = string::split( strParams, cuT( "\t ,;" ), 100, false );
			unsigned long ulFoldFlags = 0;
			std::for_each( array.begin(), array.end(), [&]( String const & p_strFoldFlag )
			{
				ulFoldFlags |= pContext->mapFoldFlags[p_strFoldFlag];
			} );
			pContext->pCurrentLanguage->SetFoldFlags( ulFoldFlags );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of parameters : fold_flags <param1> <param2> ..." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Section )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		pContext->arrayWords.clear();
		pContext->eStyle = eSTC_TYPE_COUNT;
		pContext->eType = eSTC_TYPE_COUNT;
	}
	END_ATTRIBUTE_PUSH( LANGSection::eSection )

	IMPLEMENT_ATTRIBUTE_PARSER( Language_Style )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		pContext->arrayWords.clear();
		pContext->eStyle = eSTC_TYPE_COUNT;
		pContext->eType = eSTC_TYPE_COUNT;
	}
	END_ATTRIBUTE_PUSH( LANGSection::eStyle )

	IMPLEMENT_ATTRIBUTE_PARSER( Style_Type )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t uiType;
		p_params[0]->Get( uiType );
		pContext->pCurrentStyle = pContext->pCurrentLanguage->GetStyle( eSTC_TYPE( uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FgColour )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String name;
		p_params[0]->Get( name );
		pContext->pCurrentStyle->SetForeground( name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_BgColour )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String name;
		p_params[0]->Get( name );
		pContext->pCurrentStyle->SetBackground( name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontName )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		String name;
		p_params[0]->Get( name );
		pContext->pCurrentStyle->SetFontName( name );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontStyle )
	{
		String strParams;
		p_params[0]->Get( strParams );

		if ( !strParams.empty() )
		{
			LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
			StringArray arrayStyles = string::split( string::lower_case( string::trim( strParams ) ), cuT( "\t " ), 10, false );
			int iStyle = 0;
			std::for_each( arrayStyles.begin(), arrayStyles.end(), [&]( String const & p_strStyle )
			{
				if ( p_strStyle == cuT( "bold" ) )
				{
					iStyle |= eSTC_STYLE_BOLD;
				}
				else if ( p_strStyle == cuT( "italic" ) )
				{
					iStyle |= eSTC_STYLE_ITALIC;
				}
				else if ( p_strStyle == cuT( "underlined" ) )
				{
					iStyle |= eSTC_STYLE_UNDERL;
				}
				else if ( p_strStyle == cuT( "hidden" ) )
				{
					iStyle |= eSTC_STYLE_HIDDEN;
				}
			} );
			pContext->pCurrentStyle->SetFontStyle( iStyle );
		}
		else
		{
			PARSING_ERROR( cuT( "Must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Style_FontSize )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		int32_t size;
		p_params[0]->Get( size );
		pContext->pCurrentStyle->SetFontSize( size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Section_Type )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );
		uint32_t uiType;
		p_params[0]->Get( uiType );
		pContext->eType = eSTC_TYPE( uiType );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Section_End )
	{
		LanguageFileContextPtr pContext = std::static_pointer_cast< LanguageFileContext >( p_context );

		if ( pContext->eType != eSTC_TYPE_COUNT )
		{
			pContext->pCurrentLanguage->SetWords( pContext->eType, pContext->arrayWords );
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
	END_ATTRIBUTE_PUSH( LANGSection::eList )
}
