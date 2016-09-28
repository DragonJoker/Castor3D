#include "ViewportTreeItemProperty.hpp"

#include <Render/Viewport.hpp>
#include <Event/Frame/FunctorEvent.hpp>

#include "SizeProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
		static wxString PROPERTY_VIEWPORT_TYPE = _( "Type" );
		static wxString PROPERTY_VIEWPORT_TYPE_PERSPECTIVE = _( "Perspective" );
		static wxString PROPERTY_VIEWPORT_TYPE_FRUSTUM = _( "Frustum" );
		static wxString PROPERTY_VIEWPORT_TYPE_ORTHO = _( "Ortho" );
		static wxString PROPERTY_VIEWPORT_SIZE = _( "Size" );
		static wxString PROPERTY_VIEWPORT_TOP = _( "Top" );
		static wxString PROPERTY_VIEWPORT_BOTTOM = _( "Bottom" );
		static wxString PROPERTY_VIEWPORT_LEFT = _( "Left" );
		static wxString PROPERTY_VIEWPORT_RIGHT = _( "Right" );
		static wxString PROPERTY_VIEWPORT_NEAR = _( "Near" );
		static wxString PROPERTY_VIEWPORT_FAR = _( "Far" );
		static wxString PROPERTY_VIEWPORT_FOVY = _( "FOV Y" );
		static wxString PROPERTY_VIEWPORT_RATIO = _( "Ratio" );
	}

	ViewportTreeItemProperty::ViewportTreeItemProperty( bool p_editable, Castor3D::Engine & p_engine, Castor3D::Viewport & p_viewport )
		: TreeItemProperty( &p_engine, p_editable, ePROPERTY_DATA_TYPE_VIEWPORT )
		, m_viewport( p_viewport )
	{
		PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
		PROPERTY_VIEWPORT_TYPE = _( "Type" );
		PROPERTY_VIEWPORT_TYPE_PERSPECTIVE = _( "Perspective" );
		PROPERTY_VIEWPORT_TYPE_FRUSTUM = _( "Frustum" );
		PROPERTY_VIEWPORT_TYPE_ORTHO = _( "Ortho" );
		PROPERTY_VIEWPORT_SIZE = _( "Size" );
		PROPERTY_VIEWPORT_TOP = _( "Top" );
		PROPERTY_VIEWPORT_BOTTOM = _( "Bottom" );
		PROPERTY_VIEWPORT_LEFT = _( "Left" );
		PROPERTY_VIEWPORT_RIGHT = _( "Right" );
		PROPERTY_VIEWPORT_NEAR = _( "Near" );
		PROPERTY_VIEWPORT_FAR = _( "Far" );
		PROPERTY_VIEWPORT_FOVY = _( "FOV Y" );
		PROPERTY_VIEWPORT_RATIO = _( "Ratio" );

		CreateTreeItemMenu();
	}

	ViewportTreeItemProperty::~ViewportTreeItemProperty()
	{
	}

	void ViewportTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		wxString l_selected;
		wxPGChoices l_choices;
		l_choices.Add( PROPERTY_VIEWPORT_TYPE_PERSPECTIVE );
		l_choices.Add( PROPERTY_VIEWPORT_TYPE_FRUSTUM );
		l_choices.Add( PROPERTY_VIEWPORT_TYPE_ORTHO );

		switch ( m_viewport.GetType() )
		{
		case ViewportType::Perspective:
			l_selected = PROPERTY_VIEWPORT_TYPE_PERSPECTIVE;
			break;

		case ViewportType::Frustum:
			l_selected = PROPERTY_VIEWPORT_TYPE_FRUSTUM;
			break;

		case ViewportType::Ortho:
			l_selected = PROPERTY_VIEWPORT_TYPE_ORTHO;
			break;
		}

		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VIEWPORT ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_VIEWPORT_TYPE, PROPERTY_VIEWPORT_TYPE, l_choices ) )->SetValue( l_selected );
		p_grid->Append( new SizeProperty( PROPERTY_VIEWPORT_SIZE ) )->SetValue( WXVARIANT( m_viewport.GetSize() ) );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_TOP ) )->SetValue( m_viewport.GetTop() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_BOTTOM ) )->SetValue( m_viewport.GetBottom() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_LEFT ) )->SetValue( m_viewport.GetLeft() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RIGHT ) )->SetValue( m_viewport.GetRight() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_NEAR ) )->SetValue( m_viewport.GetNear() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FAR ) )->SetValue( m_viewport.GetFar() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FOVY ) )->SetValue( m_viewport.GetFovY().degrees() );
		p_grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RATIO ) )->SetValue( m_viewport.GetRatio() );
	}

	void ViewportTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_VIEWPORT_TYPE )
			{
				if ( l_property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_PERSPECTIVE )
				{
					OnTypeChange( ViewportType::Perspective );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_ORTHO )
				{
					OnTypeChange( ViewportType::Ortho );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_FRUSTUM )
				{
					OnTypeChange( ViewportType::Frustum );
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

	void ViewportTreeItemProperty::OnTypeChange( ViewportType p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateType( p_value );
		} );
	}

	void ViewportTreeItemProperty::OnSizeChange( Castor::Size const & p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.Resize( p_value );
		} );
	}

	void ViewportTreeItemProperty::OnTopChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateTop( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnBottomChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateBottom( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnLeftChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateLeft( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnRightChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateRight( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnNearChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateNear( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnFarChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateFar( float( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnFovYChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateFovY( Angle::from_degrees( p_value ) );
		} );
	}

	void ViewportTreeItemProperty::OnRatioChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_viewport.UpdateRatio( float( p_value ) );
		} );
	}
}
