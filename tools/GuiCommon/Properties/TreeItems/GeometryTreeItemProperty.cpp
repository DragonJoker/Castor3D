#include "GuiCommon/Properties/TreeItems/GeometryTreeItemProperty.hpp"

#include <Castor3D/Scene/Geometry.hpp>

namespace GuiCommon
{
	GeometryTreeItemProperty::GeometryTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void GeometryTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
		static wxString PROPERTY_CATEGORY_GEOMETRY_NODE = _( "Node: " );

		if ( !m_geometry )
		{
			return;
		}

		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry->getName() ) );
		addProperty( grid, PROPERTY_CATEGORY_GEOMETRY_NODE + wxString( m_geometry->getParent()->getName() ) );
	}
}
