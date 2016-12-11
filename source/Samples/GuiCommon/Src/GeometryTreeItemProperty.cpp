#include "GeometryTreeItemProperty.hpp"

#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
	}

	GeometryTreeItemProperty::GeometryTreeItemProperty( bool p_editable, Castor3D::Geometry & p_geometry )
		: TreeItemProperty( p_geometry.GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_GEOMETRY )
		, m_geometry( p_geometry )
	{
		PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );

		CreateTreeItemMenu();
	}

	GeometryTreeItemProperty::~GeometryTreeItemProperty()
	{
	}

	void GeometryTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry.GetName() ) ) );
	}

	void GeometryTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
		}
	}
}
