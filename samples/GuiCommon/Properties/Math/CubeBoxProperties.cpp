#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( BoundingBox )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( BoundingBoxProperty, wxPGProperty, BoundingBox, BoundingBox const &, TextCtrl )

		BoundingBoxProperty::BoundingBoxProperty( wxString const & label
		, wxString const & name
		, BoundingBox const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) ) );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Min" ), wxPG_LABEL, value.getMin() ) ) );
		AddPrivateChild( addAttributes( new Point3fProperty( wxT( "Max" ), wxPG_LABEL, value.getMax() ) ) );
	}

	BoundingBoxProperty::~BoundingBoxProperty()
	{
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
