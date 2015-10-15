#include "AnimationManager.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimationManager::AnimationManager( Engine & p_engine )
		: Manager< String, Animation >( p_engine )
	{
	}

	AnimationManager::~AnimationManager()
	{
	}
}
