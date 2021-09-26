#include "GuiCommon/Properties/Math/RectangleProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, Rectangle )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( RectangleProperty, wxPGProperty, castor::Rectangle, castor::Rectangle const &, TextCtrl )

	RectangleProperty::RectangleProperty( wxString const & label, wxString const & name, castor::Rectangle const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );

		AddPrivateChild( addAttributes( new wxIntProperty( _( "Left" ), wxPG_LABEL, value.left() ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( _( "Top" ), wxPG_LABEL, value.top() ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( _( "Right" ), wxPG_LABEL, value.right() ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( _( "Bottom" ), wxPG_LABEL, value.bottom() ) ) );
	}

	void RectangleProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const castor::Point4i & point = Point4iRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant RectangleProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Point4i & point = Point4iRefFromVariant( thisValue );
		auto val = int( childValue.GetLong() );

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

	void RectangleProperty::setValueI( castor::Rectangle const & value )
	{
		castor::Point4i var( value.left(), value.top(), value.right(), value.bottom() );
		m_value = WXVARIANT( var );
	}
}
