#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( castor, Size )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( SizeProperty, wxPGProperty, castor::Size, const castor::Size &, TextCtrl )

	SizeProperty::SizeProperty( wxString const & label
		, wxString const & name
		, castor::Size const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		AddPrivateChild( addAttributes( new wxIntProperty( _( "Width" ), wxPG_LABEL, long( value.getWidth() ) ) ) );
		AddPrivateChild( addAttributes( new wxIntProperty( _( "Height" ), wxPG_LABEL, long( value.getHeight() ) ) ) );
	}

	void SizeProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const castor::Size & size = SizeRefFromVariant( m_value );
			Item( 0 )->SetValue( long( size.getWidth() ) );
			Item( 1 )->SetValue( long( size.getHeight() ) );
		}
	}

	wxVariant SizeProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const
	{
		castor::Size & size = SizeRefFromVariant( thisValue );
		auto val = int( childValue.GetLong() );

		switch ( childIndex )
		{
		case 0:
			size.getWidth() = uint32_t( val );
			break;

		case 1:
			size.getHeight() = uint32_t( val );
			break;
		}

		wxVariant newVariant;
		newVariant << size;
		return newVariant;
	}
}
