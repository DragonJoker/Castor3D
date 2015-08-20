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
#ifndef ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___
#define ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/panel.h>
#include <wx/treectrl.h>

namespace GuiCommon
{
	class wxGeometriesListFrame
		: public wxPanel
	{
	public:
		typedef enum eBMP
		{
			eBMP_VISIBLE,
			eBMP_VISIBLE_SEL,
			eBMP_HIDDEN,
			eBMP_HIDDEN_SEL,
			eBMP_GEOMETRY,
			eBMP_GEOMETRY_SEL,
			eBMP_GEOMETRY_OPEN,
			eBMP_GEOMETRY_OPEN_SEL,
			eBMP_SUBMESH,
			eBMP_SUBMESH_SEL,
			eBMP_COUNT,
		}	eBMP;

	private:
		typedef enum eID
		{
			eID_TREE_GEOMETRIES,
			eID_COMBO_MATERIALS,
			eID_BUTTON_DELETE,
		}	eID;

		typedef enum eTREE_ID
		{
			eTREE_ID_VISIBLE,
			eTREE_ID_VISIBLE_SEL,
			eTREE_ID_HIDDEN,
			eTREE_ID_HIDDEN_SEL,
			eTREE_ID_GEOMETRY,
			eTREE_ID_GEOMETRY_SEL,
			eTREE_ID_GEOMETRY_OPEN,
			eTREE_ID_GEOMETRY_OPEN_SEL,
			eTREE_ID_SUBMESH,
			eTREE_ID_SUBMESH_SEL,
			eTREE_ID_COUNT,
		}	eTREE_ID;

		wxTreeCtrl * m_pTreeGeometries;
		wxButton * m_pButtonDeleteSelected;
		wxComboBox * m_pComboMaterials;
		wxImageList * m_pListImages;
		wxArrayString m_arrayItems;
		std::set< wxTreeItemId > m_setGeometriesInTree;
		std::set< wxTreeItemId > m_setSubmeshesInTree;
		Castor3D::SceneWPtr m_pScene;
		uint32_t m_uiNbItems;
		Castor3D::Engine * m_pEngine;
		wxTreeItemId m_selItem;

	public:
		wxGeometriesListFrame( wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxGeometriesListFrame();
		
		void LoadScene( Castor3D::Engine * p_pEngine, Castor3D::SceneSPtr p_pScene );
		void UnloadScene();

	protected:
		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnDeleteItem( wxCommandEvent & p_event );
		void OnExpandItem( wxTreeEvent & p_event );
		void OnCollapseItem( wxTreeEvent & p_event );
		void OnActivateItem( wxTreeEvent & p_event );
		void OnChangeItem( wxTreeEvent & p_event );
		void OnComboMaterials( wxCommandEvent & p_event );
	};
}

#endif
