/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDERER_SELECTOR_H___
#define ___GUICOMMON_RENDERER_SELECTOR_H___

#define CV_IMG_CASTOR	1268

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

#include <wx/dialog.h>

namespace GuiCommon
{
	class RendererSelector
		:	public wxDialog
	{
	public:
		RendererSelector( castor3d::Engine & engine
			, wxWindow * parent
			, wxString const & title );

		castor3d::Renderer getSelected();

	private:
		wxListBox * doFillRenderers();
		wxListBox * doInitialiseDevices();
		void doFillDevices( castor3d::Renderer const & renderer );
		void doDraw( wxDC * dc );
		void doSelectRenderer( bool next );
		void doSelectDevice( bool next);

		DECLARE_EVENT_TABLE()
		void onPaint( wxPaintEvent & event );
		void onDevicesKeyUp( wxKeyEvent & event );
		void onRenderersKeyUp( wxKeyEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void onButtonOk( wxCommandEvent & event );
		void onButtonCancel( wxCommandEvent & event );
		void onSelectRenderer( wxCommandEvent & event );
		void onSelectDevice( wxCommandEvent & event );

	private:
		wxImage * m_castorImg;
		wxListBox * m_renderersList;
		wxListBox * m_devicesList;
		castor3d::Engine & m_engine;
		std::vector< castor3d::Renderer > m_renderers;
		castor3d::Renderer * m_currentRenderer{};
	};
}

#endif
