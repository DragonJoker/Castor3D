#include "GuiCommon/Properties/TreeItems/AnimationTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	AnimationTreeItemProperty::AnimationTreeItemProperty( Engine * engine
		, bool editable
		, AnimatedObjectGroup & group
		, castor::String const & name
		, GroupAnimation const & anim )
		: TreeItemProperty( engine, editable )
		, m_group( group )
		, m_name( name )
		, m_groupAnim( anim )
	{
		CreateTreeItemMenu();
	}

	void AnimationTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_ANIMATION_STARTING_POINT = _( "Start at" );
		static wxString PROPERTY_ANIMATION_STOPPING_POINT = _( "Stop at" );
		static wxString PROPERTY_ANIMATION_SPEED = _( "Speed" );
		static wxString PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		static wxString PROPERTY_ANIMATION_STATE[] = { _( "Play" ), _( "Stop" ), _( "Pause" ) };

		auto group = getGroup();

		if ( group )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATION + make_wxString( m_name ) );
			addProperty( grid, PROPERTY_ANIMATION_SPEED, m_groupAnim.scale
				, [this]( wxVariant const & var )
				{
					m_groupAnim.scale = variantCast< float >( var );
					getGroup()->setAnimationScale( m_name, m_groupAnim.scale );
				} );
			addProperty( grid, PROPERTY_ANIMATION_STARTING_POINT, m_groupAnim.startingPoint
				, [this]( wxVariant const & var )
				{
					m_groupAnim.startingPoint = variantCast< castor::Milliseconds >( var );
					getGroup()->setAnimationStartingPoint( m_name, m_groupAnim.startingPoint );
				} );
			addProperty( grid, PROPERTY_ANIMATION_STOPPING_POINT, m_groupAnim.stoppingPoint
				, [this]( wxVariant const & var )
				{
					m_groupAnim.stoppingPoint = variantCast< castor::Milliseconds >( var );
					getGroup()->setAnimationStoppingPoint( m_name, m_groupAnim.stoppingPoint );
				} );
			addProperty( grid, PROPERTY_ANIMATION_LOOPED, m_groupAnim.looped
				, [this]( wxVariant const & var )
				{
					m_groupAnim.looped = variantCast< bool >( var );
					getGroup()->setAnimationLooped( m_name, m_groupAnim.looped );
				} );
			m_stateProperty = addProperty( grid, PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.state )], editor
				, static_cast< ButtonEventMethod >( [this]( wxVariant const & var )
					{
						auto group = getGroup();

						switch ( m_groupAnim.state )
						{
						case AnimationState::ePlaying:
							group->pauseAnimation( m_name );
							m_groupAnim.state = AnimationState::ePaused;
							break;

						case AnimationState::eStopped:
							group->startAnimation( m_name );
							m_groupAnim.state = AnimationState::ePlaying;
							break;

						case AnimationState::ePaused:
							group->stopAnimation( m_name );
							m_groupAnim.state = AnimationState::eStopped;
							break;

						default:
							break;
						}

						m_stateProperty->SetLabel( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.state )] );
					} ) );
		}
	}
}
