/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_SPLASH_SCREEN_H___
#define ___GUICOMMON_SPLASH_SCREEN_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/frame.h>

namespace GuiCommon
{
	class wxSplashScreen
		:	public wxFrame
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
		void DoDraw( wxDC * p_pDC );

		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
	};
}

#endif
