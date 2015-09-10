#include "AdditionalProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace Castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Size )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Position )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix2x2f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix3x3f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix4x4f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix2x2d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix3x3d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix4x4d )
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

	ButtonData::ButtonData( ButtonEventMethod p_method, wxEvtHandler * p_handler )
		: m_method( p_method )
		, m_handler( p_handler )
	{
	}

	bool ButtonData::Call( wxPGProperty * property )
	{
		return ( *m_handler.*m_method )( property );
	}

	//************************************************************************************************

	wxPGWindowList ButtonEventEditor::CreateControls( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxPoint const & p_pos, wxSize const & p_size )const
	{
		// create and return a single button to be used as editor
		// size and pos represent the entire value cell: use that to position the button
		return wxPGWindowList( new wxButton( p_propgrid, wxPG_SUBID1, _( "Edit" ), p_pos, p_size ) );
	}

	// since the editor does not need to change the primary control (the button)
	// to reflect changes, UpdateControl is just a no-op
	void ButtonEventEditor::UpdateControl(wxPGProperty* property, wxWindow* ctrl) const
	{
	}

	bool ButtonEventEditor::OnEvent( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxWindow * p_wnd_primary, wxEvent & p_event )const
	{
		// handle the button event
		if( p_event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
		{
			// extract the client data from the property
			if( ButtonData * btn = dynamic_cast< ButtonData * >( p_property->GetClientObject() ) )
			{
				// call the method
				return btn->Call( p_property );
			}
		}

		return false;
	}

	//************************************************************************************************

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2bProperty, wxPGProperty, Point2b, Point2b const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3bProperty, wxPGProperty, Point3b, Point3b const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4bProperty, wxPGProperty, Point4b, Point4b const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2iProperty, wxPGProperty, Point2i, Point2i const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3iProperty, wxPGProperty, Point3i, Point3i const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4iProperty, wxPGProperty, Point4i, Point4i const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2uiProperty, wxPGProperty, Point2ui, Point2ui const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3uiProperty, wxPGProperty, Point3ui, Point3ui const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4uiProperty, wxPGProperty, Point4ui, Point4ui const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2fProperty, wxPGProperty, Point2f, Point2f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3fProperty, wxPGProperty, Point3f, Point3f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4fProperty, wxPGProperty, Point4f, Point4f const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2dProperty, wxPGProperty, Point2d, Point2d const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3dProperty, wxPGProperty, Point3d, Point3d const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4dProperty, wxPGProperty, Point4d, Point4d const &, TextCtrl )

	//************************************************************************************************
}
