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

	void SceneManager::Update()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_it : *this )
		{
			l_it.second->Update();
		}
	}
}
