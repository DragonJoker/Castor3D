#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Material/Material.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	MaterialTreeItemProperty::MaterialTreeItemProperty( bool p_editable, castor3d::Material & p_material )
		: TreeItemProperty( p_material.getEngine(), p_editable )
		, m_material( &p_material )
	{
		CreateTreeItemMenu();
	}

	void MaterialTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );

		if ( m_material )
		{
			addProperty( grid, PROPERTY_CATEGORY_MATERIAL + wxString( m_material->getName() ) );
		}
	}
}
