#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

CU_ImplementCUSmartPtr( castor3d, SceneNodeAnimationKeyFrame )

namespace castor3d
{
	SceneNodeAnimationKeyFrame::SceneNodeAnimationKeyFrame( SceneNodeAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< SceneNodeAnimation >{ parent }
	{
	}
}
