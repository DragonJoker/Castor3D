#include "Castor3D/Cache/SceneNodeCache.hpp"

#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, SceneNodeCache )

namespace c3d
{
	const String ObjectCacheTraitsT< SceneNode, String >::Name = cuT( "SceneNode" );

	//*********************************************************************************************

	ObjectCacheT< SceneNode, String, SceneNodeCacheTraits >::ObjectCacheT( Scene & scene
		, ElementInitialiserT initialise
		, ElementCleanerT clean
		, ElementMergerT merge
		, ElementAttacherT attach
		, ElementDetacherT detach )
		: ElementObjectCacheT{ scene
			, nullptr
			, nullptr
			, nullptr
			, c3d::move( initialise )
			, c3d::move( clean )
			, c3d::move( merge )
			, c3d::move( attach )
			, c3d::move( detach ) }
	{
		auto node = makeUnique< SceneNode >( Scene::RootNode, scene );
		m_rootNode = add( Scene::RootNode, node, false );
		node = makeUnique< SceneNode >( Scene::ObjectRootNode, scene );
		m_rootObjectNode = add( Scene::ObjectRootNode, node, false );
		node = makeUnique< SceneNode >( Scene::CameraRootNode, scene );
		m_rootCameraNode = add( Scene::CameraRootNode, node, false );

		m_rootCameraNode->attachTo( *m_rootNode );
		m_rootObjectNode->attachTo( *m_rootNode );
	}

	SceneNodeCache::ElementObsT ObjectCacheT< SceneNode, String, SceneNodeCacheTraits >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		auto lock( makeUniqueLock( *this ) );
		auto result = this->doAddNoLock( name
			, element
			, initialise );
		return result;
	}

	SceneNodeCache::ElementObsT ObjectCacheT< SceneNode, String, SceneNodeCacheTraits >::addNew( ElementKeyT const & name )
	{
		auto lock( makeUniqueLock( *this ) );
		auto result = this->doAddNewNoLockT( name
			, *getScene() );
		return result;
	}

	//*********************************************************************************************
}
