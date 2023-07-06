#include "GuiCommon/Properties/TreeItems/SceneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	SceneTreeItemProperty::SceneTreeItemProperty( wxWindow * parent
		, bool editable
		, castor3d::Scene & scene )
		: TreeItemProperty{ scene.getEngine(), editable }
		, m_scene( scene )
	{
		CreateTreeItemMenu();
	}

	void SceneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SCENE = _( "Scene: " );
		static wxString PROPERTY_SCENE_AMBIENT_LIGHT = _( "Ambient light" );
		static wxString PROPERTY_SHADOW_LPV_INDIRECT_ATT = _( "Indirect Attenuation" );

		addProperty( grid, PROPERTY_CATEGORY_SCENE + m_scene.getName() );
		addPropertyT( grid, PROPERTY_SCENE_AMBIENT_LIGHT, m_scene.getAmbientLight(), &m_scene, &castor3d::Scene::setAmbientLight );
		addPropertyT( grid, PROPERTY_SHADOW_LPV_INDIRECT_ATT, m_scene.getLpvIndirectAttenuation(), &m_scene, &castor3d::Scene::setLpvIndirectAttenuation );

		doCreateFogProperties( editor, grid );
		doCreateVctProperties( editor, grid );
	}

	void SceneTreeItemProperty::doCreateFogProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_FOG = _( "Fog" );
		static wxString PROPERTY_FOG_TYPE = _( "Type" );
		static wxString PROPERTY_FOG_DISABLED = _( "None" );
		static wxString PROPERTY_FOG_LINEAR = _( "Linear" );
		static wxString PROPERTY_FOG_EXPONENTIAL = _( "Exponential" );
		static wxString PROPERTY_FOG_SQUARED_EXPONENTIAL = _( "Squared Exponential" );
		static wxString PROPERTY_FOG_DENSITY = _( "Density" );

		wxArrayString choices;
		choices.push_back( PROPERTY_FOG_DISABLED );
		choices.push_back( PROPERTY_FOG_LINEAR );
		choices.push_back( PROPERTY_FOG_EXPONENTIAL );
		choices.push_back( PROPERTY_FOG_SQUARED_EXPONENTIAL );

		auto & fogConfig = m_scene.getFog();
		addProperty( grid, PROPERTY_FOG );
		addPropertyE( grid, PROPERTY_FOG_TYPE, choices, fogConfig.getType()
			, [&fogConfig]( castor3d::FogType value ){ fogConfig.setType( value ); } );
		addPropertyT( grid, PROPERTY_FOG_DENSITY, fogConfig.getDensity(), &fogConfig, &castor3d::Fog::setDensity );
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

	void SceneTreeItemProperty::onDebugOverlaysChange( wxVariant const & var )
	{
		m_scene.getEngine()->getRenderLoop().showDebugOverlays( var.GetBool() );
	}

	void SceneTreeItemProperty::onAmbientLightChange( wxVariant const & var )
	{
		wxColour colour;
		colour << var;
		m_scene.setAmbientLight( castor::RgbColour::fromBGR( colour.GetRGB() ) );
	}
}
