#include "LanguageFileParser.hpp"
#include "LanguageFileContext.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#include <Logger.hpp>

using namespace Castor;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

namespace GuiCommon
{
	LanguageFileParser::LanguageFileParser( StcContext * p_pStcContext )
		:	FileParser( eSECTION_ROOT, eSECTION_COUNT )
		,	m_pStcContext( p_pStcContext )
	{
	}

	LanguageFileParser::~LanguageFileParser()
	{
	}

	void LanguageFileParser::DoInitialiseParser( TextFile & p_file )
	{
		LanguageFileContextPtr l_pContext = std::make_shared< LanguageFileContext >( &p_file );
		AddParser( eSECTION_ROOT,		cuT( "language"	),	Root_Language		, 1,	ePARAMETER_TYPE_NAME	);
		AddParser( eSECTION_LANGUAGE,	cuT( "pattern"	),	Language_Pattern	, 1,	ePARAMETER_TYPE_TEXT	);
		AddParser( eSECTION_LANGUAGE,	cuT( "lexer"	),	Language_Lexer		, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapLexers	);
		AddParser( eSECTION_LANGUAGE,	cuT( "fold_flags"	),	Language_FoldFlags	, 1,	ePARAMETER_TYPE_TEXT	);
		AddParser( eSECTION_LANGUAGE,	cuT( "section"	),	Language_Section	);
		AddParser( eSECTION_LANGUAGE,	cuT( "style"	),	Language_Style	);
		AddParser( eSECTION_STYLE,		cuT( "type"	),	Style_Type			, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapTypes	);
		AddParser( eSECTION_STYLE,		cuT( "fg_colour"	),	Style_FgColour		, 1,	ePARAMETER_TYPE_NAME	);
		AddParser( eSECTION_STYLE,		cuT( "bg_colour"	),	Style_BgColour		, 1,	ePARAMETER_TYPE_NAME	);
		AddParser( eSECTION_STYLE,		cuT( "font_name"	),	Style_FontName		, 1,	ePARAMETER_TYPE_TEXT	);
		AddParser( eSECTION_STYLE,		cuT( "font_style"	),	Style_FontStyle		, 1,	ePARAMETER_TYPE_TEXT	);
		AddParser( eSECTION_STYLE,		cuT( "font_size"	),	Style_FontSize		, 1,	ePARAMETER_TYPE_INT32	);
		AddParser( eSECTION_SECTION,	cuT( "type"	),	Section_Type		, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapTypes	);
		AddParser( eSECTION_SECTION,	cuT( "list"	),	Section_List	);
		AddParser( eSECTION_SECTION,	cuT( "}"	),	Section_End	);
		m_pParsingContext = l_pContext;
		l_pContext->pCurrentLanguage.reset( new LanguageInfo );
	}

	void LanguageFileParser::DoCleanupParser()
	{
		std::static_pointer_cast< LanguageFileContext >( m_pParsingContext )->pCurrentLanguage.reset();
	}

	void LanguageFileParser::DoDiscardParser( String const & p_strLine )
	{
		if ( m_pParsingContext->stackSections.top() == eSECTION_LIST )
		{
			String l_strWords( p_strLine );
			str_utils::replace( l_strWords, wxT( "\\" ), wxT( "" ) );
			StringArray l_arrayWords = str_utils::split( str_utils::trim( l_strWords ), wxT( "\t " ), 1000, false );
			LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( m_pParsingContext );
			l_pContext->arrayWords.insert( l_pContext->arrayWords.end(), l_arrayWords.begin(), l_arrayWords.end() );
		}
		else
		{
			Logger::LogWarning( cuT( "Parser not found @ line " ) + str_utils::to_string( m_pParsingContext->ui64Line ) + cuT( " : " ) + p_strLine );
		}
	}

	void LanguageFileParser::DoValidate()
	{
		m_pStcContext->AddLanguage( std::static_pointer_cast< LanguageFileContext >( m_pParsingContext )->pCurrentLanguage );
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Root_Language )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentLanguage->SetName( l_strName );
}
END_ATTRIBUTE_PUSH( eSECTION_LANGUAGE )

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Pattern )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if ( !l_strParams.empty() )
	{
		StringArray l_array = str_utils::split( l_strParams, cuT( "\t ,;" ), 100, false );
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
		PARSING_ERROR( cuT( "directive <pattern> must be followed by a list of file patterns : pattern *.glsl [*.frag ...]" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Lexer )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiLexer;
	p_arrayParams[0]->Get( l_uiLexer );
	l_pContext->pCurrentLanguage->SetLexer( eSTC_LEX( l_uiLexer ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_FoldFlags )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if ( !l_strParams.empty() )
	{
		StringArray l_array = str_utils::split( l_strParams, cuT( "\t ,;" ), 100, false );
		unsigned long l_ulFoldFlags = 0;
		std::for_each( l_array.begin(), l_array.end(), [&]( String const & p_strFoldFlag )
		{
			l_ulFoldFlags |= l_pContext->mapFoldFlags[p_strFoldFlag];
		} );
		l_pContext->pCurrentLanguage->SetFoldFlags( l_ulFoldFlags );
	}
	else
	{
		PARSING_ERROR( cuT( "directive <fold_flags> must be followed by a list of parameters : fold_flags <param1> <param2> ..." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Section )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	l_pContext->arrayWords.clear();
	l_pContext->eStyle = eSTC_TYPE_COUNT;
	l_pContext->eType = eSTC_TYPE_COUNT;
}
END_ATTRIBUTE_PUSH( eSECTION_SECTION )

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Style )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	l_pContext->arrayWords.clear();
	l_pContext->eStyle = eSTC_TYPE_COUNT;
	l_pContext->eType = eSTC_TYPE_COUNT;
}
END_ATTRIBUTE_PUSH( eSECTION_STYLE )

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_Type )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );
	l_pContext->pCurrentStyle = l_pContext->pCurrentLanguage->GetStyle( eSTC_TYPE( l_uiType ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FgColour )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetForeground( l_strName );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_BgColour )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetBackground( l_strName );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontName )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetFontName( l_strName );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontStyle )
{
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if ( !l_strParams.empty() )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
		StringArray l_arrayStyles = str_utils::split( str_utils::lower_case( str_utils::trim( l_strParams ) ), wxT( "\t " ), 10, false );
		int l_iStyle = 0;
		std::for_each( l_arrayStyles.begin(), l_arrayStyles.end(), [&]( String const & p_strStyle )
		{
			if ( p_strStyle == wxT( "bold" ) )
			{
				l_iStyle |= eSTC_STYLE_BOLD;
			}
			else if ( p_strStyle == wxT( "italic" ) )
			{
				l_iStyle |= eSTC_STYLE_ITALIC;
			}
			else if ( p_strStyle == wxT( "underlined" ) )
			{
				l_iStyle |= eSTC_STYLE_UNDERL;
			}
			else if ( p_strStyle == wxT( "hidden" ) )
			{
				l_iStyle |= eSTC_STYLE_HIDDEN;
			}
		} );
		l_pContext->pCurrentStyle->SetFontStyle( l_iStyle );
	}
	else
	{
		PARSING_ERROR( cuT( "directive <style::font_style> must be followed by a list of parameters : font_style <param1> <param2> ..." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontSize )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	int32_t l_iSize;
	p_arrayParams[0]->Get( l_iSize );
	l_pContext->pCurrentStyle->SetFontSize( l_iSize );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_Type )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );
	l_pContext->eType = eSTC_TYPE( l_uiType );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_End )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );

	if ( l_pContext->eType != eSTC_TYPE_COUNT )
	{
		l_pContext->pCurrentLanguage->SetWords( l_pContext->eType, l_pContext->arrayWords );
	}
	else
	{
		PARSING_ERROR( cuT( "directive <section> must contain a directive <type>" ) );
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_List )
{
}
END_ATTRIBUTE_PUSH( eSECTION_LIST )

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
