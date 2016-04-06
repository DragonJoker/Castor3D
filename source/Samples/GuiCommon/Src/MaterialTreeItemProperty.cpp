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
		static wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
	}

	MaterialTreeItemProperty::MaterialTreeItemProperty( bool p_editable, MaterialSPtr p_material )
		: TreeItemProperty( p_material->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_MATERIAL )
		, m_material( p_material )
	{
		PROPERTY_CATEGORY_MATERIAL = _( "Material: " );

		CreateTreeItemMenu();
	}

	MaterialTreeItemProperty::~MaterialTreeItemProperty()
	{
	}

	void MaterialTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		MaterialSPtr l_material = GetMaterial();

		if ( l_material )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_MATERIAL + wxString( l_material->GetName() ) ) );
		}
	}

	void MaterialTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		MaterialSPtr l_material = GetMaterial();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_material )
		{
		}
	}
}
