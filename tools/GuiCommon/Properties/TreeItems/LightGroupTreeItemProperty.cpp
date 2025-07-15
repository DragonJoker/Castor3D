#include "GuiCommon/Properties/TreeItems/LightGroupTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Scene/Light/LightGroup.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	LightGroupTreeItemProperty::LightGroupTreeItemProperty( bool editable, c3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void LightGroupTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		if ( m_lightGroup )
		{
			TreeItemConfigurationBuilder::submit( grid, *this, *m_lightGroup );
		}
	}
}
