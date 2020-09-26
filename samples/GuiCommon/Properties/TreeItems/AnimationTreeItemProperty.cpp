#include "GuiCommon/Properties/TreeItems/AnimationTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
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
		: TreeItemProperty( engine, editable, ePROPERTY_DATA_TYPE_LIGHT )
		, m_name( name )
		, m_group( group )
		, m_groupAnim( anim )
	{
		CreateTreeItemMenu();
	}

	AnimationTreeItemProperty::~AnimationTreeItemProperty()
	{
	}

	void AnimationTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_ANIMATION = _( "Animation: " );
		static wxString PROPERTY_ANIMATION_SPEED = _( "Speed" );
		static wxString PROPERTY_ANIMATION_LOOPED = _( "Looped" );
		static wxString PROPERTY_ANIMATION_STATE[] = { _( "Play" ), _( "Stop" ), _( "Pause" ) };

		auto group = getGroup();

		if ( group )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATION + make_wxString( m_name ) );
			addProperty( grid, PROPERTY_ANIMATION_SPEED, m_groupAnim.m_scale
				, [this]( wxVariant const & var )
				{
					m_groupAnim.m_scale = variantCast< float >( var );
					getGroup()->setAnimationScale( m_name, m_groupAnim.m_scale );
				} );
			addProperty( grid, PROPERTY_ANIMATION_LOOPED, m_groupAnim.m_looped
				, [this]( wxVariant const & var )
				{
					m_groupAnim.m_looped = variantCast< bool >( var );
					getGroup()->setAnimationLooped( m_name, m_groupAnim.m_looped );
				} );
			m_stateProperty = addProperty( grid, PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )], editor
				, static_cast< ButtonEventMethod >( [this]( wxVariant const & var )
					{
						auto group = getGroup();

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

						default:
							break;
						}

						m_stateProperty->SetLabel( PROPERTY_ANIMATION_STATE[uint32_t( m_groupAnim.m_state )] );
					} ) );
		}
	}
}
