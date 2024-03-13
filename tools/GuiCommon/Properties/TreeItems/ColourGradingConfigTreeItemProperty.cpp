#include "GuiCommon/Properties/TreeItems/ColourGradingConfigTreeItemProperty.hpp"

#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ColourGradingConfigTreeItemProperty::ColourGradingConfigTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::ColourGradingConfig & config )
		: TreeItemProperty{ engine, editable }
		, m_config{ config }
	{
		CreateTreeItemMenu();
	}

	void ColourGradingConfigTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		TreeItemConfigurationBuilder::submit( grid, *this, m_config );
	}
}
