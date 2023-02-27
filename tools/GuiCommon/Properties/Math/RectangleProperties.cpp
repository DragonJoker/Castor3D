#include "GuiCommon/Properties/Math/RectangleProperties.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, Rectangle )
GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, HdrRgbColour )
GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, HdrRgbaColour )

namespace GuiCommon
{
	//*********************************************************************************************

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
			const castor::Rectangle & point = RectangleRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant RectangleProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Rectangle & point = RectangleRefFromVariant( thisValue );
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
		m_value = WXVARIANT( value );
	}

	//*********************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( HdrRgbColourProperty, wxPGProperty, castor::HdrRgbColour, castor::HdrRgbColour const &, TextCtrl )

		HdrRgbColourProperty::HdrRgbColourProperty( wxString const & label, wxString const & name, castor::HdrRgbColour const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );

		AddPrivateChild( addAttributes( new wxFloatProperty( _( "R" ), wxPG_LABEL, value.red() ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( _( "G" ), wxPG_LABEL, value.green() ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( _( "B" ), wxPG_LABEL, value.blue() ) ) );
	}

	void HdrRgbColourProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const castor::Point3f & point = Point3fRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
		}
	}

	wxVariant HdrRgbColourProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Point3f & point = Point3fRefFromVariant( thisValue );
		auto val = float( childValue.GetDouble() );

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
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	void HdrRgbColourProperty::setValueI( castor::HdrRgbColour const & value )
	{
		castor::Point3f var( value.red(), value.green(), value.blue() );
		m_value = WXVARIANT( var );
	}

	//*********************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( HdrRgbaColourProperty, wxPGProperty, castor::HdrRgbaColour, castor::HdrRgbaColour const &, TextCtrl )

	HdrRgbaColourProperty::HdrRgbaColourProperty( wxString const & label, wxString const & name, castor::HdrRgbaColour const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );

		AddPrivateChild( addAttributes( new wxFloatProperty( _( "R" ), wxPG_LABEL, value.red() ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( _( "G" ), wxPG_LABEL, value.green() ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( _( "B" ), wxPG_LABEL, value.blue() ) ) );
		AddPrivateChild( addAttributes( new wxFloatProperty( _( "A" ), wxPG_LABEL, value.alpha() ) ) );
	}

	void HdrRgbaColourProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const castor::Point4f & point = Point4fRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant HdrRgbaColourProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		castor::Point4f & point = Point4fRefFromVariant( thisValue );
		auto val = float( childValue.GetDouble() );

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

	void HdrRgbaColourProperty::setValueI( castor::HdrRgbaColour const & value )
	{
		castor::Point4f var( value.red(), value.green(), value.blue(), value.alpha() );
		m_value = WXVARIANT( var );
	}
}
