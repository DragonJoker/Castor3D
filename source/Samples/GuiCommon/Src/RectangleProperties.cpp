#include "RectangleProperties.hpp"

#include "PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( RectangleProperty, wxPGProperty, castor::Rectangle, castor::Rectangle const &, TextCtrl )

	RectangleProperty::RectangleProperty( wxString const & label, wxString const & name, castor::Rectangle const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		AddPrivateChild( new wxIntProperty( _( "Left" ), wxPG_LABEL, value.left() ) );
		AddPrivateChild( new wxIntProperty( _( "Top" ), wxPG_LABEL, value.top() ) );
		AddPrivateChild( new wxIntProperty( _( "Right" ), wxPG_LABEL, value.right() ) );
		AddPrivateChild( new wxIntProperty( _( "Bottom" ), wxPG_LABEL, value.bottom() ) );
	}

	RectangleProperty::~RectangleProperty()
	{
	}

	void RectangleProperty::refreshChildren()
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

	wxVariant RectangleProperty::childChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Point4i & point = Point4iRefFromVariant( thisValue );
		int val = childValue.GetLong();

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
