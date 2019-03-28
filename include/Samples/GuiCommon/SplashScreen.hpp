/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SPLASH_SCREEN_H___
#define ___GUICOMMON_SPLASH_SCREEN_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/frame.h>

namespace GuiCommon
{
	class SplashScreen
		: public wxFrame
	{
	public:
		SplashScreen( wxString const & p_strTitle, wxPoint const & p_ptTitlePos, wxPoint const & p_ptCopyrightPos, wxPoint const & p_ptVersionPos, wxPoint p_ptPos, int p_iRange, castor3d::Version const & p_version );
		~SplashScreen();

		void Step( wxString const & p_strText, int p_iIncrement );
		void Step( int p_iIncrement );
		void SubStatus( wxString const & p_strText );

	protected:
		void doDraw( wxDC * p_pDC );

		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );

	private:
		wxBitmap m_bmpSplash;
		wxPanel * m_pPanelBmp;
		wxGauge * m_pGauge;
		wxPoint m_ptTitlePosition;
		wxPoint m_ptCopyrightPosition;
		wxPoint m_ptVersionPosition;
		wxString m_strCopyright;
		wxString m_strEngineVersion;
		wxString m_strStatus;
		wxString m_strSubStatus;
	};
}

#endif
