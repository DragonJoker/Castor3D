#ifndef ___GC_SplashScreen___
#define ___GC_SplashScreen___

#include <wx/gauge.h>

namespace GuiCommon
{
	class wxSplashScreen : public wxFrame
	{
	private:
		wxBitmap	m_bmpSplash;
		wxPanel *	m_pPanelBmp;
		wxGauge *	m_pGauge;
		wxPoint		m_ptTitlePosition;
		wxPoint		m_ptCopyrightPosition;
		wxPoint		m_ptVersionPosition;
		wxString	m_strCopyright;
		wxString	m_strEngineVersion;
		wxString	m_strStatus;
		wxString	m_strSubStatus;

	public:
		wxSplashScreen( wxWindow * p_pParent, wxString const & p_strTitle, wxString const & p_strCopyright, wxPoint const & p_ptTitlePos, wxPoint const & p_ptCopyrightPos, wxPoint const & p_ptVersionPos, wxPoint p_ptPos, int p_iRange );
		~wxSplashScreen();

		void Step( wxString const & p_strText, int p_iIncrement );
		void Step( int p_iIncrement );
		void SubStatus( wxString const & p_strText );

	protected:
		void DoDraw( wxDC * p_pDC);

		DECLARE_EVENT_TABLE()
		void OnPaint			( wxPaintEvent & p_event );
		void OnEraseBackground	( wxEraseEvent & p_event );
	};
}

#endif
