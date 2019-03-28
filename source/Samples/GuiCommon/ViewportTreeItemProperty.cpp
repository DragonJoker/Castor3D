#include "GuiCommon/ViewportTreeItemProperty.hpp"

#include "GuiCommon/SizeProperties.hpp"

#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

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

	ViewportTreeItemProperty::ViewportTreeItemProperty( bool editable
		, castor3d::Engine & engine
		, castor3d::Viewport & viewport )
		: TreeItemProperty( &engine, editable, ePROPERTY_DATA_TYPE_VIEWPORT )
		, m_viewport( viewport )
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

	void ViewportTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		wxString selected;
		wxPGChoices choices;
		choices.Add( PROPERTY_VIEWPORT_TYPE_PERSPECTIVE );
		choices.Add( PROPERTY_VIEWPORT_TYPE_FRUSTUM );
		choices.Add( PROPERTY_VIEWPORT_TYPE_ORTHO );

		switch ( m_viewport.getType() )
		{
		case ViewportType::ePerspective:
			selected = PROPERTY_VIEWPORT_TYPE_PERSPECTIVE;
			break;

		case ViewportType::eFrustum:
			selected = PROPERTY_VIEWPORT_TYPE_FRUSTUM;
			break;

		case ViewportType::eOrtho:
			selected = PROPERTY_VIEWPORT_TYPE_ORTHO;
			break;
		}

		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VIEWPORT ) );
		grid->Append( new wxEnumProperty( PROPERTY_VIEWPORT_TYPE, PROPERTY_VIEWPORT_TYPE, choices ) )->SetValue( selected );
		grid->Append( new SizeProperty( PROPERTY_VIEWPORT_SIZE ) )->SetValue( WXVARIANT( m_viewport.getSize() ) );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_TOP ) )->SetValue( m_viewport.getTop() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_BOTTOM ) )->SetValue( m_viewport.getBottom() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_LEFT ) )->SetValue( m_viewport.getLeft() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RIGHT ) )->SetValue( m_viewport.getRight() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_NEAR ) )->SetValue( m_viewport.getNear() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FAR ) )->SetValue( m_viewport.getFar() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_FOVY ) )->SetValue( m_viewport.getFovY().degrees() );
		grid->Append( new wxFloatProperty( PROPERTY_VIEWPORT_RATIO ) )->SetValue( m_viewport.getRatio() );
	}

	void ViewportTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		wxPGProperty * property = event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_VIEWPORT_TYPE )
			{
				if ( property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_PERSPECTIVE )
				{
					OnTypeChange( ViewportType::ePerspective );
				}
				else if ( property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_ORTHO )
				{
					OnTypeChange( ViewportType::eOrtho );
				}
				else if ( property->GetValueAsString() == PROPERTY_VIEWPORT_TYPE_FRUSTUM )
				{
					OnTypeChange( ViewportType::eFrustum );
				}
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_SIZE )
			{
				OnSizeChange( SizeRefFromVariant( property->GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_TOP )
			{
				OnTopChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_BOTTOM )
			{
				OnBottomChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_LEFT )
			{
				OnLeftChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_RIGHT )
			{
				OnRightChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_NEAR )
			{
				OnNearChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_FAR )
			{
				OnFarChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_FOVY )
			{
				OnFovYChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_VIEWPORT_RATIO )
			{
				OnRatioChange( property->GetValue() );
			}
		}
	}

	void ViewportTreeItemProperty::OnTypeChange( ViewportType value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateType( value );
		} );
	}

	void ViewportTreeItemProperty::OnSizeChange( castor::Size const & value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.resize( value );
		} );
	}

	void ViewportTreeItemProperty::OnTopChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateTop( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnBottomChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateBottom( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnLeftChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateLeft( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnRightChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateRight( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnNearChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateNear( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnFarChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateFar( float( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnFovYChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateFovY( Angle::fromDegrees( value ) );
		} );
	}

	void ViewportTreeItemProperty::OnRatioChange( double value )
	{
		doApplyChange( [value, this]()
		{
			m_viewport.updateRatio( float( value ) );
		} );
	}
}
