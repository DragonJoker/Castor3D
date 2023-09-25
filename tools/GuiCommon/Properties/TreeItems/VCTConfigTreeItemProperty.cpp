#include "GuiCommon/Properties/TreeItems/VCTConfigTreeItemProperty.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	VCTConfigTreeItemProperty::VCTConfigTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::VctConfig & config )
		: TreeItemProperty{ engine, editable }
		, m_config{ config }
	{
		CreateTreeItemMenu();
	}

	void VCTConfigTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		TreeItemConfigurationBuilder::submit( grid, *this, m_config );
	}
}
