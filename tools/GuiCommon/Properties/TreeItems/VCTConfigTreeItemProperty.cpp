#include "GuiCommon/Properties/TreeItems/VCTConfigTreeItemProperty.hpp"

#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	VCTConfigTreeItemProperty::VCTConfigTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::VoxelSceneData & config )
		: TreeItemProperty{ engine, editable }
		, m_config{ config }
	{
		CreateTreeItemMenu();
	}

	void VCTConfigTreeItemProperty::doCreateProperties( wxPGEditor * editor
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

		auto & vctConfig = m_config;
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
