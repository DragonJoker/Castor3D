#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/SyntaxHighlighter.hpp"

#include <boost/spirit/include/qi.hpp>

using namespace GuiCommon;
using namespace boost;
using namespace boost::spirit;

//*************************************************************************************************

LanguageSyntax :: LanguageSyntax( const wxString & p_strName)
	:	m_strName( p_strName)
{
}

LanguageSyntax :: ~LanguageSyntax()
{
}

void LanguageSyntax :: AddWord( eSYNTAX_TYPE p_eType, const wxString & p_strWord)
{
	m_arrayWords[p_eType].insert( p_strWord);
}

void LanguageSyntax :: SetTypeAttributes( eSYNTAX_TYPE p_eType, const wxTextAttr & p_attribs)
{
	m_arrayAttributes[p_eType] = p_attribs;
}

const wxTextAttr & LanguageSyntax :: GetWordAttribs( const wxString & p_strWord)
{
	String l_strWord = (const wxChar *)p_strWord.c_str();

	if (m_arrayWords[eSYNTAX_TYPE_KEYWORD].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_KEYWORD].end())
	{
		return m_arrayAttributes[eSYNTAX_TYPE_KEYWORD];
	}
	else if (m_arrayWords[eSYNTAX_TYPE_TYPE].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_TYPE].end())
	{
		return m_arrayAttributes[eSYNTAX_TYPE_TYPE];
	}
	else if (m_arrayWords[eSYNTAX_TYPE_FUNCTION].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_FUNCTION].end())
	{
		return m_arrayAttributes[eSYNTAX_TYPE_FUNCTION];
	}
	else if (m_arrayWords[eSYNTAX_TYPE_ATTRIBUTE].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_ATTRIBUTE].end())
	{
		return m_arrayAttributes[eSYNTAX_TYPE_ATTRIBUTE];
	}
	else if (l_strWord.is_integer() || l_strWord.is_floating())
	{
		return m_arrayAttributes[eSYNTAX_TYPE_NUMBER];
	}
	else
	{
		return m_arrayAttributes[eSYNTAX_TYPE_TEXT];
	}
}

bool LanguageSyntax :: IsOperator( const wxString & p_strWord)
{
	return m_arrayWords[eSYNTAX_TYPE_OPERATOR].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_OPERATOR].end();
}

bool LanguageSyntax :: IsSeparator( const wxString & p_strWord)
{
	return m_arrayWords[eSYNTAX_TYPE_SEPARATOR].find( p_strWord) != m_arrayWords[eSYNTAX_TYPE_SEPARATOR].end();
}

//*************************************************************************************************

wxSyntaxHighlighterBase :: wxSyntaxHighlighterBase( wxTextCtrl * p_pTextCtrl)
	:	m_pTextCtrl( p_pTextCtrl)
	,	m_pSyntax( new LanguageSyntax( wxT( "GLSL")))
{
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( ","));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( ","));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( ";"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( " "));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "("));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( ")"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "}"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "{"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "]"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "["));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "\n"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "\r"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		wxT( "\t"));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( ":"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "."));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "*"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "+"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "-"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "/"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "%"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "!"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "="));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "<"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( ">"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( "<="));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		wxT( ">="));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "texture"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "texture2D"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "main"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "dot"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "min"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "max"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "mul"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "floor"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "ceil"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "normalize"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "cross"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "clamp"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "pow"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		wxT( "reflect"));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "in"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "out"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "varying"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "uniform"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "smooth"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "const"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "invariant"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "precision"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "highp"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "attribute"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		wxT( "low"));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "void"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "sampler2D"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "bool"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "int"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "float"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec2b"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec3b"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec4b"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec2i"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec3i"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec4i"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec2"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec3"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "vec4"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat2"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat2x3"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat2x4"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat3x2"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat3"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat3x4"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat4x2"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat4x3"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_TYPE,			wxT( "mat4"));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_KEYWORD,		wxT( "for"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_KEYWORD,		wxT( "if"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_KEYWORD,		wxT( "while"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_KEYWORD,		wxT( "return"));

	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_PREPROCESSOR,	wxT( "#"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_COMMENT,		wxT( "//"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_COMMENT_BLOCK,	wxT( "/**/"));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_STRING,		wxT( "\""));
	m_pSyntax->AddWord( LanguageSyntax::eSYNTAX_TYPE_STRING,		wxT( "'"));

	wxFont l_font( 10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT( "Lucida Console"));
	// line condition texts
	wxTextAttr l_attrText(			* wxBLACK,					* wxWHITE, l_font);
	wxTextAttr l_attrComment(		wxColour( 0, 127, 0),		* wxWHITE, l_font);
	wxTextAttr l_attrString(		wxColour( 0, 127, 127),		* wxWHITE, l_font);
	wxTextAttr l_attrPreprocessor(	* wxBLUE,					* wxWHITE, l_font);
	// word condition texts
	wxTextAttr l_attrKeyword(		* wxBLUE,					* wxWHITE, l_font);
	wxTextAttr l_attrType(			* wxBLUE,					* wxWHITE, l_font);
	wxTextAttr l_attrAttribute(		wxColour( 127, 127, 255),	* wxWHITE, l_font);
	wxTextAttr l_attrSeparator(		wxColour( 255, 0, 0),		* wxWHITE, l_font);
	wxTextAttr l_attrNumber(		* wxLIGHT_GREY,				* wxWHITE, l_font);
	wxTextAttr l_attrOperator(		wxColour( 0, 127, 0),		* wxWHITE, l_font);
	wxTextAttr l_attrFunction(		wxColour( 127, 0, 0),		* wxWHITE, l_font);

	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_TEXT,			l_attrText);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_COMMENT,			l_attrComment);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_COMMENT_BLOCK,	l_attrComment);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_STRING,			l_attrString);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_PREPROCESSOR,	l_attrPreprocessor);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_KEYWORD,			l_attrKeyword);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_TYPE,			l_attrType);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_ATTRIBUTE,		l_attrAttribute);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR,		l_attrSeparator);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_NUMBER,			l_attrNumber);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_OPERATOR,		l_attrOperator);
	m_pSyntax->SetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_FUNCTION,		l_attrFunction);
}

wxSyntaxHighlighterBase :: ~wxSyntaxHighlighterBase()
{
	delete m_pSyntax;
}

void wxSyntaxHighlighterBase :: ProcessChar( wxChar p_char, long p_lPosition)
{
	long l_currentCharPosition = p_lPosition;

	if (m_pSyntax->IsSeparator( p_char))
	{
		FinishWord( l_currentCharPosition);
		m_pTextCtrl->SetStyle( l_currentCharPosition, l_currentCharPosition + 1, m_pSyntax->GetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_SEPARATOR));
	}
	else if (m_pSyntax->IsOperator( p_char))
	{
		FinishWord( l_currentCharPosition);
		m_pTextCtrl->SetStyle( l_currentCharPosition, l_currentCharPosition + 1, m_pSyntax->GetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_OPERATOR));
	}
	else if (m_strWord.empty())
	{
		m_strWord += p_char;
		m_wordBegin = l_currentCharPosition;
		m_pTextCtrl->SetStyle( l_currentCharPosition, l_currentCharPosition + 1, m_pSyntax->GetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_TEXT));
	}
	else
	{
		m_strWord += p_char;
	}
}

void wxSyntaxHighlighterBase :: FinishWord( long p_lWordEnd)
{
	m_pTextCtrl->SetStyle( m_wordBegin, p_lWordEnd, m_pSyntax->GetWordAttribs( m_strWord));
	m_strWord.clear();
	m_wordBegin = p_lWordEnd + 1;
}

bool wxSyntaxHighlighterBase :: ParseLine( const wxString & p_strLine, long p_lIndex)
{
	bool l_bReturn = false;

	using qi::double_;
	using qi::phrase_parse;
	using ascii::space;

//	l_bReturn = phrase_parse( p_strLine.begin(), p_strLine.end(), double_ >> *(',' >> double_), space);

	if (p_strLine.find( wxT( "#")) == 0)
	{
		m_pTextCtrl->SetStyle( p_lIndex, p_lIndex + p_strLine.size(), m_pSyntax->GetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_PREPROCESSOR));
	}
	else
	{
		l_bReturn = p_strLine.find( wxT( "/*")) != wxString::npos;
		size_t l_uiIndexComment = std::min( p_strLine.find( wxT( "//")), p_strLine.find( wxT( "/*")));
		bool l_bCommented = l_uiIndexComment != wxString::npos;

		if (l_bCommented)
		{
			m_pTextCtrl->SetStyle( p_lIndex + l_uiIndexComment, p_lIndex + p_strLine.size(), m_pSyntax->GetTypeAttributes( LanguageSyntax::eSYNTAX_TYPE_COMMENT));
		}

		for (size_t i = 0 ; i < l_uiIndexComment && i < p_strLine.size() ; i++)
		{
			ProcessChar( p_strLine[i], p_lIndex + i);
		}
	}

	return l_bReturn;
}

void wxSyntaxHighlighterBase :: ParseText()
{
	m_wordBegin = 0;
	long l_lCurrent = 0;
	long l_lLineBegin = 0;
	size_t i = 0;
	wxChar l_char;
	wxString l_strLine;

	while (l_lCurrent < m_pTextCtrl->GetLastPosition() && i < m_pTextCtrl->GetValue().size())
	{
		l_char = m_pTextCtrl->GetValue()[i];

		if (l_char == wxT( '\n'))
		{
			ParseLine( l_strLine, l_lLineBegin);
			l_lCurrent++;
			l_strLine.clear();
			l_lLineBegin = l_lCurrent + 1;
		}
		else
		{
			l_strLine += l_char;
		}

		l_lCurrent++;
		i++;
	}
}

//*************************************************************************************************
