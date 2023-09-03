#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	GeometryTreeItemProperty::GeometryTreeItemProperty( bool editable, castor3d::Geometry & geometry )
		: TreeItemProperty( geometry.getScene()->getEngine(), editable )
		, m_geometry( geometry )
	{
		CreateTreeItemMenu();
	}

	void GeometryTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
		static wxString PROPERTY_CATEGORY_GEOMETRY_NODE = _( "Node: " );

		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry.getName() ) );
		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY_NODE + wxString( m_geometry.getParent()->getName() ) );
	}
}
