#include "GuiCommon/Shader/LanguageFileContext.hpp"

#include "GuiCommon/Shader/StcContext.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"

using namespace castor;

namespace GuiCommon
{
	LanguageFileContext::LanguageFileContext( Path const & path )
		:	FileParserContext( path )
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

		mapFoldFlags[cuT( "comment" )] = eSTC_FOLD_COMMENT;
		mapFoldFlags[cuT( "compact" )] = eSTC_FOLD_COMPACT;
		mapFoldFlags[cuT( "preprocessor" )] = eSTC_FOLD_PREPROC;
	}
}
