#ifndef ___GC_SyntaxHighlighter___
#define ___GC_SyntaxHighlighter___

namespace GuiCommon
{
	class LanguageSyntax
	{
	public:
		typedef enum
		{	eSYNTAX_TYPE_KEYWORD
		,	eSYNTAX_TYPE_PREPROCESSOR
		,	eSYNTAX_TYPE_TYPE
		,	eSYNTAX_TYPE_FUNCTION
		,	eSYNTAX_TYPE_NUMBER
		,	eSYNTAX_TYPE_ATTRIBUTE
		,	eSYNTAX_TYPE_MATCHING
		,	eSYNTAX_TYPE_SEPARATOR
		,	eSYNTAX_TYPE_OPERATOR
		,	eSYNTAX_TYPE_TEXT
		,	eSYNTAX_TYPE_STRING
		,	eSYNTAX_TYPE_COMMENT
		,	eSYNTAX_TYPE_COMMENT_BLOCK
		,	eSYNTAX_TYPE_COUNT
		}	eSYNTAX_TYPE;

	private:
		wxString m_strName;
		Castor::Templates::array<std::set<wxString>,	eSYNTAX_TYPE_COUNT> m_arrayWords;
		Castor::Templates::array<wxTextAttr,			eSYNTAX_TYPE_COUNT> m_arrayAttributes;
		std::map <wxString, eSYNTAX_TYPE> m_mapWords;

	public:
		LanguageSyntax( const wxString & p_strName);
		~LanguageSyntax();

		void AddWord( eSYNTAX_TYPE p_eType, const wxString & p_strWord);
		void SetTypeAttributes( eSYNTAX_TYPE p_eType, const wxTextAttr & p_attribs);
		const wxTextAttr & GetTypeAttributes( eSYNTAX_TYPE p_eType) { return m_arrayAttributes[p_eType]; }

		const wxTextAttr & GetWordAttribs( const wxString & p_strWord);
		bool IsOperator( const wxString & p_strWord);
		bool IsSeparator( const wxString & p_strWord);
	};

	class wxSyntaxHighlighterBase
	{
	private:
		wxTextCtrl * m_pTextCtrl;
		wxString m_strWord;
		long m_wordBegin;
		LanguageSyntax * m_pSyntax;

	public:
		wxSyntaxHighlighterBase( wxTextCtrl * p_pTextCtrl);
		~wxSyntaxHighlighterBase();

		inline void SetSyntax( LanguageSyntax * p_pSyntax) { m_pSyntax = p_pSyntax; }

		void ProcessChar( wxChar p_char, long p_lPosition);
		void FinishWord( long p_lWordEnd);
		bool ParseLine( const wxString & p_strLine, long p_lIndex);
		void ParseText();
	};

	template <class TextEditClass=wxTextCtrl>
	class wxSyntaxHighlighter : public wxSyntaxHighlighterBase, public TextEditClass
	{
	public:
		wxSyntaxHighlighter(  wxWindow * p_pParent, wxWindowID p_id, const wxString & p_strValue = wxEmptyString, const wxPoint & p_ptPos = wxDefaultPosition, const wxSize & p_szSize = wxDefaultSize, long p_lStyle = 0);
		~wxSyntaxHighlighter();

		virtual bool LoadFile( const wxString & filename, int fileType = wxTEXT_TYPE_ANY);

	protected:
		DECLARE_EVENT_TABLE()
		void OnText( wxKeyEvent & p_event);
	};

#	include "SyntaxHighlighter.inl"
}

#endif
