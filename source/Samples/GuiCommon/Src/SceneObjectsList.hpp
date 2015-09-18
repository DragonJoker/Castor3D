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
	class SceneObjectsList
		: public wxTreeCtrl
	{
	public:
		SceneObjectsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~SceneObjectsList();

		void LoadScene( Castor3D::Engine * p_pEngine, Castor3D::SceneSPtr p_pScene );
		void UnloadScene();

	protected:
		void DoAddRenderWindow( wxTreeItemId p_id, Castor3D::RenderWindowSPtr p_window );
		void DoAddGeometry( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_geometry );
		void DoAddCamera( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_camera );
		void DoAddLight( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_light );
		void DoAddNode( wxTreeItemId p_id, Castor3D::SceneNodeSPtr p_node );
		void DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlaySPtr p_overlay );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnSelectItem( wxTreeEvent & p_event );
		void OnMouseRButtonUp( wxTreeEvent & p_event );

	private:
		Castor3D::SceneWPtr m_pScene;
		Castor3D::Engine * m_pEngine;
		PropertiesHolder * m_propertiesHolder;
	};
}

#endif
