#include "ViewportTreeItemProperty.hpp"

#include <Viewport.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
		static const wxString PROPERTY_VIEWPORT_TYPE = _( "Type" );
		static const wxString PROPERTY_VIEWPORT_TYPE_3D = _( "3D" );
		static const wxString PROPERTY_VIEWPORT_TYPE_2D = _( "2D" );
		static const wxString PROPERTY_VIEWPORT_SIZE = _( "Size" );
		static const wxString PROPERTY_VIEWPORT_TOP = _( "Top" );
		static const wxString PROPERTY_VIEWPORT_BOTTOM = _( "Bottom" );
		static const wxString PROPERTY_VIEWPORT_LEFT = _( "Left" );
		static const wxString PROPERTY_VIEWPORT_RIGHT = _( "Right" );
		static const wxString PROPERTY_VIEWPORT_NEAR = _( "Near" );
		static const wxString PROPERTY_VIEWPORT_FAR = _( "Far" );
		static const wxString PROPERTY_VIEWPORT_FOVY = _( "FOV Y" );
		static const wxString PROPERTY_VIEWPORT_RATIO = _( "Ratio" );
	}

	ViewportTreeItemProperty::ViewportTreeItemProperty( bool p_editable, Castor3D::ViewportSPtr p_viewport )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_VIEWPORT )
		, m_viewport( p_viewport )
	{
	}

	ViewportTreeItemProperty::~ViewportTreeItemProperty()
	{
	}

	void ViewportTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		ViewportSPtr l_viewport = GetViewport();

		if ( l_viewport )
		{
			wxString l_selected;
			wxPGChoices l_choices;
			l_choices.Add( PROPERTY_VIEWPORT_TYPE_3D );
			l_choices.Add( PROPERTY_VIEWPORT_TYPE_2D );

			switch ( l_viewport->GetType() )
			{
			case eVIEWPORT_TYPE_3D:
				l_selected = PROPERTY_VIEWPORT_TYPE_3D;
				break;

			case eVIEWPORT_TYPE_2D:
				l_selected = PROPERTY_VIEWPORT_TYPE_2D;
				break;
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VIEWPORT ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_VIEWPORT_TYPE, PROPERTY_VIEWPORT_TYPE, l_choices ) )->SetValue( l_selected );
			p_grid->Append( new SizeProperty( PROPERTY_VIEWPORT_SIZE ) )->SetValue( wxVariant( l_viewport->GetSize() ) );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_TOP ) )->SetValue( l_viewport->GetTop() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_BOTTOM ) )->SetValue( l_viewport->GetBottom() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_LEFT ) )->SetValue( l_viewport->GetLeft() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RIGHT ) )->SetValue( l_viewport->GetRight() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_NEAR ) )->SetValue( l_viewport->GetNear() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FAR ) )->SetValue( l_viewport->GetFar() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FOVY ) )->SetValue( l_viewport->GetFovY().Degrees() );
			p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RATIO ) )->SetValue( l_viewport->GetRatio() );
		}
	}

	void ViewportTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		ViewportSPtr l_viewport = GetViewport();

		if ( l_property && l_viewport )
		{
			if ( l_property->GetName() == PROPERTY_VIEWPORT_TYPE )
			{
				if ( l_property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_3D )
				{
					OnTypeChange( eVIEWPORT_TYPE_3D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_2D )
				{
					OnTypeChange( eVIEWPORT_TYPE_2D );
				}
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_SIZE )
			{
				OnSizeChange( SizeRefFromVariant( l_property->GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_TOP )
			{
				OnTopChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_BOTTOM )
			{
				OnBottomChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_LEFT )
			{
				OnLeftChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_RIGHT )
			{
				OnRightChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_NEAR )
			{
				OnNearChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_FAR )
			{
				OnFarChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_FOVY )
			{
				OnFovYChange( l_property->GetValue() );
			}
			else if ( l_property->GetName() == PROPERTY_VIEWPORT_RATIO )
			{
				OnRatioChange( l_property->GetValue() );
			}
		}
	}

	void ViewportTreeItemProperty::OnTypeChange( eVIEWPORT_TYPE p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetType( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnSizeChange( Castor::Size const & p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetSize( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnTopChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetTop( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnBottomChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetBottom( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnLeftChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetLeft( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnRightChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetRight( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnNearChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetNear( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnFarChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetFar( p_value );
		} ) );
	}

	void ViewportTreeItemProperty::OnFovYChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetFovY( Angle::FromDegrees( p_value ) );
		} ) );
	}

	void ViewportTreeItemProperty::OnRatioChange( double p_value )
	{
		ViewportSPtr l_viewport = GetViewport();

		l_viewport->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_viewport]()
		{
			l_viewport->SetRatio( p_value );
		} ) );
	}
}
