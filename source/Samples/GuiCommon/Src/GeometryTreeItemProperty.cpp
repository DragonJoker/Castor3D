#include "GeometryTreeItemProperty.hpp"

#include <Geometry.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
	}

	wxGeometryTreeItemProperty::wxGeometryTreeItemProperty( Castor3D::GeometrySPtr p_pGeometry )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_GEOMETRY )
		, m_pGeometry( p_pGeometry )
	{
	}

	wxGeometryTreeItemProperty::~wxGeometryTreeItemProperty()
	{
	}

	void wxGeometryTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		GeometrySPtr l_geometry = GetGeometry();

		if ( l_geometry )
		{
		}
	}

	void wxGeometryTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		GeometrySPtr l_geometry = GetGeometry();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_geometry )
		{
		}
	}
}
