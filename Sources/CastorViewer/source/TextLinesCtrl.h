#ifndef ___Troll_TextLinesCtrl___
#define ___Troll_TextLinesCtrl___

class wxStaticText;

namespace CastorViewer
{
	class TextCtrl;
	class TextPanel;

	class TextLinesCtrl : public wxPanel
	{
	private:
		int m_nbLines;
		wxListBox * m_lines;

	public:
		TextLinesCtrl( wxWindow *parent, wxWindowID id, const wxPoint& pos, 
					   const wxSize& size, long style);

		void AddLines( unsigned int p_nbLines);
		void RemoveLine();
		void GoToLine( int p_line);

	private:
		DECLARE_EVENT_TABLE()
		void _onDoubleClick( wxMouseEvent & p_event);
	};
}

#endif