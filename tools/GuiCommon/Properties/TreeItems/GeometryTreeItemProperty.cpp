#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
	}

	GeometryTreeItemProperty::GeometryTreeItemProperty( bool p_editable, castor3d::Geometry & p_geometry )
		: TreeItemProperty( p_geometry.getScene()->getEngine(), p_editable )
		, m_geometry( p_geometry )
	{
		PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );

		CreateTreeItemMenu();
	}

	GeometryTreeItemProperty::~GeometryTreeItemProperty()
	{
	}

	void GeometryTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry.getName() ) );
	}
}
