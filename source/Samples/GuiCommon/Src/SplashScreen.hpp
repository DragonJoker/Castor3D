/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_SPLASH_SCREEN_H___
#define ___GUICOMMON_SPLASH_SCREEN_H___

#include "GuiCommonPrerequisites.hpp"

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
