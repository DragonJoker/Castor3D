#include "SphereBoxProperties.hpp"

#include "PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( BoundingSphere )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( BoundingSphereProperty, wxPGProperty, BoundingSphere, BoundingSphere const &, TextCtrl )

		BoundingSphereProperty::BoundingSphereProperty( wxString const & label
			, wxString const & name
			, BoundingSphere const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( new Point3rProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Radius" ), wxPG_LABEL, value.getRadius() ) );
	}

	BoundingSphereProperty::~BoundingSphereProperty()
	{
	}

	void BoundingSphereProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			auto & box = BoundingSphereRefFromVariant( m_value );
			Item( 0 )->SetValue( WXVARIANT( box.getCenter() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getRadius() ) );
		}
	}
}
