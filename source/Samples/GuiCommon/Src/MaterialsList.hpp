/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___GUICOMMON_MATERIALS_LIST_VIEW_H___
#define ___GUICOMMON_MATERIALS_LIST_VIEW_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class MaterialsList
		: public wxTreeCtrl
	{
	public:
		MaterialsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~MaterialsList();

		void LoadMaterials( Castor3D::Engine * p_engine );
		void UnloadMaterials();

	private:
		void DoAddMaterial( wxTreeItemId p_id, Castor3D::MaterialSPtr p_material );
		void DoAddPass( wxTreeItemId p_id, uint32_t p_index, Castor3D::PassSPtr p_pass );
		void DoAddTexture( wxTreeItemId p_id, uint32_t p_index, Castor3D::TextureUnitSPtr p_texture );

		DECLARE_EVENT_TABLE();
		void OnClose( wxCloseEvent & p_event );
		void OnSelectItem( wxTreeEvent & p_event );
		void OnMouseRButtonUp( wxTreeEvent & p_event );

	private:
		Castor3D::Engine * m_engine;
		PropertiesHolder * m_propertiesHolder;
	};
}

#endif
