#include "GuiCommon/Properties/Math/ColourProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, HdrRgbColour )
GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, HdrRgbaColour )

namespace GuiCommon
{
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
			const auto & colour = HdrRgbColourRefFromVariant( m_value );
			Item( 0 )->SetValue( float( colour.red() ) );
			Item( 1 )->SetValue( float( colour.green() ) );
			Item( 2 )->SetValue( float( colour.blue() ) );
		}
	}

	wxVariant HdrRgbColourProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		auto & colour = HdrRgbColourRefFromVariant( thisValue );
		auto val = float( childValue.GetDouble() );

		switch ( childIndex )
		{
		case 0:
			colour.red() = val;
			break;

		case 1:
			colour.green() = val;
			break;

		case 2:
			colour.blue() = val;
			break;
		}

		wxVariant newVariant;
		newVariant << colour;
		return newVariant;
	}

	void HdrRgbColourProperty::setValueI( castor::HdrRgbColour const & value )
	{
		m_value = WXVARIANT( value );
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
			const auto & colour = HdrRgbaColourRefFromVariant( m_value );
			Item( 0 )->SetValue( float( colour.red() ) );
			Item( 1 )->SetValue( float( colour.green() ) );
			Item( 2 )->SetValue( float( colour.blue() ) );
			Item( 3 )->SetValue( float( colour.alpha() ) );
		}
	}

	wxVariant HdrRgbaColourProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		auto & colour = HdrRgbaColourRefFromVariant( thisValue );
		auto val = float( childValue.GetDouble() );

		switch ( childIndex )
		{
		case 0:
			colour.red() = val;
			break;

		case 1:
			colour.green() = val;
			break;

		case 2:
			colour.blue() = val;
			break;

		case 3:
			colour.alpha() = val;
			break;
		}

		wxVariant newVariant;
		newVariant << colour;
		return newVariant;
	}

	void HdrRgbaColourProperty::setValueI( castor::HdrRgbaColour const & value )
	{
		m_value = WXVARIANT( value );
	}

	//*********************************************************************************************
}
