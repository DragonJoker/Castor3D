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
#ifndef ___GUICOMMON_MATERIALS_LIST_FRAME_H___
#define ___GUICOMMON_MATERIALS_LIST_FRAME_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/panel.h>
#include <wx/treectrl.h>

namespace GuiCommon
{
	class wxMaterialsListFrame
		: public wxPanel
	{
	public:
		wxMaterialsListFrame( wxWindow * p_propsParent, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxMaterialsListFrame();
		
		void LoadMaterials( Castor3D::Engine * p_pEngine );
		void UnloadMaterials();

	protected:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnSelected( wxListEvent & p_event );

	private:
		wxMaterialPanel * m_materialPanel;
		wxMaterialsListView * m_pMaterialsList;
		Castor3D::Engine * m_engine;
	};
}

#endif
