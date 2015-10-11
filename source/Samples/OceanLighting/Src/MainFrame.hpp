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
		Castor3D::RenderWindowSPtr	m_pWindow;
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
