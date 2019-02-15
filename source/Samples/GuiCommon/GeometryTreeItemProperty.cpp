#include "GeometryTreeItemProperty.hpp"

#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );
	}

	GeometryTreeItemProperty::GeometryTreeItemProperty( bool p_editable, castor3d::Geometry & p_geometry )
		: TreeItemProperty( p_geometry.getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_GEOMETRY )
		, m_geometry( p_geometry )
	{
		PROPERTY_CATEGORY_GEOMETRY = _( "Geometry: " );

		CreateTreeItemMenu();
	}

	GeometryTreeItemProperty::~GeometryTreeItemProperty()
	{
	}

	void GeometryTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_GEOMETRY + wxString( m_geometry.getName() ) ) );
	}

	void GeometryTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
		}
	}
}
