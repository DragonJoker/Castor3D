#include "GuiCommon/MaterialTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"

#include <Castor3D/Material/Material.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
	}

	MaterialTreeItemProperty::MaterialTreeItemProperty( bool p_editable, MaterialSPtr p_material )
		: TreeItemProperty( p_material->getEngine(), p_editable, ePROPERTY_DATA_TYPE_MATERIAL )
		, m_material( p_material )
	{
		PROPERTY_CATEGORY_MATERIAL = _( "Material: " );

		CreateTreeItemMenu();
	}

	MaterialTreeItemProperty::~MaterialTreeItemProperty()
	{
	}

	void MaterialTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		MaterialSPtr material = getMaterial();

		if ( material )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_MATERIAL + wxString( material->getName() ) ) );
		}
	}

	void MaterialTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		MaterialSPtr material = getMaterial();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && material )
		{
		}
	}
}
