#include "SceneManager.hpp"

#include "SceneNodeManager.hpp"
#include "GeometryManager.hpp"
#include "BillboardManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< Scene >::Name = cuT( "Scene" );
	const String ManagedObjectNamer< SceneNode >::Name = cuT( "SceneNode" );
	const String ManagedObjectNamer< Geometry >::Name = cuT( "Geometry" );
	const String ManagedObjectNamer< BillboardList >::Name = cuT( "BillboardList" );

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
