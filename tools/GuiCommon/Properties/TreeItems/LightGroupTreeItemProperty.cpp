#include "GuiCommon/Properties/TreeItems/LightGroupTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Scene/Light/LightGroup.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	LightGroupTreeItemProperty::LightGroupTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
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
