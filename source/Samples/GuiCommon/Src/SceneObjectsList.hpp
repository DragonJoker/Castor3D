/*
See LICENSE file in root folder
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
	private:
		using SubmeshIdMap = std::map< castor3d::SubmeshSPtr, wxTreeItemId >;
		using GeometrySubmeshIdMap = std::map< castor3d::GeometrySPtr, SubmeshIdMap >;

	public:
		SceneObjectsList( PropertiesContainer * p_propertiesHolder, wxWindow * p_parent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~SceneObjectsList();

		void loadScene( castor3d::Engine * engine, castor3d::SceneSPtr p_scene );
		void unloadScene();
		void select( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh );

	protected:
		void doAddSubmesh( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh, wxTreeItemId id );
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
		PropertiesContainer * m_propertiesHolder;
		GeometrySubmeshIdMap m_ids;
	};
}

#endif
