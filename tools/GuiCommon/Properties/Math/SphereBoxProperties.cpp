#include "GuiCommon/Properties/Math/SphereBoxProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, BoundingSphere )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( BoundingSphereProperty, wxPGProperty, castor::BoundingSphere, castor::BoundingSphere const &, TextCtrl )

		BoundingSphereProperty::BoundingSphereProperty( wxString const & label
			, wxString const & name
			, castor::BoundingSphere const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( new Point3fProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( wxT( "Radius" ), wxPG_LABEL, value.getRadius() ) ) );
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
