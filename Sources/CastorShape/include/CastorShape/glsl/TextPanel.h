#ifndef ___TextPanel___
#define ___TextPanel___

#include "ScriptCompiler.h"

namespace CastorShape
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

	class TextCtrl;
	class TextLinesCtrl;

	class TextPanel : public wxScrolledWindow
	{
	private:
		wxPanel * m_container;
		TextCtrl * m_text;
		TextLinesCtrl * m_lines;
		bool m_showLines;
		wxString m_strFileName;

		wxString m_search;
		C3DVector( SearchInfo *) m_searchPositions;
		size_t m_currentSearchPosition;

	public:
		TextPanel( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size,
				   long style, bool p_showLines);

		void WriteText( const wxString & p_text);
		void LoadFile( const wxString & p_path, ScriptCompiler * p_pCompiler);
		void SaveFile( const wxString & p_path=wxEmptyString, ScriptCompiler * p_pCompiler=NULL);
		void ParseFile( ScriptCompiler * p_pCompiler);
		void UpdateText();
		void UpdateSize();
		void UpdateScrollBars();
		bool IsModified();
		void GoToLine( int p_line);
		bool Search( const wxString & p_search);
		bool SearchNext();
		bool SearchPrevious();
		void Replace( const wxString & p_replacement);

	public:
		inline TextCtrl *	GetText()const { return m_text; }

	private:
		DECLARE_EVENT_TABLE()
		void _onResize( wxSizeEvent & p_event);
		void _onSetFocus( wxFocusEvent & p_event);
	};
}

#endif