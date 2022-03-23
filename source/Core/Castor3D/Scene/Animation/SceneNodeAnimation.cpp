#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	SceneNodeAnimation::SceneNodeAnimation( SceneNode & node
		, castor::String const & name )
		: Animation{ *node.getEngine()
			, AnimationType::eSceneNode
			, node
			, name }
	{
	}
}
