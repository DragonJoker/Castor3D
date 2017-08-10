#include "AnimationTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Animation/Animation.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_ANIMATION_SPEED = _( "Speed" );
		static wxString PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		static wxString PROPERTY_ANIMATION_STATE[] = { _( "Play" ), _( "Stop" ), _( "Pause" ) };
	}

	AnimationTreeItemProperty::AnimationTreeItemProperty( Engine * engine, bool p_editable, AnimatedObjectGroupSPtr p_group, castor::String const & p_name, GroupAnimation const & p_anim )
		: TreeItemProperty( engine, p_editable, ePROPERTY_DATA_TYPE_LIGHT )
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

	void AnimationTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		auto group = getGroup();

		if ( group )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATION + make_wxString( m_name ) ) );
			p_grid->Append( CreateProperty< wxFloatProperty >( PROPERTY_ANIMATION_SPEED, wxVariant( m_groupAnim.m_scale ), _( "The animation time scale." ) ) );
			p_grid->Append( CreateProperty< wxBoolProperty >( PROPERTY_ANIMATION_LOOPED, wxVariant( m_groupAnim.m_looped ), _( "sets the animation looped or not" ) ) );
			p_grid->Append( CreateProperty( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )], PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )], static_cast< ButtonEventMethod >( &AnimationTreeItemProperty::OnStateChange ), this, p_editor ) );
		}
	}

	void AnimationTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto group = getGroup();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && group )
		{
			wxColour colour;

			if ( property->GetName() == PROPERTY_ANIMATION_SPEED )
			{
				OnSpeedChange( property->GetValue().GetReal() );
			}
			else if ( property->GetName() == PROPERTY_ANIMATION_LOOPED )
			{
				OnLoopedChange( property->GetValue().GetBool() );
			}
		}
	}

	void AnimationTreeItemProperty::OnSpeedChange( double p_value )
	{
		m_groupAnim.m_scale = p_value;

		doApplyChange( [this]()
		{
			getGroup()->setAnimationscale( m_name, m_groupAnim.m_scale );
		} );
	}

	void AnimationTreeItemProperty::OnLoopedChange( bool p_value )
	{
		m_groupAnim.m_looped = p_value;

		doApplyChange( [this]()
		{
			getGroup()->setAnimationLooped( m_name, m_groupAnim.m_looped );
		} );
	}

	bool AnimationTreeItemProperty::OnStateChange( wxPGProperty * p_property )
	{
		doApplyChange( [p_property, this]()
		{
			auto group = getGroup();
			ButtonData * data = reinterpret_cast< ButtonData * >( p_property->GetClientObject() );

			switch ( m_groupAnim.m_state )
			{
			case AnimationState::ePlaying:
				group->pauseAnimation( m_name );
				m_groupAnim.m_state = AnimationState::ePaused;
				break;

			case AnimationState::eStopped:
				group->startAnimation( m_name );
				m_groupAnim.m_state = AnimationState::ePlaying;
				break;

			case AnimationState::ePaused:
				group->stopAnimation( m_name );
				m_groupAnim.m_state = AnimationState::eStopped;
				break;
			}

			p_property->SetLabel( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )] );
		} );

		return false;
	}
}
