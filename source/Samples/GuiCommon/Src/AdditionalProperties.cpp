#include "AdditionalProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace Castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Size )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Position )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2r )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3r )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4r )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Quaternion )

namespace GuiCommon
{
	WX_PG_IMPLEMENT_PROPERTY_CLASS( SizeProperty, wxPGProperty, Size, const Size &,TextCtrl )

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
			case 0: size.width() = val; break;
			case 1: size.height() = val; break;
		}

		wxVariant newVariant;
		newVariant << size;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( PositionProperty, wxPGProperty, Position, Position const &,TextCtrl )

	PositionProperty::PositionProperty( wxString const & label, wxString const & name, Position const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxIntProperty( wxT( "X" ), wxPG_LABEL, value.x() ) );
		AddPrivateChild( new wxIntProperty( wxT( "Y" ), wxPG_LABEL, value.y() ) );
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
		Position& point = PositionRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point.x() = val; break;
			case 1: point.y() = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( RectangleProperty, wxPGProperty, Castor::Point4i, Castor::Point4i const &,TextCtrl )

	RectangleProperty::RectangleProperty( wxString const & label, wxString const & name, Castor::Point4i const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxIntProperty( _( "Left" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxIntProperty( _( "Top" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxIntProperty( _( "Right" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxIntProperty( _( "Bottom" ), wxPG_LABEL, value[3] ) );
	}

	RectangleProperty::~RectangleProperty()
	{
	}

	void RectangleProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Castor::Point4i & point = Point4iRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant RectangleProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Castor::Point4i& point = Point4iRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point[0] = val; break;
			case 1: point[1] = val; break;
			case 2: point[2] = val; break;
			case 3: point[3] = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point2rProperty, wxPGProperty, Point2r, Point2r const &,TextCtrl )

	Point2rProperty::Point2rProperty( wxString const & label, wxString const & name, Point2r const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
	}

	Point2rProperty::~Point2rProperty()
	{
	}

	void Point2rProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point2r & point = Point2rRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
		}
	}

	wxVariant Point2rProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point2r & point = Point2rRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point[0] = val; break;
			case 1: point[1] = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point3rProperty, wxPGProperty, Point3r, Point3r const &,TextCtrl )

	Point3rProperty::Point3rProperty( wxString const & label, wxString const & name, Point3r const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
	}

	Point3rProperty::~Point3rProperty()
	{
	}

	void Point3rProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point3r & point = Point3rRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
		}
	}

	wxVariant Point3rProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point3r & point = Point3rRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point[0] = val; break;
			case 1: point[1] = val; break;
			case 2: point[2] = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point4rProperty, wxPGProperty, Point4r, Point4r const &,TextCtrl )

	Point4rProperty::Point4rProperty( wxString const & label, wxString const & name, Point4r const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "W" ), wxPG_LABEL, value[3] ) );
	}

	Point4rProperty::~Point4rProperty()
	{
	}

	void Point4rProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point4r & point = Point4rRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant Point4rProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point4r & point = Point4rRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point[0] = val; break;
			case 1: point[1] = val; break;
			case 2: point[2] = val; break;
			case 3: point[3] = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( QuaternionProperty, wxPGProperty, Quaternion, Quaternion const &,TextCtrl )

	QuaternionProperty::QuaternionProperty( wxString const & label, wxString const & name, Quaternion const & value)
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "W" ), wxPG_LABEL, value[3] ) );
	}

	QuaternionProperty::~QuaternionProperty()
	{
	}

	void QuaternionProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Quaternion & point = QuaternionRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant QuaternionProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Quaternion & point = QuaternionRefFromVariant( thisValue );
		int val = childValue.GetLong();

		switch ( childIndex )
		{
			case 0: point[0] = val; break;
			case 1: point[1] = val; break;
			case 2: point[2] = val; break;
			case 3: point[3] = val; break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************
}
