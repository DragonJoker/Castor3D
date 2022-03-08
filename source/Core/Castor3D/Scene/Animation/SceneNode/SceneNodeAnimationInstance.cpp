#include "Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	SceneNodeAnimationInstance::SceneNodeAnimationInstance( AnimatedSceneNode & object
		, SceneNodeAnimation & animation )
		: AnimationInstance{ object, animation }
		, m_animatedSceneNode{ object }
		, m_sceneNodeAnimation{ animation }
		, m_initialTranslate{ object.getSceneNode().getPosition() }
		, m_initialRotate{ object.getSceneNode().getOrientation() }
		, m_initialScale{ object.getSceneNode().getScale() }
	{
	}

	void SceneNodeAnimationInstance::doUpdate()
	{
		if ( m_first )
		{
			m_prev = m_sceneNodeAnimation.isEmpty() ? m_sceneNodeAnimation.end() : m_sceneNodeAnimation.begin();
			m_curr = m_sceneNodeAnimation.isEmpty() ? m_sceneNodeAnimation.end() : m_sceneNodeAnimation.begin() + 1;
			m_first = false;
		}

		m_sceneNodeAnimation.findKeyFrame( m_currentTime
			, m_prev
			, m_curr );
		auto & prev = static_cast< SceneNodeAnimationKeyFrame const & >( **m_prev );
		auto & curr = static_cast< SceneNodeAnimationKeyFrame const & >( **m_curr );
		auto ratio = float( ( m_currentTime - ( *m_prev )->getTimeIndex() ).count() ) / float( ( ( *m_curr )->getTimeIndex() - ( *m_prev )->getTimeIndex() ).count() );

		auto translate = m_vecInterpolator.interpolate( prev.getPosition(), curr.getPosition(), ratio );
		auto rotate = m_quatInterpolator.interpolate( prev.getRotation(), curr.getRotation(), ratio );
		auto scale = m_vecInterpolator.interpolate( prev.getScale(), curr.getScale(), ratio );

		m_animatedSceneNode.getSceneNode().setPosition( translate );
		m_animatedSceneNode.getSceneNode().setOrientation( rotate );
		m_animatedSceneNode.getSceneNode().setScale( scale );
	}
}
