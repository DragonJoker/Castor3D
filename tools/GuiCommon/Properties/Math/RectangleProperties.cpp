#include "GuiCommon/Properties/Math/RectangleProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( c3d, Rectangle )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( RectangleProperty, wxPGProperty, c3d::Rectangle, c3d::Rectangle const &, TextCtrl )

	RectangleProperty::RectangleProperty( wxString const & label, wxString const & name, c3d::Rectangle const & value )
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
			const c3d::Rectangle & point = RectangleRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant RectangleProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		c3d::Rectangle & point = RectangleRefFromVariant( thisValue );
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

	void RectangleProperty::setValueI( c3d::Rectangle const & value )
	{
		m_value = WXVARIANT( value );
	}
}
