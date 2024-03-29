#include "GuiCommon/Properties/Math/QuaternionProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, Quaternion )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( QuaternionProperty, wxPGProperty, castor::Quaternion, castor::Quaternion const &, TextCtrl )

	QuaternionProperty::QuaternionProperty( wxString const & label
		, wxString const & name
		, castor::Quaternion const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		value.toAxisAngle( m_axis, m_angle );
		AddPrivateChild( addAttributes( new wxFloatProperty( wxT( "Axis X" ), wxPG_LABEL, m_axis[0] ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( wxT( "Axis Y" ), wxPG_LABEL, m_axis[1] ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( wxT( "Axis Z" ), wxPG_LABEL, m_axis[2] ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( wxT( "Angle" ), wxPG_LABEL, m_angle.degrees() ) ) );
	}

	void QuaternionProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const castor::Quaternion & quat = QuaternionRefFromVariant( m_value );
			quat.toAxisAngle( m_axis, m_angle );
			Item( 0 )->SetValue( m_axis[0] );
			Item( 1 )->SetValue( m_axis[1] );
			Item( 2 )->SetValue( m_axis[2] );
			Item( 3 )->SetValue( m_angle.degrees() );
		}
	}

	wxVariant QuaternionProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Quaternion & quat = QuaternionRefFromVariant( thisValue );
		auto axis = m_axis;
		auto angle = m_angle;
		float val = float( childValue.GetDouble() );

		switch ( childIndex )
		{
		case 0:
			axis[0] = val;
			break;

		case 1:
			axis[1] = val;
			break;

		case 2:
			axis[2] = val;
			break;

		case 3:
			angle.degrees( val );
			break;
		}

		quat = castor::Quaternion::fromAxisAngle( axis, angle );
		wxVariant newVariant;
		newVariant << quat;
		return newVariant;
	}
}
