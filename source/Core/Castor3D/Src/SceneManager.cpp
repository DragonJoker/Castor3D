#include "SceneManager.hpp"

using namespace Castor;

namespace Castor3D
{
	SceneManager::SceneManager( Engine & p_engine )
		: Manager< String, Scene, Engine >( p_engine )
	{
	}

	SceneManager::~SceneManager()
	{
	}
}
