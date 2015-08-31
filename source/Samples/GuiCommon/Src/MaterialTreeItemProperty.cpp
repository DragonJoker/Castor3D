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

	wxMaterialTreeItemProperty::wxMaterialTreeItemProperty( MaterialSPtr p_material )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_MATERIAL )
		, m_material( p_material )
	{
	}

	wxMaterialTreeItemProperty::~wxMaterialTreeItemProperty()
	{
	}

	void wxMaterialTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		MaterialSPtr l_material = GetMaterial();

		if ( l_material )
		{
		}
	}

	void wxMaterialTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		MaterialSPtr l_material = GetMaterial();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_material )
		{
		}
	}
}
