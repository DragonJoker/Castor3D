#ifndef ___GC_SplashScreen___
#define ___GC_SplashScreen___

#include <wx/gauge.h>

namespace GuiCommon
{
	class SplashScreen : public wxFrame
	{
	private:
		wxBitmap m_bmpSplash;
		wxPanel * m_pBmpPanel;
		wxGauge * m_pGauge;
		String m_strCopyright;

	public:
		SplashScreen( const String & p_strTitle, const String & p_strCopyright, wxPoint p_ptPos, int p_iRange);
		~SplashScreen();

		void Step( const String & p_strText, int p_iIncrement);
		void Step( int p_iIncrement);

	protected:
		void _draw( wxDC * p_pDC);

		DECLARE_EVENT_TABLE()
		void _onPaint			( wxPaintEvent & p_event);
		void _onEraseBackground	( wxEraseEvent & p_event);
	};
}

#endif
