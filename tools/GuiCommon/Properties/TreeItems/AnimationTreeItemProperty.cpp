#include "GuiCommon/Properties/TreeItems/AnimationTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	AnimationTreeItemProperty::AnimationTreeItemProperty( bool editable
		, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void AnimationTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_ANIMATION_STARTING_POINT = _( "Start at" );
		static wxString PROPERTY_ANIMATION_STOPPING_POINT = _( "Stop at" );
		static wxString PROPERTY_ANIMATION_SPEED = _( "Speed" );
		static wxString PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		static wxString PROPERTY_ANIMATION_STATE = { _( "State" ) };

		if ( !m_group )
		{
			return;
		}

		wxArrayString choices;
		choices.Add( _( "Play" ) );
		choices.Add( _( "Stop" ) );
		choices.Add( _( "Pause" ) );

		addProperty( grid, PROPERTY_CATEGORY_ANIMATION + make_wxString( m_groupAnim.name ) );
		addProperty( grid, PROPERTY_ANIMATION_SPEED, m_groupAnim.scale
			, [this]( wxVariant const & var )
			{
				m_groupAnim.scale = variantCast< float >( var );
				m_group->setAnimationScale( m_groupAnim.name, m_groupAnim.scale );
			} );
		addProperty( grid, PROPERTY_ANIMATION_STARTING_POINT, m_groupAnim.startingPoint
			, [this]( wxVariant const & var )
			{
				m_groupAnim.startingPoint = variantCast< castor::Milliseconds >( var );
				m_group->setAnimationStartingPoint( m_groupAnim.name, m_groupAnim.startingPoint );
			} );
		addProperty( grid, PROPERTY_ANIMATION_STOPPING_POINT, m_groupAnim.stoppingPoint
			, [this]( wxVariant const & var )
			{
				m_groupAnim.stoppingPoint = variantCast< castor::Milliseconds >( var );
				m_group->setAnimationStoppingPoint( m_groupAnim.name, m_groupAnim.stoppingPoint );
			} );
		addProperty( grid, PROPERTY_ANIMATION_LOOPED, m_groupAnim.looped
			, [this]( wxVariant const & var )
			{
				m_groupAnim.looped = variantCast< bool >( var );
				m_group->setAnimationLooped( m_groupAnim.name, m_groupAnim.looped );
			} );
		addProperty( grid, PROPERTY_ANIMATION_STATE, choices, choices[size_t( m_groupAnim.state )]
			, [this]( wxVariant const & var )
			{
				auto state = castor3d::AnimationState( var.GetLong() );

				switch ( state )
				{
				case castor3d::AnimationState::ePlaying:
					m_group->startAnimation( m_groupAnim.name );
					m_groupAnim.state = castor3d::AnimationState::ePlaying;
					break;
				case castor3d::AnimationState::eStopped:
					m_group->stopAnimation( m_groupAnim.name );
					m_groupAnim.state = castor3d::AnimationState::eStopped;
					break;
				case castor3d::AnimationState::ePaused:
					m_group->pauseAnimation( m_groupAnim.name );
					m_groupAnim.state = castor3d::AnimationState::ePaused;
					break;
				default:
					break;
				}
			} );
	}
}
