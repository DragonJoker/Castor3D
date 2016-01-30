#include "SceneManager.hpp"

using namespace Castor;

namespace Castor3D
{
	SceneManager::SceneManager( Engine & p_engine )
		: ResourceManager< String, Scene >( p_engine )
	{
	}

	SceneManager::~SceneManager()
	{
	}
}
