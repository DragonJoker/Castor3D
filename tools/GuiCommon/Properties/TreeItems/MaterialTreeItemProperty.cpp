#include "GuiCommon/Properties/TreeItems/MaterialTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Material/Material.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	MaterialTreeItemProperty::MaterialTreeItemProperty( bool editable, c3d::Engine * engine )
		: TreeItemProperty( engine, editable )
	{
		CreateTreeItemMenu();
	}

	void MaterialTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
		static wxString PROPERTY_MATERIAL_VISIBLE = _( "Visible" );

		if ( m_material )
		{
			addProperty( grid, PROPERTY_CATEGORY_MATERIAL + wxString( m_material->getName() ) );
			addPropertyT( grid, PROPERTY_MATERIAL_VISIBLE, m_material->isVisible(), m_material, &c3d::Material::setVisible );
		}
	}
}
