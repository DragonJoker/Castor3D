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

	private:
		wxImage * m_pImgCastor;
		wxListBox * m_pListRenderers;
		wxButton * m_pBtnOk;
		wxButton * m_pBtnCancel;
		Castor3D::Engine * m_pEngine;

	public:
		RendererSelector( Castor3D::Engine * p_pEngine, wxWindow * p_pParent, wxString const & p_strTitle );
		virtual ~RendererSelector();

		Castor3D::eRENDERER_TYPE GetSelectedRenderer()const;

	private:
		void DoDraw( wxDC * p_pDC );
		void DoSelect();

	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnButtonOk( wxCommandEvent & p_event );
		void OnButtonCancel( wxCommandEvent & p_event );
	};
}

#endif
