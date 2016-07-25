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
#ifndef ___C3D_OCEAN_LIGHTING_MAIN_FRAME_H___
#define ___C3D_OCEAN_LIGHTING_MAIN_FRAME_H___

#ifndef TW_CALL
#	define TW_CALL
#endif

#include <wx/app.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/cmdline.h>
#include <wx/colordlg.h>
#include <wx/clrpicker.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/display.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include <wx/valtext.h>
#include <wx/aui/framemanager.h>

#include "Technique.hpp"

namespace OceanLighting
{
	DECLARE_SMART_PTR( RenderTechnique );

	class MainFrame : public wxFrame
	{
	private:
		int							m_oldx;
		int							m_oldy;
		bool						m_drag;
		int							m_width;
		int							m_height;
		wxTimer  		*			m_timer;
		Castor3D::Engine 	*		m_pCastor3D;
		Castor3D::RenderWindowSPtr	m_window;
		RenderTechniqueSPtr			m_pTechnique;

	public:
		MainFrame();
		virtual ~MainFrame();

		bool Initialise();

	private:
		bool DoInitialise3D( wxWindow * p_parent );

		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
		void OnClose( wxCloseEvent & p_event );
		void OnTimer( wxTimerEvent & p_event );
		void OnSize( wxSizeEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnMouseLDown( wxMouseEvent & p_event );
		void OnMouseLUp( wxMouseEvent & p_event );
		void OnMouseMove( wxMouseEvent & p_event );
		void OnMouseLeave( wxMouseEvent & p_event );

		// ----------------------------------------------------------------------------
		// Callbacks used by TweakAPI (not used anymore but kept in case of)
		// ----------------------------------------------------------------------------
		void TW_CALL getBool( void * value, void * clientData );
		void TW_CALL setBool( const void * value, void * clientData );
		void TW_CALL computeSlopeVarianceTex( void * unused );
		void TW_CALL getFloat( void * value, void * clientData );
		void TW_CALL setFloat( const void * value, void * clientData );
		void TW_CALL getInt( void * value, void * clientData );
		void TW_CALL setInt( const void * value, void * clientData );
		void TW_CALL getBool2( void * value, void * clientData );
		void TW_CALL setBool2( const void * value, void * clientData );
	};
}

#endif
