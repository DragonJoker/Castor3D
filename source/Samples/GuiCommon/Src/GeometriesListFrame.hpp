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
		: public wxTreeCtrl
	{
	public:
		typedef enum eBMP
		{
			eBMP_SCENE,
			eBMP_SCENE_SEL,
			eBMP_NODE,
			eBMP_NODE_SEL,
			eBMP_CAMERA,
			eBMP_CAMERA_SEL,
			eBMP_GEOMETRY,
			eBMP_GEOMETRY_SEL,
			eBMP_LIGHT,
			eBMP_LIGHT_SEL,
			eBMP_SUBMESH,
			eBMP_SUBMESH_SEL,
			eBMP_OVERLAY,
			eBMP_OVERLAY_SEL,
			eBMP_COUNT,
		}	eBMP;

	public:
		wxGeometriesListFrame( wxWindow * p_propsParent, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxGeometriesListFrame();
		
		void LoadScene( Castor3D::Engine * p_pEngine, Castor3D::SceneSPtr p_pScene );
		void UnloadScene();

	protected:
		void DoAddGeometry( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_geometry );
		void DoAddCamera( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_camera );
		void DoAddLight( wxTreeItemId p_id, Castor3D::MovableObjectSPtr p_light );
		void DoAddNode( wxTreeItemId p_id, Castor3D::SceneNodeSPtr p_node );
		void DoAddOverlay( wxTreeItemId p_id, Castor3D::OverlaySPtr p_overlay );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnExpandItem( wxTreeEvent & p_event );
		void OnCollapseItem( wxTreeEvent & p_event );
		void OnActivateItem( wxTreeEvent & p_event );
		void OnChangeItem( wxTreeEvent & p_event );

	private:
		Castor3D::SceneWPtr m_pScene;
		Castor3D::Engine * m_pEngine;
	};
}

#endif
