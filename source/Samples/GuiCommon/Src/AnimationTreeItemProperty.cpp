#include "AnimationTreeItemProperty.hpp"

#include <Animation.hpp>
#include <Engine.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_LIGHT_SPEED = _( "Speed" );
		static wxString PROPERTY_LIGHT_STATE[] = { _( "Play" ), _( "Stop" ), _( "Pause" ) };
	}

	AnimationTreeItemProperty::AnimationTreeItemProperty( Engine * p_engine, bool p_editable, AnimatedObjectGroupSPtr p_group, Castor::String const & p_name )
		: TreeItemProperty( p_engine, p_editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_name( p_name )
	{
		PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		PROPERTY_LIGHT_SPEED = _( "Speed" );
		PROPERTY_LIGHT_STATE[Animation::eSTATE_PLAYING] = _( "Pause" );
		PROPERTY_LIGHT_STATE[Animation::eSTATE_STOPPED] = _( "Play" );
		PROPERTY_LIGHT_STATE[Animation::eSTATE_PAUSED] = _( "Stop" );

		CreateTreeItemMenu();
	}

	AnimationTreeItemProperty::~AnimationTreeItemProperty()
	{
	}

	void AnimationTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		auto l_group = GetGroup();

		if ( l_group )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATION + wxString( l_group->GetName() ) ) );
			//p_grid->Append( new wxColourProperty( PROPERTY_LIGHT_SPEED ) )->SetValue( wxVariant( l_group->GetScale() ) );

			//auto l_state = l_animation->GetState();
			auto l_state = Animation::eSTATE_STOPPED;
			p_grid->Append( CreateProperty( PROPERTY_LIGHT_STATE[l_state], PROPERTY_LIGHT_STATE[l_state], static_cast< ButtonEventMethod >( &AnimationTreeItemProperty::OnStateChange ), this, p_editor ) );
		}
	}

	void AnimationTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto l_group = GetGroup();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_group )
		{
			wxColour l_colour;

			if ( l_property->GetName() == PROPERTY_LIGHT_SPEED )
			{
				OnSpeedChange( l_property->GetValue().GetReal() );
			}
		}
	}

	void AnimationTreeItemProperty::OnSpeedChange( double p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			//GetAnimation()->SetScale( p_value );
		} );
	}

	bool AnimationTreeItemProperty::OnStateChange( wxPGProperty * p_property )
	{
		DoApplyChange( [p_property, this]()
		{
			auto l_group = GetGroup();
			ButtonData * l_data = reinterpret_cast< ButtonData * >( p_property->GetClientObject() );

			//switch ( l_animation->GetState() )
			//{
			//case Animation::eSTATE_PLAYING:
			//	l_animation->Pause();
			//	break;

			//case Animation::eSTATE_STOPPED:
			//	l_animation->Play();
			//	break;

			//case Animation::eSTATE_PAUSED:
			//	l_animation->Stop();
			//	break;
			//}

			//p_property->SetLabel( PROPERTY_LIGHT_STATE[l_animation->GetState()] );
		} );

		return false;
	}
}
