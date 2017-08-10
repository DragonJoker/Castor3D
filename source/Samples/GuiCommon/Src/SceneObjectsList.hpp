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
		SceneObjectsList( PropertiesHolder * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~SceneObjectsList();

		void LoadScene( castor3d::Engine * engine, castor3d::SceneSPtr p_scene );
		void UnloadScene();

	protected:
		void doAddRenderWindow( wxTreeItemId p_id, castor3d::RenderWindowSPtr p_window );
		void doAddGeometry( wxTreeItemId p_id, castor3d::Geometry & p_geometry );
		void doAddCamera( wxTreeItemId p_id, castor3d::Camera & p_camera );
		void doAddLight( wxTreeItemId p_id, castor3d::Light & p_light );
		void doAddBillboard( wxTreeItemId p_id, castor3d::BillboardList & p_billboard );
		void doAddNode( wxTreeItemId p_id, castor3d::SceneNodeSPtr p_node );
		void doAddAnimatedObjectGroup( wxTreeItemId p_id, castor3d::AnimatedObjectGroupSPtr p_group );
		void doAddOverlay( wxTreeItemId p_id, castor3d::OverlayCategorySPtr p_overlay );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnSelectItem( wxTreeEvent & p_event );
		void OnMouseRButtonUp( wxTreeEvent & p_event );

	private:
		castor3d::SceneWPtr m_scene;
		castor3d::Engine * m_engine;
		PropertiesHolder * m_propertiesHolder;
	};
}

#endif
