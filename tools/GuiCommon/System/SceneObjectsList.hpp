/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___
#define ___GUICOMMON_GEOMETRIES_LIST_FRAME_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <Castor3D/Gui/GuiModule.hpp>

#include <wx/panel.h>
#include <wx/treectrl.h>

namespace GuiCommon
{
	class SceneObjectsList final
		: public wxTreeCtrl
	{
	private:
		using SubmeshIdMap = std::map< castor3d::Submesh const *, wxTreeItemId >;
		using GeometrySubmeshIdMap = std::map< castor3d::GeometryRPtr, SubmeshIdMap >;
		using MaterialIdMap = std::map< castor3d::MaterialRPtr, wxTreeItemId >;

	public:
		SceneObjectsList( PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );

		void loadScene( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneMaterials( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneOverlays( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneGui( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneNodes( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneLights( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void loadSceneObjects( castor3d::Engine * engine
			, castor3d::RenderWindow & window
			, castor3d::SceneRPtr scene );
		void unloadScene();
		void select( castor3d::GeometryRPtr geometry
			, castor3d::Submesh const * submesh );
		void select( castor3d::MaterialRPtr material );

	protected:
		void doAddSubmesh( wxTreeItemId id
			, castor3d::GeometryRPtr geometry
			, castor3d::Submesh const * submesh );
		void doAddSkeleton( wxTreeItemId id
			, castor3d::Skeleton const & skeleton );
		void doAddGeometry( wxTreeItemId id
			, castor3d::Geometry & geometry );
		void doAddCamera( wxTreeItemId id
			, castor3d::Camera & camera );
		void doAddBillboard( wxTreeItemId id
			, castor3d::BillboardList & billboard );
		void doAddNode( wxTreeItemId id
			, castor3d::SceneNode & node
			, uint32_t & count );
		void doAddAnimatedObjectGroup( wxTreeItemId id
			, castor3d::AnimatedObjectGroup & group );
		void doAddOverlay( wxTreeItemId id
			, castor3d::OverlayCategory & overlay );
		void doAddStyles( wxTreeItemId id
			, castor3d::StylesHolder const & styles
			, castor3d::SceneRPtr scene );
		void doAddStyle( wxTreeItemId id
			, castor::String const & name
			, castor3d::ControlStyle & style
			, castor3d::SceneRPtr scene );
		void doAddControl( wxTreeItemId id
			, castor::String const & name
			, castor3d::Control & control
			, bool full
			, bool inLayout );
		void doAddMaterial( wxTreeItemId id
			, castor3d::MaterialObs material );
		void doAddPass( wxTreeItemId id
			, uint32_t index
			, castor3d::Pass & pass );
		void doAddTexture( wxTreeItemId id
			, uint32_t index
			, castor3d::Pass & pass
			, castor3d::TextureUnit & texture );
		void doLoadSceneLights( wxTreeItemId parentId
			, castor3d::SceneRPtr scene
			, wxString const & name
			, castor3d::LightType type
			, int icon
			, int iconSel
			, uint32_t & count );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );

	private:
		castor3d::SceneRPtr m_scene{};
		castor3d::Engine * m_engine{};
		PropertiesContainer * m_propertiesHolder{};
		wxImageList m_images{};
		GeometrySubmeshIdMap m_objects{};
		MaterialIdMap m_materials{};
	};
}

#endif
