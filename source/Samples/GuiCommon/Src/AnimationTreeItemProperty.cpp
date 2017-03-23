#include "AnimationTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Animation/Animation.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_ANIMATION_SPEED = _( "Speed" );
		static wxString PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		static wxString PROPERTY_ANIMATION_STATE[] = { _( "Play" ), _( "Stop" ), _( "Pause" ) };
	}

	AnimationTreeItemProperty::AnimationTreeItemProperty( Engine * p_engine, bool p_editable, AnimatedObjectGroupSPtr p_group, Castor::String const & p_name, GroupAnimation const & p_anim )
		: TreeItemProperty( p_engine, p_editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_name( p_name )
		, m_group( p_group )
		, m_groupAnim( p_anim )
	{
		PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		PROPERTY_ANIMATION_SPEED = _( "Speed" );
		PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		PROPERTY_ANIMATION_STATE[uint32_t( AnimationState::ePlaying )] = _( "Pause" );
		PROPERTY_ANIMATION_STATE[uint32_t( AnimationState::eStopped )] = _( "Play" );
		PROPERTY_ANIMATION_STATE[uint32_t( AnimationState::ePaused )] = _( "Stop" );

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
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATION + make_wxString( m_name ) ) );
			p_grid->Append( CreateProperty< wxFloatProperty >( PROPERTY_ANIMATION_SPEED, wxVariant( m_groupAnim.m_scale ), _( "The animation time scale." ) ) );
			p_grid->Append( CreateProperty< wxBoolProperty >( PROPERTY_ANIMATION_LOOPED, wxVariant( m_groupAnim.m_looped ), _( "Sets the animation looped or not" ) ) );
			p_grid->Append( CreateProperty( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )], PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )], static_cast< ButtonEventMethod >( &AnimationTreeItemProperty::OnStateChange ), this, p_editor ) );
		}
	}

	void AnimationTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto l_group = GetGroup();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_group )
		{
			wxColour l_colour;

			if ( l_property->GetName() == PROPERTY_ANIMATION_SPEED )
			{
				OnSpeedChange( l_property->GetValue().GetReal() );
			}
			else if ( l_property->GetName() == PROPERTY_ANIMATION_LOOPED )
			{
				OnLoopedChange( l_property->GetValue().GetBool() );
			}
		}
	}

	void AnimationTreeItemProperty::OnSpeedChange( double p_value )
	{
		m_groupAnim.m_scale = p_value;

		DoApplyChange( [this]()
		{
			GetGroup()->SetAnimationScale( m_name, m_groupAnim.m_scale );
		} );
	}

	void AnimationTreeItemProperty::OnLoopedChange( bool p_value )
	{
		m_groupAnim.m_looped = p_value;

		DoApplyChange( [this]()
		{
			GetGroup()->SetAnimationLooped( m_name, m_groupAnim.m_looped );
		} );
	}

	bool AnimationTreeItemProperty::OnStateChange( wxPGProperty * p_property )
	{
		DoApplyChange( [p_property, this]()
		{
			auto l_group = GetGroup();
			ButtonData * l_data = reinterpret_cast< ButtonData * >( p_property->GetClientObject() );

			switch ( m_groupAnim.m_state )
			{
			case AnimationState::ePlaying:
				l_group->PauseAnimation( m_name );
				m_groupAnim.m_state = AnimationState::ePaused;
				break;

			case AnimationState::eStopped:
				l_group->StartAnimation( m_name );
				m_groupAnim.m_state = AnimationState::ePlaying;
				break;

			case AnimationState::ePaused:
				l_group->StopAnimation( m_name );
				m_groupAnim.m_state = AnimationState::eStopped;
				break;
			}

			p_property->SetLabel( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )] );
		} );

		return false;
	}
}
