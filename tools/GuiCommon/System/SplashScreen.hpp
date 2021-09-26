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
		SplashScreen( wxString const & title
			, wxPoint const & titlePos
			, wxPoint const & copyrightPos
			, wxPoint const & versionPos
			, wxPoint const & pos
			, int range
			, castor3d::Version const & appVersion
			, castor3d::Version const & engineVersion );

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
		wxString m_strAppVersion;
		wxString m_strEngineVersion;
		wxString m_strStatus;
		wxString m_strSubStatus;
	};
}

#endif
