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
		RendererSelector( Castor3D::Engine * p_engine, wxWindow * p_parent, wxString const & p_strTitle );
		virtual ~RendererSelector();

		Castor::String GetSelectedRenderer()const;

	private:
		void DoDraw( wxDC * p_pDC );
		void DoSelect();

	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnButtonOk( wxCommandEvent & p_event );
		void OnButtonCancel( wxCommandEvent & p_event );

	private:
		wxImage * m_pImgCastor;
		wxListBox * m_pListRenderers;
		Castor3D::Engine * m_engine;
	};
}

#endif
