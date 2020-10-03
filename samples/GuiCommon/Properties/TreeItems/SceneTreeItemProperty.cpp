#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	SceneTreeItemProperty::SceneTreeItemProperty( wxWindow * parent
		, bool editable
		, Scene & scene )
		: TreeItemProperty{ scene.getEngine(), editable, ePROPERTY_DATA_TYPE_SCENE }
		, m_parent{ parent }
		, m_scene( scene )
	{
		CreateTreeItemMenu();
	}

	SceneTreeItemProperty::~SceneTreeItemProperty()
	{
	}

	void SceneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		static wxString PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );
		static wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );

		addProperty( grid, PROPERTY_CATEGORY_SCENE + m_scene.getName() );
		addPropertyT( grid, PROPERTY_SCENE_DEBUG_OVERLAYS, m_scene.getEngine()->getRenderLoop().hasDebugOverlays(), &m_scene.getEngine()->getRenderLoop(), &RenderLoop::showDebugOverlays );
		addPropertyT( grid, PROPERTY_SCENE_AMBIENT_LIGHT, m_scene.getAmbientLight(), &m_scene, &Scene::setAmbientLight );
	}

	void SceneTreeItemProperty::onDebugOverlaysChange( wxVariant const & var )
	{
		m_scene.getEngine()->getRenderLoop().showDebugOverlays( var.GetBool() );
	}

	void SceneTreeItemProperty::onAmbientLightChange( wxVariant const & var )
	{
		wxColour colour;
		colour << var;
		m_scene.setAmbientLight( RgbColour::fromBGR( colour.GetRGB() ) );
	}
}
