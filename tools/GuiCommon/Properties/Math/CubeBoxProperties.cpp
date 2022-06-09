#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 5054 )
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#pragma warning( pop )

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, BoundingBox )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( BoundingBoxProperty, wxPGProperty
		, castor::BoundingBox
		, castor::BoundingBox const &
		, TextCtrl )

		BoundingBoxProperty::BoundingBoxProperty( wxString const & label
		, wxString const & name
		, castor::BoundingBox const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) ) );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Min" ), wxPG_LABEL, value.getMin() ) ) );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Max" ), wxPG_LABEL, value.getMax() ) ) );
	}

	void BoundingBoxProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			auto & box = BoundingBoxRefFromVariant( m_value );
			Item( 0 )->SetValue( WXVARIANT( box.getCenter() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getMin() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getMax() ) );
		}
	}
}
