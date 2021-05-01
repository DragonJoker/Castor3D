#include "Aria/Editor/LanguageInfo.hpp"

#include "Aria/Prerequisites.hpp"
#include "Aria/Editor/StyleInfo.hpp"

namespace aria
{
	static wxColour const CommentColour{ wxT( "FOREST GREEN" ) };
	static wxColour const StringColour{ wxT( "BROWN" ) };
	static wxColour const LiteralColour{ wxT( "SIENNA" ) };
	static wxColour const KeywordColour{ wxT( "SIENNA" ) };
	static wxColour const PreprocColour{ wxT( "SIENNA" ) };

	LanguageInfo::LanguageInfo()
		: name( "cscn" )
		, m_styles
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

	LanguageInfo::~LanguageInfo()
	{
	}

	void LanguageInfo::setKeywords( uint32_t index
		, wxArrayString const & keywords )
	{
		CU_Require( index < 9 );
		auto & toAdd = m_keywords[index];
		wxString sep;

		for ( auto keyword : keywords )
		{
			toAdd += sep + keyword;
			sep = " ";
		}
	}
}
