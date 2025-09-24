#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Scene/Light/Light.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	LightTreeItemProperty::LightTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void LightTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		if ( m_light )
		{
			TreeItemConfigurationBuilder::submit( grid, *this, *m_light );
		}
	}
}
