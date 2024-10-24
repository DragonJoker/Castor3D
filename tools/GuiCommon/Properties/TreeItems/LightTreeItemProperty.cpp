#include "GuiCommon/Properties/TreeItems/LightTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	LightTreeItemProperty::LightTreeItemProperty( bool editable, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
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
