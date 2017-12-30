/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDERER_SELECTOR_H___
#define ___GUICOMMON_RENDERER_SELECTOR_H___

#define CV_IMG_CASTOR	1268

#include "GuiCommonPrerequisites.hpp"

#include <wx/dialog.h>

namespace GuiCommon
{
	class RendererSelector
		:	public wxDialog
	{
	private:
		typedef enum eID
		{
			eID_BUTTON_OK,
			eID_BUTTON_CANCEL,
			eID_LIST_RENDERERS,
		}	eID;

	public:
		RendererSelector( castor3d::Engine * engine, wxWindow * p_parent, wxString const & p_strTitle );
		virtual ~RendererSelector();

		castor::String getSelectedRenderer()const;

	private:
		void doDraw( wxDC * p_pDC );
		void doSelect();

	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnButtonOk( wxCommandEvent & p_event );
		void OnButtonCancel( wxCommandEvent & p_event );

	private:
		wxImage * m_pImgCastor;
		wxListBox * m_pListRenderers;
		castor3d::Engine * m_engine;
	};
}

#endif
