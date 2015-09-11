#include "SizeProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace Castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Size )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( SizeProperty, wxPGProperty, Size, const Size &, TextCtrl )

	SizeProperty::SizeProperty( wxString const & label, wxString const & name, Size const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxIntProperty( _( "Width" ), wxPG_LABEL, value.width() ) );
		AddPrivateChild( new wxIntProperty( _( "Height" ), wxPG_LABEL, value.height() ) );
	}

	SizeProperty::~SizeProperty()
	{
	}

	void SizeProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Size & size = SizeRefFromVariant( m_value );
			Item( 0 )->SetValue( long( size.width() ) );
			Item( 1 )->SetValue( long( size.height() ) );
		}
	}

	wxVariant SizeProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const
	{
		Size & size = SizeRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
		case 0:
			size.width() = val;
			break;

		case 1:
			size.height() = val;
			break;
		}

		wxVariant newVariant;
		newVariant << size;
		return newVariant;
	}
}
