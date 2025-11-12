#include "GuiCommon/Properties/Math/PositionProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( c3d, Position )

namespace GuiCommon
{
	wxPG_IMPLEMENT_PROPERTY_CLASS( PositionProperty, wxPGProperty, TextCtrl )

	PositionProperty::PositionProperty( wxString const & label
		, wxString const & name
		, c3d::Position const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		AddPrivateChild( addAttributes( new wxIntProperty( wxT( "X" ), wxPG_LABEL, value.x() ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( wxT( "Y" ), wxPG_LABEL, value.y() ) ) );
	}

	void PositionProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const c3d::Position & point = PositionRefFromVariant( m_value );
			Item( 0 )->SetValue( long( point.x() ) );
			Item( 1 )->SetValue( long( point.y() ) );
		}
	}

	wxVariant PositionProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		c3d::Position & point = PositionRefFromVariant( thisValue );
		auto val = int( childValue.GetLong() );

		switch ( childIndex )
		{
		case 0:
			point.x() = val;
			break;

		case 1:
			point.y() = val;
			break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}
}
