/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDERER_SELECTOR_H___
#define ___GUICOMMON_RENDERER_SELECTOR_H___

#define CV_IMG_CASTOR	1268

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/dialog.h>

namespace GuiCommon
{
	class RendererSelector
		:	public wxDialog
	{
	private:
		enum ID
		{
			ID_LIST_RENDERERS,
		};

	public:
		RendererSelector( castor3d::Engine & engine
			, wxWindow * parent
			, wxString const & title );
		virtual ~RendererSelector();

		castor::String getSelectedRenderer()const;

	private:
		void doDraw( wxDC * dc );
		void doSelect();

	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & event );
		void OnKeyUp( wxKeyEvent & event );
		void OnButtonOk( wxCommandEvent & event );
		void OnButtonCancel( wxCommandEvent & event );

	private:
		wxImage * m_castorImg;
		wxListBox * m_renderers;
		castor3d::Engine & m_engine;
		std::vector< castor::String > m_names;
	};
}

#endif
