#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

CU_ImplementSmartPtr( c3d, SceneNodeAnimationKeyFrame )

namespace c3d
{
	SceneNodeAnimationKeyFrame::SceneNodeAnimationKeyFrame( SceneNodeAnimation & parent
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< SceneNodeAnimation >{ parent }
	{
	}

	AnimationKeyFrameUPtr SceneNodeAnimationKeyFrame::clone( Animation & parent )const
	{
		auto & skelAnim = static_cast< SceneNodeAnimation & >( parent );
		auto result = makeUnique< SceneNodeAnimationKeyFrame >( skelAnim, getTimeIndex() );
		result->m_position = m_position;
		result->m_rotation = m_rotation;
		result->m_scale = m_scale;
		doCloneInto( *result );
		return ptrRefCast< AnimationKeyFrame >( result );
	}
}
