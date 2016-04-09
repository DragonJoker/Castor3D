#include "BillboardManager.hpp"

using namespace Castor;

namespace Castor3D
{
	BillboardManager::BillboardManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		: ObjectManager< Castor::String, BillboardList >( p_owner, p_rootNode, p_rootCameraNode, p_rootObjectNode )
	{
	}

	BillboardManager::~BillboardManager()
	{
	}

	void BillboardManager::Cleanup()
	{
		ObjectManager< Castor::String, BillboardList >::Cleanup();
		auto l_lock = make_unique_lock( *this );

		for ( auto l_billboard : *this )
		{
			GetEngine()->PostEvent( MakeCleanupEvent( *l_billboard.second ) );
		}
	}
}
