#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/LanguageParser.hpp"

using namespace GuiCommon;
using namespace Castor;

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

//*************************************************************************************************

LanguageFileContext :: LanguageFileContext( TextFile * p_pFile )
	:	FileParserContext( p_pFile )
{
	mapTypes[cuT( "default"			)] = eSTC_TYPE_DEFAULT;
	mapTypes[cuT( "word1"			)] = eSTC_TYPE_WORD1;
	mapTypes[cuT( "word2"			)] = eSTC_TYPE_WORD2;
	mapTypes[cuT( "word3"			)] = eSTC_TYPE_WORD3;
	mapTypes[cuT( "word4"			)] = eSTC_TYPE_WORD4;
	mapTypes[cuT( "word5"			)] = eSTC_TYPE_WORD5;
	mapTypes[cuT( "word6"			)] = eSTC_TYPE_WORD6;
	mapTypes[cuT( "comment"			)] = eSTC_TYPE_COMMENT;
	mapTypes[cuT( "comment_doc"		)] = eSTC_TYPE_COMMENT_DOC;
	mapTypes[cuT( "comment_line"	)] = eSTC_TYPE_COMMENT_LINE;
	mapTypes[cuT( "comment_special"	)] = eSTC_TYPE_COMMENT_SPECIAL;
	mapTypes[cuT( "char"			)] = eSTC_TYPE_CHARACTER;
	mapTypes[cuT( "char_eol"		)] = eSTC_TYPE_CHARACTER_EOL;
	mapTypes[cuT( "string"			)] = eSTC_TYPE_STRING;
	mapTypes[cuT( "string_eol"		)] = eSTC_TYPE_STRING_EOL;
	mapTypes[cuT( "delimiter"		)] = eSTC_TYPE_DELIMITER;
	mapTypes[cuT( "punctuation"		)] = eSTC_TYPE_PUNCTUATION;
	mapTypes[cuT( "operator"		)] = eSTC_TYPE_OPERATOR;
	mapTypes[cuT( "brace"			)] = eSTC_TYPE_BRACE;
	mapTypes[cuT( "command"			)] = eSTC_TYPE_COMMAND;
	mapTypes[cuT( "identifier"		)] = eSTC_TYPE_IDENTIFIER;
	mapTypes[cuT( "label"			)] = eSTC_TYPE_LABEL;
	mapTypes[cuT( "number"			)] = eSTC_TYPE_NUMBER;
	mapTypes[cuT( "parameter"		)] = eSTC_TYPE_PARAMETER;
	mapTypes[cuT( "regex"			)] = eSTC_TYPE_REGEX;
	mapTypes[cuT( "uuid"			)] = eSTC_TYPE_UUID;
	mapTypes[cuT( "value"			)] = eSTC_TYPE_VALUE;
	mapTypes[cuT( "preprocessor"	)] = eSTC_TYPE_PREPROCESSOR;
	mapTypes[cuT( "script"			)] = eSTC_TYPE_SCRIPT;
	mapTypes[cuT( "error"			)] = eSTC_TYPE_ERROR;
	mapTypes[cuT( "undefined"		)] = eSTC_TYPE_UNDEFINED;

	mapLexers[cuT( "Container"		)] =	eSTC_LEX_CONTAINER	;
	mapLexers[cuT( "Null"			)] =	eSTC_LEX_NULL		;
	mapLexers[cuT( "Python"			)] =	eSTC_LEX_PYTHON		;
	mapLexers[cuT( "C/C++"			)] =	eSTC_LEX_CPP		;
	mapLexers[cuT( "HTML"			)] =	eSTC_LEX_HTML		;
	mapLexers[cuT( "XML"			)] =	eSTC_LEX_XML		;
	mapLexers[cuT( "Perl"			)] =	eSTC_LEX_PERL		;
	mapLexers[cuT( "SQL"			)] =	eSTC_LEX_SQL		;
	mapLexers[cuT( "VB"				)] =	eSTC_LEX_VB			;
	mapLexers[cuT( "Properties"		)] =	eSTC_LEX_PROPERTIES	;
	mapLexers[cuT( "ErrorList"		)] =	eSTC_LEX_ERRORLIST	;
	mapLexers[cuT( "Makefile"		)] =	eSTC_LEX_MAKEFILE	;
	mapLexers[cuT( "Batch"			)] =	eSTC_LEX_BATCH		;
	mapLexers[cuT( "XCode"			)] =	eSTC_LEX_XCODE		;
	mapLexers[cuT( "Latex"			)] =	eSTC_LEX_LATEX		;
	mapLexers[cuT( "LUA"			)] =	eSTC_LEX_LUA		;
	mapLexers[cuT( "Diff"			)] =	eSTC_LEX_DIFF		;
	mapLexers[cuT( "Conf"			)] =	eSTC_LEX_CONF		;
	mapLexers[cuT( "Pascal"			)] =	eSTC_LEX_PASCAL		;
	mapLexers[cuT( "Ave"			)] =	eSTC_LEX_AVE		;
	mapLexers[cuT( "Ada"			)] =	eSTC_LEX_ADA		;
	mapLexers[cuT( "Lisp"			)] =	eSTC_LEX_LISP		;
	mapLexers[cuT( "Ruby"			)] =	eSTC_LEX_RUBY		;
	mapLexers[cuT( "Eiffel"			)] =	eSTC_LEX_EIFFEL		;
	mapLexers[cuT( "TCL"			)] =	eSTC_LEX_TCL		;
	mapLexers[cuT( "NnCrontTab"		)] =	eSTC_LEX_NNCRONTAB	;
	mapLexers[cuT( "Bullant"		)] =	eSTC_LEX_BULLANT	;
	mapLexers[cuT( "VBScript"		)] =	eSTC_LEX_VBSCRIPT	;
	mapLexers[cuT( "BAAN"			)] =	eSTC_LEX_BAAN		;
	mapLexers[cuT( "Matlab"			)] =	eSTC_LEX_MATLAB		;
	mapLexers[cuT( "Scriptol"		)] =	eSTC_LEX_SCRIPTOL	;
	mapLexers[cuT( "Asm"			)] =	eSTC_LEX_ASM		;
	mapLexers[cuT( "Fortran"		)] =	eSTC_LEX_FORTRAN	;
	mapLexers[cuT( "F77"			)] =	eSTC_LEX_F77		;
	mapLexers[cuT( "CSS"			)] =	eSTC_LEX_CSS		;
	mapLexers[cuT( "POV"			)] =	eSTC_LEX_POV		;
	mapLexers[cuT( "Lout"			)] =	eSTC_LEX_LOUT		;
	mapLexers[cuT( "EScript"		)] =	eSTC_LEX_ESCRIPT	;
	mapLexers[cuT( "PS"				)] =	eSTC_LEX_PS			;
	mapLexers[cuT( "NSIS"			)] =	eSTC_LEX_NSIS		;
	mapLexers[cuT( "MMixal"			)] =	eSTC_LEX_MMIXAL		;
	mapLexers[cuT( "CLW"			)] =	eSTC_LEX_CLW		;
	mapLexers[cuT( "LOT"			)] =	eSTC_LEX_LOT		;
	mapLexers[cuT( "YAML"			)] =	eSTC_LEX_YAML		;
	mapLexers[cuT( "Tex"			)] =	eSTC_LEX_TEX		;
	mapLexers[cuT( "Metapost"		)] =	eSTC_LEX_METAPOST	;
	mapLexers[cuT( "PowerBasic"		)] =	eSTC_LEX_POWERBASIC	;
	mapLexers[cuT( "Forth"			)] =	eSTC_LEX_FORTH		;
	mapLexers[cuT( "Erlang"			)] =	eSTC_LEX_ERLANG		;
	mapLexers[cuT( "Octave"			)] =	eSTC_LEX_OCTAVE		;
	mapLexers[cuT( "MSSQL"			)] =	eSTC_LEX_MSSQL		;
	mapLexers[cuT( "Verilog"		)] =	eSTC_LEX_VERILOG	;
	mapLexers[cuT( "Kix"			)] =	eSTC_LEX_KIX		;
	mapLexers[cuT( "Gui4Cli"		)] =	eSTC_LEX_GUI4CLI	;
	mapLexers[cuT( "Specman"		)] =	eSTC_LEX_SPECMAN	;
	mapLexers[cuT( "AU3"			)] =	eSTC_LEX_AU3		;
	mapLexers[cuT( "APDL"			)] =	eSTC_LEX_APDL		;
	mapLexers[cuT( "Bash"			)] =	eSTC_LEX_BASH		;
	mapLexers[cuT( "ASN1"			)] =	eSTC_LEX_ASN1		;
	mapLexers[cuT( "VHDL"			)] =	eSTC_LEX_VHDL		;
	mapLexers[cuT( "XAML"			)] =	eSTC_LEX_CAML		;
	mapLexers[cuT( "BlitzBasic"		)] =	eSTC_LEX_BLITZBASIC	;
	mapLexers[cuT( "PureBasic"		)] =	eSTC_LEX_PUREBASIC	;
	mapLexers[cuT( "Haskell"		)] =	eSTC_LEX_HASKELL	;
	mapLexers[cuT( "PhpScript"		)] =	eSTC_LEX_PHPSCRIPT	;
	mapLexers[cuT( "TADS3"			)] =	eSTC_LEX_TADS3		;
	mapLexers[cuT( "Rebol"			)] =	eSTC_LEX_REBOL		;
	mapLexers[cuT( "SmallTalk"		)] =	eSTC_LEX_SMALLTALK	;
	mapLexers[cuT( "Flagship"		)] =	eSTC_LEX_FLAGSHIP	;
	mapLexers[cuT( "CSound"			)] =	eSTC_LEX_CSOUND		;
	mapLexers[cuT( "FreeBasic"		)] =	eSTC_LEX_FREEBASIC	;
	mapLexers[cuT( "InnoSetup"		)] =	eSTC_LEX_INNOSETUP	;
	mapLexers[cuT( "Opal"			)] =	eSTC_LEX_OPAL		;
	mapLexers[cuT( "Spice"			)] =	eSTC_LEX_SPICE		;
#if wxVERSION_NUMBER >= 2900
	mapLexers[cuT( "D"				)] =	eSTC_LEX_D			;
	mapLexers[cuT( "CMake"			)] =	eSTC_LEX_CMAKE		;
	mapLexers[cuT( "Gap"			)] =	eSTC_LEX_GAP		;
	mapLexers[cuT( "PLM"			)] =	eSTC_LEX_PLM		;
	mapLexers[cuT( "Progress"		)] =	eSTC_LEX_PROGRESS	;
	mapLexers[cuT( "Abaqs"			)] =	eSTC_LEX_ABAQUS		;
	mapLexers[cuT( "Asymptote"		)] =	eSTC_LEX_ASYMPTOTE	;
	mapLexers[cuT( "R"				)] =	eSTC_LEX_R			;
	mapLexers[cuT( "Magik"			)] =	eSTC_LEX_MAGIK		;
	mapLexers[cuT( "PowerShell"		)] =	eSTC_LEX_POWERSHELL	;
	mapLexers[cuT( "MySQL"			)] =	eSTC_LEX_MYSQL		;
	mapLexers[cuT( "PO"				)] =	eSTC_LEX_PO			;
	mapLexers[cuT( "TAL"			)] =	eSTC_LEX_TAL		;
	mapLexers[cuT( "Cobol"			)] =	eSTC_LEX_COBOL		;
	mapLexers[cuT( "TACL"			)] =	eSTC_LEX_TACL		;
	mapLexers[cuT( "Sorcus"			)] =	eSTC_LEX_SORCUS		;
	mapLexers[cuT( "PowerPro"		)] =	eSTC_LEX_POWERPRO	;
	mapLexers[cuT( "Nimrod"			)] =	eSTC_LEX_NIMROD		;
	mapLexers[cuT( "SML"			)] =	eSTC_LEX_SML		;
	mapLexers[cuT( "Markdown"		)] =	eSTC_LEX_MARKDOWN	;
#endif

	mapFoldFlags[cuT( "comment"		)] = eSTC_FOLD_COMMENT;
	mapFoldFlags[cuT( "compact"		)] = eSTC_FOLD_COMPACT;
	mapFoldFlags[cuT( "preprocessor")] = eSTC_FOLD_PREPROC;
}

//*************************************************************************************************

LanguageFileParser :: LanguageFileParser( StcContext * p_pStcContext )
	:	FileParser( eSECTION_ROOT, eSECTION_COUNT )
	,	m_pStcContext	( p_pStcContext )
{
}

LanguageFileParser :: ~LanguageFileParser()
{
}

void LanguageFileParser :: DoInitialiseParser( TextFile & p_file )
{
	LanguageFileContextPtr l_pContext = std::make_shared< LanguageFileContext >( &p_file );

	AddParser( eSECTION_ROOT,		cuT( "language"		),	Root_Language		, 1,	ePARAMETER_TYPE_NAME									);
	AddParser( eSECTION_LANGUAGE,	cuT( "pattern"		),	Language_Pattern	, 1,	ePARAMETER_TYPE_TEXT									);
	AddParser( eSECTION_LANGUAGE,	cuT( "lexer"		),	Language_Lexer		, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapLexers	);
	AddParser( eSECTION_LANGUAGE,	cuT( "fold_flags"	),	Language_FoldFlags	, 1,	ePARAMETER_TYPE_TEXT									);
	AddParser( eSECTION_LANGUAGE,	cuT( "section"		),	Language_Section																	);
	AddParser( eSECTION_LANGUAGE,	cuT( "style"		),	Language_Style																		);
	AddParser( eSECTION_STYLE,		cuT( "type"			),	Style_Type			, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapTypes	);
	AddParser( eSECTION_STYLE,		cuT( "fg_colour"	),	Style_FgColour		, 1,	ePARAMETER_TYPE_NAME									);
	AddParser( eSECTION_STYLE,		cuT( "bg_colour"	),	Style_BgColour		, 1,	ePARAMETER_TYPE_NAME									);
	AddParser( eSECTION_STYLE,		cuT( "font_name"	),	Style_FontName		, 1,	ePARAMETER_TYPE_TEXT									);
	AddParser( eSECTION_STYLE,		cuT( "font_style"	),	Style_FontStyle		, 1,	ePARAMETER_TYPE_TEXT									);
	AddParser( eSECTION_STYLE,		cuT( "font_size"	),	Style_FontSize		, 1,	ePARAMETER_TYPE_INT32									);
	AddParser( eSECTION_SECTION,	cuT( "type"			),	Section_Type		, 1,	ePARAMETER_TYPE_CHECKED_TEXT,	&l_pContext->mapTypes	);
	AddParser( eSECTION_SECTION,	cuT( "list"			),	Section_List																		);
	AddParser( eSECTION_SECTION,	cuT( "}"			),	Section_End																			);

	m_pParsingContext = l_pContext;
	l_pContext->pCurrentLanguage.reset( new LanguageInfo );
}

void LanguageFileParser :: DoCleanupParser()
{
	std::static_pointer_cast< LanguageFileContext >( m_pParsingContext )->pCurrentLanguage.reset();
}

void LanguageFileParser :: DoDiscardParser( String const & p_strLine )
{
	if( m_pParsingContext->stackSections.top() == eSECTION_LIST )
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

void LanguageFileParser :: DoValidate()
{
	m_pStcContext->AddLanguage( std::static_pointer_cast< LanguageFileContext >( m_pParsingContext )->pCurrentLanguage );
}

//*************************************************************************************************

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Root_Language )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentLanguage->SetName( l_strName );
	ATTRIBUTE_END_PUSH( eSECTION_LANGUAGE );
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Pattern )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( !l_strParams.empty() )
	{
		StringArray l_array = str_utils::split( l_strParams, cuT( "\t ,;" ), 100, false );
		String l_strPatterns;

		std::for_each( l_array.begin(), l_array.end(), [&]( String const & p_strPattern )
		{
			if( !l_strPatterns.empty() )
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

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Lexer )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiLexer;
	p_arrayParams[0]->Get( l_uiLexer );
	l_pContext->pCurrentLanguage->SetLexer( eSTC_LEX( l_uiLexer ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_FoldFlags )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( !l_strParams.empty() )
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

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Section )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	l_pContext->arrayWords.clear();
	l_pContext->eStyle = eSTC_TYPE_COUNT;
	l_pContext->eType = eSTC_TYPE_COUNT;

	ATTRIBUTE_END_PUSH( eSECTION_SECTION );
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Language_Style )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	l_pContext->arrayWords.clear();
	l_pContext->eStyle = eSTC_TYPE_COUNT;
	l_pContext->eType = eSTC_TYPE_COUNT;

	ATTRIBUTE_END_PUSH( eSECTION_STYLE );
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_Type )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );
	l_pContext->pCurrentStyle = l_pContext->pCurrentLanguage->GetStyle( eSTC_TYPE( l_uiType ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FgColour )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetForeground( l_strName );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_BgColour )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetBackground( l_strName );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontName )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	l_pContext->pCurrentStyle->SetFontName( l_strName );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontStyle )
{
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( !l_strParams.empty() )
	{
		LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
		StringArray l_arrayStyles = str_utils::split( str_utils::lower_case( str_utils::trim( l_strParams ) ), wxT( "\t " ), 10, false );
		int l_iStyle = 0;

		std::for_each( l_arrayStyles.begin(), l_arrayStyles.end(), [&]( String const & p_strStyle )
		{
			if( p_strStyle == wxT( "bold" ) )
			{
				l_iStyle |= eSTC_STYLE_BOLD;
			}
			else if( p_strStyle == wxT( "italic" ) )
			{
				l_iStyle |= eSTC_STYLE_ITALIC;
			}
			else if( p_strStyle == wxT( "underlined" ) )
			{
				l_iStyle |= eSTC_STYLE_UNDERL;
			}
			else if( p_strStyle == wxT( "hidden" ) )
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

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Style_FontSize )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	int32_t l_iSize;
	p_arrayParams[0]->Get( l_iSize );
	l_pContext->pCurrentStyle->SetFontSize( l_iSize );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_Type )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );
	l_pContext->eType = eSTC_TYPE( l_uiType );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_End )
{
	LanguageFileContextPtr l_pContext = std::static_pointer_cast< LanguageFileContext >( p_pContext );

	if( l_pContext->eType != eSTC_TYPE_COUNT )
	{
		l_pContext->pCurrentLanguage->SetWords( l_pContext->eType, l_pContext->arrayWords );
	}
	else
	{
		PARSING_ERROR( cuT( "directive <section> must contain a directive <type>" ) );
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( GuiCommon, Section_List )
{
	ATTRIBUTE_END_PUSH( eSECTION_LIST );
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

//*************************************************************************************************
