#include "LanguageFileContext.hpp"
#include "StcContext.hpp"
#include "StyleInfo.hpp"
#include "LanguageInfo.hpp"

using namespace castor;

namespace GuiCommon
{
	LanguageFileContext::LanguageFileContext( TextFile * p_pFile )
		:	FileParserContext( p_pFile )
	{
		mapTypes[cuT( "default" )] = eSTC_TYPE_DEFAULT;
		mapTypes[cuT( "word1" )] = eSTC_TYPE_WORD1;
		mapTypes[cuT( "word2" )] = eSTC_TYPE_WORD2;
		mapTypes[cuT( "word3" )] = eSTC_TYPE_WORD3;
		mapTypes[cuT( "word4" )] = eSTC_TYPE_WORD4;
		mapTypes[cuT( "word5" )] = eSTC_TYPE_WORD5;
		mapTypes[cuT( "word6" )] = eSTC_TYPE_WORD6;
		mapTypes[cuT( "comment" )] = eSTC_TYPE_COMMENT;
		mapTypes[cuT( "comment_doc" )] = eSTC_TYPE_COMMENT_DOC;
		mapTypes[cuT( "comment_line" )] = eSTC_TYPE_COMMENT_LINE;
		mapTypes[cuT( "comment_special" )] = eSTC_TYPE_COMMENT_SPECIAL;
		mapTypes[cuT( "char" )] = eSTC_TYPE_CHARACTER;
		mapTypes[cuT( "char_eol" )] = eSTC_TYPE_CHARACTER_EOL;
		mapTypes[cuT( "string" )] = eSTC_TYPE_STRING;
		mapTypes[cuT( "string_eol" )] = eSTC_TYPE_STRING_EOL;
		mapTypes[cuT( "delimiter" )] = eSTC_TYPE_DELIMITER;
		mapTypes[cuT( "punctuation" )] = eSTC_TYPE_PUNCTUATION;
		mapTypes[cuT( "operator" )] = eSTC_TYPE_OPERATOR;
		mapTypes[cuT( "brace" )] = eSTC_TYPE_BRACE;
		mapTypes[cuT( "command" )] = eSTC_TYPE_COMMAND;
		mapTypes[cuT( "identifier" )] = eSTC_TYPE_IDENTIFIER;
		mapTypes[cuT( "label" )] = eSTC_TYPE_LABEL;
		mapTypes[cuT( "number" )] = eSTC_TYPE_NUMBER;
		mapTypes[cuT( "parameter" )] = eSTC_TYPE_PARAMETER;
		mapTypes[cuT( "regex" )] = eSTC_TYPE_REGEX;
		mapTypes[cuT( "uuid" )] = eSTC_TYPE_UUID;
		mapTypes[cuT( "value" )] = eSTC_TYPE_VALUE;
		mapTypes[cuT( "preprocessor" )] = eSTC_TYPE_PREPROCESSOR;
		mapTypes[cuT( "script" )] = eSTC_TYPE_SCRIPT;
		mapTypes[cuT( "error" )] = eSTC_TYPE_ERROR;
		mapTypes[cuT( "undefined" )] = eSTC_TYPE_UNDEFINED;

		mapLexers[cuT( "Container" )] =	eSTC_LEX_CONTAINER	;
		mapLexers[cuT( "Null" )] =	eSTC_LEX_NULL		;
		mapLexers[cuT( "Python" )] =	eSTC_LEX_PYTHON		;
		mapLexers[cuT( "C/C++" )] =	eSTC_LEX_CPP		;
		mapLexers[cuT( "HTML" )] =	eSTC_LEX_HTML		;
		mapLexers[cuT( "XML" )] =	eSTC_LEX_XML		;
		mapLexers[cuT( "Perl" )] =	eSTC_LEX_PERL		;
		mapLexers[cuT( "SQL" )] =	eSTC_LEX_SQL		;
		mapLexers[cuT( "VB" )] =	eSTC_LEX_VB			;
		mapLexers[cuT( "Properties" )] =	eSTC_LEX_PROPERTIES	;
		mapLexers[cuT( "ErrorList" )] =	eSTC_LEX_ERRORLIST	;
		mapLexers[cuT( "Makefile" )] =	eSTC_LEX_MAKEFILE	;
		mapLexers[cuT( "Batch" )] =	eSTC_LEX_BATCH		;
		mapLexers[cuT( "XCode" )] =	eSTC_LEX_XCODE		;
		mapLexers[cuT( "Latex" )] =	eSTC_LEX_LATEX		;
		mapLexers[cuT( "LUA" )] =	eSTC_LEX_LUA		;
		mapLexers[cuT( "Diff" )] =	eSTC_LEX_DIFF		;
		mapLexers[cuT( "Conf" )] =	eSTC_LEX_CONF		;
		mapLexers[cuT( "Pascal" )] =	eSTC_LEX_PASCAL		;
		mapLexers[cuT( "Ave" )] =	eSTC_LEX_AVE		;
		mapLexers[cuT( "Ada" )] =	eSTC_LEX_ADA		;
		mapLexers[cuT( "Lisp" )] =	eSTC_LEX_LISP		;
		mapLexers[cuT( "Ruby" )] =	eSTC_LEX_RUBY		;
		mapLexers[cuT( "Eiffel" )] =	eSTC_LEX_EIFFEL		;
		mapLexers[cuT( "TCL" )] =	eSTC_LEX_TCL		;
		mapLexers[cuT( "NnCrontTab" )] =	eSTC_LEX_NNCRONTAB	;
		mapLexers[cuT( "Bullant" )] =	eSTC_LEX_BULLANT	;
		mapLexers[cuT( "VBScript" )] =	eSTC_LEX_VBSCRIPT	;
		mapLexers[cuT( "BAAN" )] =	eSTC_LEX_BAAN		;
		mapLexers[cuT( "Matlab" )] =	eSTC_LEX_MATLAB		;
		mapLexers[cuT( "Scriptol" )] =	eSTC_LEX_SCRIPTOL	;
		mapLexers[cuT( "Asm" )] =	eSTC_LEX_ASM		;
		mapLexers[cuT( "Fortran" )] =	eSTC_LEX_FORTRAN	;
		mapLexers[cuT( "F77" )] =	eSTC_LEX_F77		;
		mapLexers[cuT( "CSS" )] =	eSTC_LEX_CSS		;
		mapLexers[cuT( "POV" )] =	eSTC_LEX_POV		;
		mapLexers[cuT( "Lout" )] =	eSTC_LEX_LOUT		;
		mapLexers[cuT( "EScript" )] =	eSTC_LEX_ESCRIPT	;
		mapLexers[cuT( "PS" )] =	eSTC_LEX_PS			;
		mapLexers[cuT( "NSIS" )] =	eSTC_LEX_NSIS		;
		mapLexers[cuT( "MMixal" )] =	eSTC_LEX_MMIXAL		;
		mapLexers[cuT( "CLW" )] =	eSTC_LEX_CLW		;
		mapLexers[cuT( "LOT" )] =	eSTC_LEX_LOT		;
		mapLexers[cuT( "YAML" )] =	eSTC_LEX_YAML		;
		mapLexers[cuT( "Tex" )] =	eSTC_LEX_TEX		;
		mapLexers[cuT( "Metapost" )] =	eSTC_LEX_METAPOST	;
		mapLexers[cuT( "PowerBasic" )] =	eSTC_LEX_POWERBASIC	;
		mapLexers[cuT( "Forth" )] =	eSTC_LEX_FORTH		;
		mapLexers[cuT( "Erlang" )] =	eSTC_LEX_ERLANG		;
		mapLexers[cuT( "Octave" )] =	eSTC_LEX_OCTAVE		;
		mapLexers[cuT( "MSSQL" )] =	eSTC_LEX_MSSQL		;
		mapLexers[cuT( "Verilog" )] =	eSTC_LEX_VERILOG	;
		mapLexers[cuT( "Kix" )] =	eSTC_LEX_KIX		;
		mapLexers[cuT( "Gui4Cli" )] =	eSTC_LEX_GUI4CLI	;
		mapLexers[cuT( "Specman" )] =	eSTC_LEX_SPECMAN	;
		mapLexers[cuT( "AU3" )] =	eSTC_LEX_AU3		;
		mapLexers[cuT( "APDL" )] =	eSTC_LEX_APDL		;
		mapLexers[cuT( "Bash" )] =	eSTC_LEX_BASH		;
		mapLexers[cuT( "ASN1" )] =	eSTC_LEX_ASN1		;
		mapLexers[cuT( "VHDL" )] =	eSTC_LEX_VHDL		;
		mapLexers[cuT( "XAML" )] =	eSTC_LEX_CAML		;
		mapLexers[cuT( "BlitzBasic" )] =	eSTC_LEX_BLITZBASIC	;
		mapLexers[cuT( "PureBasic" )] =	eSTC_LEX_PUREBASIC	;
		mapLexers[cuT( "Haskell" )] =	eSTC_LEX_HASKELL	;
		mapLexers[cuT( "PhpScript" )] =	eSTC_LEX_PHPSCRIPT	;
		mapLexers[cuT( "TADS3" )] =	eSTC_LEX_TADS3		;
		mapLexers[cuT( "Rebol" )] =	eSTC_LEX_REBOL		;
		mapLexers[cuT( "SmallTalk" )] =	eSTC_LEX_SMALLTALK	;
		mapLexers[cuT( "Flagship" )] =	eSTC_LEX_FLAGSHIP	;
		mapLexers[cuT( "CSound" )] =	eSTC_LEX_CSOUND		;
		mapLexers[cuT( "FreeBasic" )] =	eSTC_LEX_FREEBASIC	;
		mapLexers[cuT( "Innosetup" )] =	eSTC_LEX_INNOSETUP	;
		mapLexers[cuT( "Opal" )] =	eSTC_LEX_OPAL		;
		mapLexers[cuT( "Spice" )] =	eSTC_LEX_SPICE		;
#if wxVERSION_NUMBER >= 2900
		mapLexers[cuT( "D" )] =	eSTC_LEX_D			;
		mapLexers[cuT( "CMake" )] =	eSTC_LEX_CMAKE		;
		mapLexers[cuT( "Gap" )] =	eSTC_LEX_GAP		;
		mapLexers[cuT( "PLM" )] =	eSTC_LEX_PLM		;
		mapLexers[cuT( "Progress" )] =	eSTC_LEX_PROGRESS	;
		mapLexers[cuT( "Abaqs" )] =	eSTC_LEX_ABAQUS		;
		mapLexers[cuT( "Asymptote" )] =	eSTC_LEX_ASYMPTOTE	;
		mapLexers[cuT( "R" )] =	eSTC_LEX_R			;
		mapLexers[cuT( "Magik" )] =	eSTC_LEX_MAGIK		;
		mapLexers[cuT( "PowerShell" )] =	eSTC_LEX_POWERSHELL	;
		mapLexers[cuT( "MySQL" )] =	eSTC_LEX_MYSQL		;
		mapLexers[cuT( "PO" )] =	eSTC_LEX_PO			;
		mapLexers[cuT( "TAL" )] =	eSTC_LEX_TAL		;
		mapLexers[cuT( "Cobol" )] =	eSTC_LEX_COBOL		;
		mapLexers[cuT( "TACL" )] =	eSTC_LEX_TACL		;
		mapLexers[cuT( "Sorcus" )] =	eSTC_LEX_SORCUS		;
		mapLexers[cuT( "PowerPro" )] =	eSTC_LEX_POWERPRO	;
		mapLexers[cuT( "Nimrod" )] =	eSTC_LEX_NIMROD		;
		mapLexers[cuT( "SML" )] =	eSTC_LEX_SML		;
		mapLexers[cuT( "Markdown" )] =	eSTC_LEX_MARKDOWN	;
#endif

		mapFoldFlags[cuT( "comment" )] = eSTC_FOLD_COMMENT;
		mapFoldFlags[cuT( "compact" )] = eSTC_FOLD_COMPACT;
		mapFoldFlags[cuT( "preprocessor" )] = eSTC_FOLD_PREPROC;
	}
}
