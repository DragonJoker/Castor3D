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
		SceneObjectsList( PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~SceneObjectsList();

		void loadScene( castor3d::Engine * engine
			, castor3d::SceneSPtr scene );
		void unloadScene();
		void select( castor3d::GeometrySPtr geometry
			, castor3d::SubmeshSPtr submesh );

	protected:
		void doAddSubmesh( castor3d::GeometrySPtr geometry
			, castor3d::SubmeshSPtr submesh
			, wxTreeItemId id );
		void doAddSkeleton( castor3d::Skeleton const & skeleton
			, wxTreeItemId id );
		void doAddRenderWindow( wxTreeItemId id
			, castor3d::RenderWindowSPtr window );
		void doAddGeometry( wxTreeItemId id
			, castor3d::Geometry & geometry );
		void doAddCamera( wxTreeItemId id
			, castor3d::Camera & camera );
		void doAddLight( wxTreeItemId id
			, castor3d::Light & light );
		void doAddBillboard( wxTreeItemId id
			, castor3d::BillboardList & billboard );
		void doAddNode( wxTreeItemId id
			, castor3d::SceneNodeSPtr node );
		void doAddAnimatedObjectGroup( wxTreeItemId id
			, castor3d::AnimatedObjectGroupSPtr group );
		void doAddOverlay( wxTreeItemId id
			, castor3d::OverlayCategorySPtr overlay );

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );
		void onMouseRButtonUp( wxTreeEvent & event );

	private:
		castor3d::SceneWPtr m_scene;
		castor3d::Engine * m_engine;
		PropertiesContainer * m_propertiesHolder;
		GeometrySubmeshIdMap m_ids;
	};
}

#endif
