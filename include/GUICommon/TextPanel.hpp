#ifndef ___GC_TextPanel___
#define ___GC_TextPanel___

namespace GuiCommon
{
	struct SearchInfo
	{
		int m_line;
		int m_position;
		int m_index;

		SearchInfo()
			:	m_line( 0),
				m_position( 0),
				m_index( 0)
		{}

		SearchInfo( int p_line, int p_position, int p_index)
			:	m_line( p_line),
				m_position( p_position),
				m_index( p_index)
		{}
	};

	class wxTextEditor;
	class wxTextLinesCtrl;
	class ScriptCompiler;
	template <class TextEditClass> class wxSyntaxHighlighter;

	class wxTextPanel : public wxScrolledWindow
	{
	private:
		static const int LinesWidth;
		wxPanel * m_container;
		wxSyntaxHighlighter<wxTextEditor> * m_text;
		wxTextLinesCtrl * m_lines;
		bool m_showLines;
		wxString m_strFileName;

		wxString m_search;
		Container<SearchInfo *>::Vector m_searchPositions;
		size_t m_currentSearchPosition;

	public:
		wxTextPanel( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size,
					 long style, bool p_showLines);

		void WriteText( const wxString & p_text);
		void LoadFile( const wxString & p_path, ScriptCompiler * p_pCompiler);
		void SaveFile( const wxString & p_path=wxEmptyString, ScriptCompiler * p_pCompiler=nullptr);
		void ParseFile( ScriptCompiler * p_pCompiler);
		void UpdateText();
		void UpdateSize( const wxSize & p_size);
		void UpdateScrollBars();
		bool IsModified();
		void GoToLine( int p_line);
		bool Search( const wxString & p_search);
		bool SearchNext();
		bool SearchPrevious();
		void replace( const wxString & p_replacement);

	public:
		inline wxSyntaxHighlighter<wxTextEditor>	*	GetText()const { return m_text; }

	private:
		DECLARE_EVENT_TABLE()
		void _onResize( wxSizeEvent & p_event);
		void _onSetFocus( wxFocusEvent & p_event);
	};
}

#endif
