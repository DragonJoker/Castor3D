#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
	}

	GeometryTreeItemProperty::GeometryTreeItemProperty( bool editable, castor3d::Geometry & geometry )
		: TreeItemProperty( geometry.getScene()->getEngine(), editable )
		, m_geometry( geometry )
	{
		PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );

		CreateTreeItemMenu();
	}

	void GeometryTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry.getName() ) );
	}
}
