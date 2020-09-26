#include "GuiCommon/Properties/Math/PositionProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Position )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( PositionProperty, wxPGProperty, Position, Position const &, TextCtrl )

	PositionProperty::PositionProperty( wxString const & label, wxString const & name, Position const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		AddPrivateChild( addAttributes( new wxIntProperty( wxT( "X" ), wxPG_LABEL, value.x() ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( wxT( "Y" ), wxPG_LABEL, value.y() ) ) );
	}

	PositionProperty::~PositionProperty()
	{
	}

	void PositionProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Position & point = PositionRefFromVariant( m_value );
			Item( 0 )->SetValue( long( point.x() ) );
			Item( 1 )->SetValue( long( point.y() ) );
		}
	}

	wxVariant PositionProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Position & point = PositionRefFromVariant( thisValue );
		int val = childValue.GetLong();

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
