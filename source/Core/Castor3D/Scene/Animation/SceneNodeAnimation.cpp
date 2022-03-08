#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include "Castor3D/Scene/SceneNode.hpp"

using namespace castor;

namespace castor3d
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
