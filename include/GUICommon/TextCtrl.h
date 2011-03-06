#ifndef ___GC_ShaderEditor___
#define ___GC_ShaderEditor___

namespace GuiCommon
{
	class TextLinesCtrl;
	class TextPanel;

	class TextCtrl : public wxTextCtrl
	{
	protected:
		long m_posDeb;
		long m_posFin;
		wxPoint m_point;
		wxString m_strFileName;
		int m_linesNumber;
		bool m_scrollByWheel;
		real m_difference;
		int m_precPosition;
		TextLinesCtrl * m_lines;
		TextPanel * m_panel;
		int m_deltaLines;
		int m_deltaPages;
		bool m_modified;

	public:
		TextCtrl( wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style);
		~TextCtrl();

		void AutoWord();
		virtual void SaveFile( const wxString & p_path, int p_fileType = 0);
		virtual bool LoadFile( const wxString& filename, int fileType = wxTEXT_TYPE_ANY);
		void GoToLine( int p_line);
		void CompleteCode( int p_keyCode);

		inline bool		IsModified	()const { return m_modified; }

		inline void SetLinesNumber	( unsigned int p_number)	{ m_linesNumber = p_number; }
		inline void SetLines		( TextLinesCtrl * p_lines)	{ m_lines = p_lines; }

	private:
		DECLARE_EVENT_TABLE()
		void _onKeyDown				( wxKeyEvent& p_event);
		void _onKeyUp				( wxKeyEvent& p_event);
		void _onMouseWheel			( wxMouseEvent & p_event);
		void _onScrollToTop			( wxScrollWinEvent & p_event);
		void _onScrollToBottom		( wxScrollWinEvent & p_event);
		void _onScrollLineUp		( wxScrollWinEvent & p_event);
		void _onScrollLineDown		( wxScrollWinEvent & p_event);
		void _onScrollPageUp		( wxScrollWinEvent & p_event);
		void _onScrollPageDown		( wxScrollWinEvent & p_event);
		void _onScrollThumbTrack	( wxScrollWinEvent & p_event);
		void _onScrollThumbRelease	( wxScrollWinEvent & p_event);
	};
}

#endif
