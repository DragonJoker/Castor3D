#include "SizeProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Size )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( SizeProperty, wxPGProperty, Size, const Size &, TextCtrl )

	SizeProperty::SizeProperty( wxString const & label, wxString const & name, Size const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		AddPrivateChild( new wxIntProperty( _( "Width" ), wxPG_LABEL, value.getWidth() ) );
		AddPrivateChild( new wxIntProperty( _( "Height" ), wxPG_LABEL, value.getHeight() ) );
	}

	SizeProperty::~SizeProperty()
	{
	}

	void SizeProperty::refreshChildren()
	{
		if ( GetChildCount() )
		{
			const Size & size = SizeRefFromVariant( m_value );
			Item( 0 )->SetValue( long( size.getWidth() ) );
			Item( 1 )->SetValue( long( size.getHeight() ) );
		}
	}

	wxVariant SizeProperty::childChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const
	{
		Size & size = SizeRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
		case 0:
			size.getWidth() = val;
			break;

		case 1:
			size.getHeight() = val;
			break;
		}

		wxVariant newVariant;
		newVariant << size;
		return newVariant;
	}
}
