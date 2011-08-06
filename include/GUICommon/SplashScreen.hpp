#ifndef ___GC_SplashScreen___
#define ___GC_SplashScreen___

#include <wx/gauge.h>

namespace GuiCommon
{
	class wxSplashScreen : public wxFrame
	{
	private:
		wxBitmap m_bmpSplash;
		wxPanel * m_pBmpPanel;
		wxGauge * m_pGauge;
		wxString m_strCopyright;
		wxString m_strEngineVersion;
		wxString m_strStatus;
		wxString m_strSubStatus;

	public:
		wxSplashScreen( wxWindow * p_pParent, const wxString & p_strTitle, const wxString & p_strCopyright, wxPoint p_ptPos, int p_iRange);
		~wxSplashScreen();

		void Step( const wxString & p_strText, int p_iIncrement);
		void Step( int p_iIncrement);
		void SubStatus( const wxString & p_strText);

	protected:
		void _draw( wxDC * p_pDC);

		DECLARE_EVENT_TABLE()
		void _onPaint			( wxPaintEvent & p_event);
		void _onEraseBackground	( wxEraseEvent & p_event);
	};
}

#endif
