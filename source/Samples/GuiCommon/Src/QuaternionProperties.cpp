#include "QuaternionProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace Castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Quaternion )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( QuaternionProperty, wxPGProperty, Quaternion, Quaternion const &, TextCtrl )

	QuaternionProperty::QuaternionProperty( wxString const & label, wxString const & name, Quaternion const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "W" ), wxPG_LABEL, value[3] ) );
	}

	QuaternionProperty::~QuaternionProperty()
	{
	}

	void QuaternionProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Quaternion & point = QuaternionRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant QuaternionProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Quaternion & point = QuaternionRefFromVariant( thisValue );
		float val = float( childValue.GetDouble() );

		switch ( childIndex )
		{
		case 0:
			point[0] = val;
			break;

		case 1:
			point[1] = val;
			break;

		case 2:
			point[2] = val;
			break;

		case 3:
			point[3] = val;
			break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}
}
