#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Engine.hpp>
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
		: TreeItemProperty{ scene.getEngine(), editable }
		, m_scene( scene )
	{
		CreateTreeItemMenu();
	}

	void SceneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		static wxString PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );
		static wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		static wxString PROPERTY_SHADOW_LPV_INDIRECT_ATT = _( "Indirect Attenuation" );

		addProperty( grid, PROPERTY_CATEGORY_SCENE + m_scene.getName() );
		addPropertyT( grid, PROPERTY_SCENE_DEBUG_OVERLAYS, m_scene.getEngine()->getRenderLoop().hasDebugOverlays(), &m_scene.getEngine()->getRenderLoop(), &RenderLoop::showDebugOverlays );
		addPropertyT( grid, PROPERTY_SCENE_AMBIENT_LIGHT, m_scene.getAmbientLight(), &m_scene, &Scene::setAmbientLight );
		addPropertyT( grid, PROPERTY_SHADOW_LPV_INDIRECT_ATT, m_scene.getLpvIndirectAttenuation(), &m_scene, &Scene::setLpvIndirectAttenuation );

		doCreateVctProperties( editor, grid );
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

	void SceneTreeItemProperty::doCreateVctProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_VCT = _( "Voxel Cone Tracing" );
		static wxString PROPERTY_VCT_ENABLED = _( "Enable VCT" );
		static wxString PROPERTY_VCT_CONSERVATIVE_RASTERIZATION = _( "Conservative Rasterization" );
		static wxString PROPERTY_VCT_OCCLUSION = _( "Occlusion" );
		static wxString PROPERTY_VCT_TEMPORAL_SMOOTHING = _( "Temporal Smoothing" );
		static wxString PROPERTY_VCT_SECONDARY_BOUNCE = _( "Secondary Bounce" );
		static wxString PROPERTY_VCT_NUM_CONES = _( "Num. Cones" );
		static wxString PROPERTY_VCT_MAX_DISTANCE = _( "Max. Distance" );
		static wxString PROPERTY_VCT_RAY_STEP_SIZE = _( "Ray Step Size" );
		static wxString PROPERTY_VCT_VOXEL_SIZE = _( "Voxel Size" );

		auto & vctConfig = m_scene.getVoxelConeTracingConfig();
		addProperty( grid, PROPERTY_VCT );
		addPropertyT( grid, PROPERTY_VCT_ENABLED, &vctConfig.enabled );
		addPropertyT( grid, PROPERTY_VCT_CONSERVATIVE_RASTERIZATION, &vctConfig.enableConservativeRasterization );
		addPropertyT( grid, PROPERTY_VCT_OCCLUSION, &vctConfig.enableOcclusion );
		addPropertyT( grid, PROPERTY_VCT_SECONDARY_BOUNCE, &vctConfig.enableSecondaryBounce );
		addPropertyT( grid, PROPERTY_VCT_TEMPORAL_SMOOTHING, &vctConfig.enableTemporalSmoothing );
		addPropertyT( grid, PROPERTY_VCT_NUM_CONES, &vctConfig.numCones );
		addPropertyT( grid, PROPERTY_VCT_MAX_DISTANCE, &vctConfig.maxDistance );
		addPropertyT( grid, PROPERTY_VCT_RAY_STEP_SIZE, &vctConfig.rayStepSize );
		addPropertyT( grid, PROPERTY_VCT_VOXEL_SIZE, &vctConfig.voxelSizeFactor );
	}
}
