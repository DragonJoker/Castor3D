#include "Aria/Editor/LanguageFileContext.hpp"

#include "Aria/Editor/LanguageInfo.hpp"
#include "Aria/Editor/StcContext.hpp"
#include "Aria/Editor/StyleInfo.hpp"

namespace aria
{
	LanguageFileContext::LanguageFileContext( castor::Path const & path
		, LanguageInfo & currentLanguage )
		: castor::FileParserContext{ *castor::Logger::getSingleton().getInstance(), path }
		, currentLanguage{ currentLanguage }
	{
		mapTypes[cuT( "default" )] = uint32_t( wxSTC_C_DEFAULT );
		mapTypes[cuT( "keyword1" )] = uint32_t( wxSTC_C_WORD );
		mapTypes[cuT( "keyword2" )] = uint32_t( wxSTC_C_WORD2 );
		mapTypes[cuT( "keyword3" )] = uint32_t( wxSTC_C_IDENTIFIER );
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
		mapTypes[cuT( "number" )] = uint32_t( wxSTC_C_NUMBER );

		mapFoldFlags[cuT( "comment" )] = eSTC_FOLD_COMMENT;
		mapFoldFlags[cuT( "compact" )] = eSTC_FOLD_COMPACT;
		mapFoldFlags[cuT( "preprocessor" )] = eSTC_FOLD_PREPROC;
	}
}
