#include "AdditionalProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace Castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Size )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Position )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Quaternion )

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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( PositionProperty, wxPGProperty, Position, Position const &, TextCtrl )

	PositionProperty::PositionProperty( wxString const & label, wxString const & name, Position const & value )
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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( RectangleProperty, wxPGProperty, Castor::Point4i, Castor::Point4i const &, TextCtrl )

	RectangleProperty::RectangleProperty( wxString const & label, wxString const & name, Castor::Point4i const & value )
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
		Castor::Point4i & point = Point4iRefFromVariant( thisValue );
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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point2fProperty, wxPGProperty, Point2f, Point2f const &, TextCtrl )

	Point2fProperty::Point2fProperty( wxString const & label, wxString const & name, Point2f const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
	}

	Point2fProperty::~Point2fProperty()
	{
	}

	void Point2fProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point2f & point = Point2fRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
		}
	}

	wxVariant Point2fProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point2f & point = Point2fRefFromVariant( thisValue );
		float val = float( childValue.GetDouble() );

		switch ( childIndex )
		{
		case 0:
			point[0] = val;
			break;

		case 1:
			point[1] = val;
			break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point3fProperty, wxPGProperty, Point3f, Point3f const &, TextCtrl )

	Point3fProperty::Point3fProperty( wxString const & label, wxString const & name, Point3f const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
	}

	Point3fProperty::~Point3fProperty()
	{
	}

	void Point3fProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point3f & point = Point3fRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
		}
	}

	wxVariant Point3fProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point3f & point = Point3fRefFromVariant( thisValue );
		float val = float( childValue.GetDouble() );

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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point4fProperty, wxPGProperty, Point4f, Point4f const &, TextCtrl )

	Point4fProperty::Point4fProperty( wxString const & label, wxString const & name, Point4f const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "W" ), wxPG_LABEL, value[3] ) );
	}

	Point4fProperty::~Point4fProperty()
	{
	}

	void Point4fProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point4f & point = Point4fRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant Point4fProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point4f & point = Point4fRefFromVariant( thisValue );
		float val = float( childValue.GetDouble() );

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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point2dProperty, wxPGProperty, Point2d, Point2d const &, TextCtrl )

	Point2dProperty::Point2dProperty( wxString const & label, wxString const & name, Point2d const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
	}

	Point2dProperty::~Point2dProperty()
	{
	}

	void Point2dProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point2d & point = Point2dRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
		}
	}

	wxVariant Point2dProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point2d & point = Point2dRefFromVariant( thisValue );
		double val = childValue.GetDouble();

		switch ( childIndex )
		{
		case 0:
			point[0] = val;
			break;

		case 1:
			point[1] = val;
			break;
		}

		wxVariant newVariant;
		newVariant << point;
		return newVariant;
	}

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point3dProperty, wxPGProperty, Point3d, Point3d const &, TextCtrl )

	Point3dProperty::Point3dProperty( wxString const & label, wxString const & name, Point3d const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
	}

	Point3dProperty::~Point3dProperty()
	{
	}

	void Point3dProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point3d & point = Point3dRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
		}
	}

	wxVariant Point3dProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point3d & point = Point3dRefFromVariant( thisValue );
		double val = childValue.GetDouble();

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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( Point4dProperty, wxPGProperty, Point4d, Point4d const &, TextCtrl )

	Point4dProperty::Point4dProperty( wxString const & label, wxString const & name, Point4d const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		AddPrivateChild( new wxFloatProperty( wxT( "X" ), wxPG_LABEL, value[0] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Y" ), wxPG_LABEL, value[1] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "Z" ), wxPG_LABEL, value[2] ) );
		AddPrivateChild( new wxFloatProperty( wxT( "W" ), wxPG_LABEL, value[3] ) );
	}

	Point4dProperty::~Point4dProperty()
	{
	}

	void Point4dProperty::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			const Point4d & point = Point4dRefFromVariant( m_value );
			Item( 0 )->SetValue( point[0] );
			Item( 1 )->SetValue( point[1] );
			Item( 2 )->SetValue( point[2] );
			Item( 3 )->SetValue( point[3] );
		}
	}

	wxVariant Point4dProperty::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		Point4d & point = Point4dRefFromVariant( thisValue );
		double val = childValue.GetDouble();

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

	//************************************************************************************************

	WX_PG_IMPLEMENT_PROPERTY_CLASS( QuaternionProperty, wxPGProperty, Quaternion, Quaternion const &, TextCtrl )

	QuaternionProperty::QuaternionProperty( wxString const & label, wxString const & name, Quaternion const & value )
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
		float val = float( childValue.GetDouble() );

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

	//************************************************************************************************
}
