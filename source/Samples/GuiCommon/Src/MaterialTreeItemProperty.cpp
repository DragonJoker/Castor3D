#include "MaterialTreeItemProperty.hpp"

#include <Material/Material.hpp>

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
		MaterialSPtr material = GetMaterial();

		if ( material )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_MATERIAL + wxString( material->GetName() ) ) );
		}
	}

	void MaterialTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		MaterialSPtr material = GetMaterial();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && material )
		{
		}
	}
}
