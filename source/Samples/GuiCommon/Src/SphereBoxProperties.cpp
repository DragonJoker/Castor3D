#include "SphereBoxProperties.hpp"

#include "PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( SphereBox )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( SphereBoxProperty, wxPGProperty, SphereBox, SphereBox const &, TextCtrl )

		SphereBoxProperty::SphereBoxProperty( wxString const & label
			, wxString const & name
			, SphereBox const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( new Point3rProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Radius" ), wxPG_LABEL, value.getRadius() ) );
	}

	SphereBoxProperty::~SphereBoxProperty()
	{
	}

	void SphereBoxProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			auto & box = SphereBoxRefFromVariant( m_value );
			Item( 0 )->SetValue( WXVARIANT( box.getCenter() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getRadius() ) );
		}
	}
}
