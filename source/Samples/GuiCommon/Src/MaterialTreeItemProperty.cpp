#include "MaterialTreeItemProperty.hpp"

#include <Material.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
	}

	MaterialTreeItemProperty::MaterialTreeItemProperty( bool p_editable, MaterialSPtr p_material )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_MATERIAL )
		, m_material( p_material )
	{
	}

	MaterialTreeItemProperty::~MaterialTreeItemProperty()
	{
	}

	void MaterialTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		MaterialSPtr l_material = GetMaterial();

		if ( l_material )
		{
		}
	}

	void MaterialTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		MaterialSPtr l_material = GetMaterial();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_material )
		{
		}
	}
}
