#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	LanguageInfo::LanguageInfo()
		:m_strName( DEFAULT_LANGUAGE )
		,m_strFilePattern( cuEmptyString )
		,m_eLexerID( eSTC_LEX_COUNT )
		,m_iFoldFlags( 0 )
	{
		m_arrayStyles[eSTC_TYPE_DEFAULT].reset( new StyleInfo( wxT( "Default" ), wxT( "BLACK" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD1].reset( new StyleInfo( wxT( "Keyword1" ), wxT( "BLUE" ), wxT( "WHITE" ), wxT( "" ), 10, eSTC_STYLE_BOLD, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD2].reset( new StyleInfo( wxT( "Keyword2" ), wxT( "MIDNIGHT BLUE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD3].reset( new StyleInfo( wxT( "Keyword3" ), wxT( "CORNFLOWER BLUE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD4].reset( new StyleInfo( wxT( "Keyword4" ), wxT( "CYAN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD5].reset( new StyleInfo( wxT( "Keyword5" ), wxT( "DARK GREY" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_WORD6].reset( new StyleInfo( wxT( "Keyword6" ), wxT( "GREY" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_COMMENT].reset( new StyleInfo( wxT( "Comment" ), wxT( "FOREST GREEN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_COMMENT_DOC].reset( new StyleInfo( wxT( "Comment (Doc)" ), wxT( "FOREST GREEN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_COMMENT_LINE].reset( new StyleInfo( wxT( "Comment line" ), wxT( "FOREST GREEN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_COMMENT_SPECIAL].reset( new StyleInfo( wxT( "Special comment" ), wxT( "FOREST GREEN" ), wxT( "WHITE" ), wxT( "" ), 10, eSTC_STYLE_ITALIC, 0 ) );
		m_arrayStyles[eSTC_TYPE_CHARACTER].reset( new StyleInfo( wxT( "Character" ), wxT( "KHAKI" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_CHARACTER_EOL].reset( new StyleInfo( wxT( "Character (EOL)" ), wxT( "KHAKI" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_STRING].reset( new StyleInfo( wxT( "String" ), wxT( "BROWN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_STRING_EOL].reset( new StyleInfo( wxT( "String (EOL)" ), wxT( "BROWN" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_DELIMITER].reset( new StyleInfo( wxT( "Delimiter" ), wxT( "ORANGE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_PUNCTUATION].reset( new StyleInfo( wxT( "Punctuation" ), wxT( "ORANGE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_OPERATOR].reset( new StyleInfo( wxT( "Operator" ), wxT( "BLACK" ), wxT( "WHITE" ), wxT( "" ), 10, eSTC_STYLE_BOLD, 0 ) );
		m_arrayStyles[eSTC_TYPE_BRACE].reset( new StyleInfo( wxT( "Label" ), wxT( "VIOLET" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_COMMAND].reset( new StyleInfo( wxT( "Command" ), wxT( "BLUE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_IDENTIFIER].reset( new StyleInfo( wxT( "Identifier" ), wxT( "BLACK" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_LABEL].reset( new StyleInfo( wxT( "Label" ), wxT( "VIOLET" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_NUMBER].reset( new StyleInfo( wxT( "Number" ), wxT( "SIENNA" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_PARAMETER].reset( new StyleInfo( wxT( "Parameter" ), wxT( "VIOLET" ), wxT( "WHITE" ), wxT( "" ), 10, eSTC_STYLE_ITALIC, 0 ) );
		m_arrayStyles[eSTC_TYPE_REGEX].reset( new StyleInfo( wxT( "Regular expression" ), wxT( "ORCHID" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_UUID].reset( new StyleInfo( wxT( "UUID" ), wxT( "ORCHID" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_VALUE].reset( new StyleInfo( wxT( "Value" ), wxT( "ORCHID" ), wxT( "WHITE" ), wxT( "" ), 10, eSTC_STYLE_ITALIC, 0 ) );
		m_arrayStyles[eSTC_TYPE_PREPROCESSOR].reset( new StyleInfo( wxT( "Preprocessor" ), wxT( "GREY" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_SCRIPT].reset( new StyleInfo( wxT( "Script" ), wxT( "DARK GREY" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_ERROR].reset( new StyleInfo( wxT( "Error" ), wxT( "RED" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
		m_arrayStyles[eSTC_TYPE_UNDEFINED].reset( new StyleInfo( wxT( "Undefined" ), wxT( "ORANGE" ), wxT( "WHITE" ), wxT( "" ), 10, 0, 0 ) );
	}

	LanguageInfo::~LanguageInfo()
	{
	}

	void LanguageInfo::SetWords( eSTC_TYPE p_type, Castor::StringArray const & p_arrayWords )
	{
		String l_strWords;
		std::for_each( p_arrayWords.begin(), p_arrayWords.end(), [&]( String const & p_strWord )
		{
			if ( !l_strWords.empty() )
			{
				l_strWords += cuT( " " );
			}

			l_strWords += p_strWord;
		} );
		m_arrayWords[p_type] = l_strWords;
	}
}
