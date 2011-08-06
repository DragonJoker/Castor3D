#ifndef ___GC_TextLinesCtrl___
#define ___GC_TextLinesCtrl___

class wxStaticText;

namespace GuiCommon
{
	class wxTextEditor;
	class wxTextPanel;

	class wxTextLinesCtrl : public wxPanel
	{
	private:
		int m_nbLines;
		wxListBox * m_lines;

	public:
		wxTextLinesCtrl( wxWindow * p_parent, wxWindowID p_id, const wxPoint & p_pos,
					     const wxSize & p_size, long p_style);

		void AddLines( unsigned int p_nbLines);
		void RemoveLine();
		void GoToLine( int p_line);

	private:
		DECLARE_EVENT_TABLE()
		void _onDoubleClick( wxMouseEvent & p_event);
	};
}

#endif
