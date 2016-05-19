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

	GeometryTreeItemProperty::GeometryTreeItemProperty( bool p_editable, Castor3D::GeometrySPtr p_pGeometry )
		: TreeItemProperty( p_pGeometry->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_GEOMETRY )
		, m_pGeometry( p_pGeometry )
	{
		PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );

		CreateTreeItemMenu();
	}

	GeometryTreeItemProperty::~GeometryTreeItemProperty()
	{
	}

	void GeometryTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		GeometrySPtr l_geometry = GetGeometry();

		if ( l_geometry )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_GEOMETRY + wxString( l_geometry->GetName() ) ) );
		}
	}

	void GeometryTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		GeometrySPtr l_geometry = GetGeometry();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_geometry )
		{
		}
	}
}
