#include "CubeBoxProperties.hpp"

#include "PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( CubeBox )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( CubeBoxProperty, wxPGProperty, CubeBox, CubeBox const &, TextCtrl )

		CubeBoxProperty::CubeBoxProperty( wxString const & label
		, wxString const & name
		, CubeBox const & value )
		: wxPGProperty{ label, name }
	{
		setValueI( value );
		AddPrivateChild( new Point3rProperty( wxT( "Center" ), wxPG_LABEL, value.getCenter() ) );
		AddPrivateChild( new Point3rProperty( wxT( "Min" ), wxPG_LABEL, value.getMin() ) );
		AddPrivateChild( new Point3rProperty( wxT( "Max" ), wxPG_LABEL, value.getMax() ) );
	}

	CubeBoxProperty::~CubeBoxProperty()
	{
	}

	void CubeBoxProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			auto & box = CubeBoxRefFromVariant( m_value );
			Item( 0 )->SetValue( WXVARIANT( box.getCenter() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getMin() ) );
			Item( 1 )->SetValue( WXVARIANT( box.getMax() ) );
		}
	}
}
