#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	AnimatedSceneNode::AnimatedSceneNode( castor::String const & name
		, SceneNode & node )
		: AnimatedObject{ AnimationType::eSceneNode, name }
		, m_node{ node }
	{
	}

	void AnimatedSceneNode::update( castor::Milliseconds const & elpased )
	{
		if ( m_playingAnimation )
		{
			m_playingAnimation->update( elpased );
		}
	}

	void AnimatedSceneNode::doAddAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			if ( m_node.hasAnimation( name ) )
			{
				auto & animation = static_cast< SceneNodeAnimation & >( m_node.getAnimation( name ) );
				auto instance = std::make_unique< SceneNodeAnimationInstance >( *this, animation );
				m_animations.emplace( name, std::move( instance ) );
			}
		}
	}

	void AnimatedSceneNode::doStartAnimation( AnimationInstance & animation )
	{
		CU_Require( m_playingAnimation == nullptr );
		m_playingAnimation = &static_cast< SceneNodeAnimationInstance & >( animation );
	}

	void AnimatedSceneNode::doStopAnimation( AnimationInstance & animation )
	{
		CU_Require( m_playingAnimation == &animation );
		m_playingAnimation = nullptr;
	}

	void AnimatedSceneNode::doClearAnimations()
	{
		m_playingAnimation = nullptr;
	}
}
