#include "AnimatedObjectGroupManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< AnimatedObjectGroup >::Name = cuT( "AnimatedObjectGroup" );

	AnimatedObjectGroupManager::AnimatedObjectGroupManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		: ObjectManager< Castor::String, AnimatedObjectGroup >( p_owner, p_rootNode, p_rootCameraNode, p_rootObjectNode )
	{
	}

	AnimatedObjectGroupManager::~AnimatedObjectGroupManager()
	{
	}

	void AnimatedObjectGroupManager::Update()
	{
		auto l_lock = make_unique_lock( m_elements );

		for ( auto l_pair : m_elements )
		{
			l_pair.second->Update();
		}
	}
}
