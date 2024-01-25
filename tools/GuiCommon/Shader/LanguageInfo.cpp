#include "GuiCommon/Shader/LanguageInfo.hpp"

#include "GuiCommon/Shader/StyleInfo.hpp"

CU_ImplementSmartPtr( GuiCommon, LanguageInfo )

namespace GuiCommon
{
	static wxColour const CommentColour{ wxT( "FOREST GREEN" ) };
	static wxColour const StringColour{ wxT( "BROWN" ) };
	static wxColour const LiteralColour{ wxT( "SIENNA" ) };
	static wxColour const KeywordColour{ wxT( "SIENNA" ) };
	static wxColour const PreprocColour{ wxT( "SIENNA" ) };

	LanguageInfo::LanguageInfo()
		: m_styles
		{
			{ wxSTC_C_DEFAULT, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTLINE, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_NUMBER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_WORD, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_CHARACTER, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_UUID, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_PREPROCESSOR, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_OPERATOR, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_IDENTIFIER, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRINGEOL, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_VERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_REGEX, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTLINEDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_WORD2, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOCKEYWORD, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOCKEYWORDERROR, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_GLOBALCLASS, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRINGRAW, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_TRIPLEVERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_HASHQUOTEDSTRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
#if wxCHECK_VERSION( 3, 1, 0 )
			{ wxSTC_C_PREPROCESSORCOMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_PREPROCESSORCOMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_USERLITERAL, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_TASKMARKER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_ESCAPESEQUENCE, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
#endif
		}
	{
	}

	void LanguageInfo::setKeywords( uint32_t index
		, castor::StringArray const & keywords )
	{
		CU_Require( index < 9 );
		castor::String & toAdd = m_keywords[index];
		castor::String sep;

		if ( isCLike )
		{
			for ( auto keyword : keywords )
			{
				toAdd += sep + keyword;
				sep = cuT( " " );
			}
		}
		else
		{
			for ( auto keyword : keywords )
			{
				toAdd += sep + castor::string::toLowerCase( keyword );
				sep = cuT( " " );
			}
		}
	}

	void LanguageInfo::updateStyles()
	{
		if ( isCLike )
		{
			m_styles = { { wxSTC_C_DEFAULT, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENTLINE, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_NUMBER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_WORD, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_STRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_CHARACTER, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_UUID, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_PREPROCESSOR, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_OPERATOR, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_IDENTIFIER, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_STRINGEOL, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_VERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_REGEX, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENTLINEDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_WORD2, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENTDOCKEYWORD, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_COMMENTDOCKEYWORDERROR, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_GLOBALCLASS, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_STRINGRAW, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_TRIPLEVERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_HASHQUOTEDSTRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
#if wxCHECK_VERSION( 3, 1, 0 )
				, { wxSTC_C_PREPROCESSORCOMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_PREPROCESSORCOMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_USERLITERAL, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_TASKMARKER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_C_ESCAPESEQUENCE, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
#endif
			};
		}
		else
		{
			m_styles = { { wxSTC_A68K_DEFAULT, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_OPERATOR, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_IDENTIFIER, StyleInfo{ PANEL_FOREGROUND_COLOUR, PANEL_BACKGROUND_COLOUR, 0, 0 } }

				, { wxSTC_A68K_NUMBER_DEC, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_NUMBER_BIN, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_NUMBER_HEX, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }

				, { wxSTC_A68K_STRING1, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_STRING2, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }

				, { wxSTC_A68K_CPUINSTRUCTION, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_EXTINSTRUCTION, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_REGISTER, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_DIRECTIVE, StyleInfo{ KeywordColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }

				, { wxSTC_A68K_MACRO_DECLARATION, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_MACRO_ARG, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_LABEL, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }

				, { wxSTC_A68K_COMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_COMMENT_WORD, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_COMMENT_SPECIAL, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
				, { wxSTC_A68K_COMMENT_DOXYGEN, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } }
			};
		}
	}
}
