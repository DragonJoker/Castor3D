#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementSmartPtr( c3d, SceneNodeAnimation )

namespace c3d
{
	SceneNodeAnimation::SceneNodeAnimation( SceneNode & node
		, String const & name )
		: Animation{ *node.getEngine()
			, AnimationType::eSceneNode
			, node
			, name }
	{
	}
}
