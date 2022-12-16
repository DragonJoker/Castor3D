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

		void Step( wxString const & strText, int iIncrement );
		void Step( int iIncrement );
		void SubStatus( wxString const & strText );

	protected:
		void doDraw( wxDC * pDC );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void OnPaint( wxPaintEvent & event );
		void OnEraseBackground( wxEraseEvent & event );

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
