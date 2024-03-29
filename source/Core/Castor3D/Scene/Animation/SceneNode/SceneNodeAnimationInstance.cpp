#include "Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

CU_ImplementSmartPtr( castor3d, SceneNodeAnimationInstance )

namespace castor3d
{
	SceneNodeAnimationInstance::SceneNodeAnimationInstance( AnimatedSceneNode & object
		, SceneNodeAnimation & animation )
		: AnimationInstance{ object, animation }
		, m_animatedSceneNode{ object }
		, m_sceneNodeAnimation{ animation }
		, m_prev{ animation.isEmpty() ? animation.end() : animation.begin() }
		, m_curr{ animation.isEmpty() ? animation.end() : animation.begin() + 1 }
		, m_initialTranslate{ object.getSceneNode().getPosition() }
		, m_initialRotate{ object.getSceneNode().getOrientation() }
		, m_initialScale{ object.getSceneNode().getScale() }
	{
	}

	void SceneNodeAnimationInstance::doUpdate()
	{
		if ( !m_sceneNodeAnimation.isEmpty() )
		{
			if ( !m_vecInterpolator
				|| m_vecInterpolator->getType() != getInterpolation() )
			{
				m_vecInterpolator = makeInterpolator< castor::Point3f >( getInterpolation() );
				m_quatInterpolator = makeInterpolator< castor::Quaternion >( getInterpolation() );
			}

			m_sceneNodeAnimation.findKeyFrame( m_currentTime
				, m_prev
				, m_curr );
			auto & prev = static_cast< SceneNodeAnimationKeyFrame const & >( **m_prev );
			auto & curr = static_cast< SceneNodeAnimationKeyFrame const & >( **m_curr );
			auto ratio = float( ( m_currentTime - ( *m_prev )->getTimeIndex() ).count() ) / float( ( ( *m_curr )->getTimeIndex() - ( *m_prev )->getTimeIndex() ).count() );

			auto translate = m_vecInterpolator->interpolate( prev.getPosition(), curr.getPosition(), ratio );
			auto rotate = m_quatInterpolator->interpolate( prev.getRotation(), curr.getRotation(), ratio );
			auto scale = m_vecInterpolator->interpolate( prev.getScale(), curr.getScale(), ratio );

			m_animatedSceneNode.getSceneNode().setPosition( translate );
			m_animatedSceneNode.getSceneNode().setOrientation( rotate );
			m_animatedSceneNode.getSceneNode().setScale( scale );
		}
	}
}
