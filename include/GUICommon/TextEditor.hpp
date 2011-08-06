#ifndef ___GC_ShaderEditor___
#define ___GC_ShaderEditor___

namespace GuiCommon
{
	class wxTextLinesCtrl;
	class wxTextPanel;

	class wxTextEditor : public wxTextCtrl
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
		wxTextLinesCtrl * m_lines;
		wxTextPanel * m_panel;
		int m_deltaLines;
		int m_deltaPages;
		bool m_modified;

	public:
		wxTextEditor( wxWindow * p_parent, wxWindowID p_id, const wxString & p_value, const wxPoint & p_pos, const wxSize & p_size, long p_style);
		~wxTextEditor();

		void AutoWord();
		virtual void SaveFile( const wxString & p_path, int p_fileType = 0);
		virtual bool LoadFile( const wxString & filename, int fileType = wxTEXT_TYPE_ANY);
		void GoToLine( int p_line);
		void CompleteCode( int p_keyCode);
		void AdjustLines();

		inline bool		IsModified	()const { return m_modified; }

		inline void SetLinesNumber	( unsigned int p_number)		{ m_linesNumber = p_number; }
		inline void SetLines		( wxTextLinesCtrl * p_lines)	{ m_lines = p_lines; }
		inline void SetTextPanel	( wxTextPanel * p_pPanel)		{ m_panel = p_pPanel; }

	private:
		DECLARE_EVENT_TABLE()
		void _onKeyDown				( wxKeyEvent & p_event);
		void _onKeyUp				( wxKeyEvent & p_event);
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
