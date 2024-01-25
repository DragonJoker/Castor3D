#include "Castor3D/Cache/SceneNodeCache.hpp"

#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, SceneNodeCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< SceneNode, castor::String >::Name = cuT( "SceneNode" );

	//*********************************************************************************************

	ObjectCacheT< SceneNode, castor::String, SceneNodeCacheTraits >::ObjectCacheT( Scene & scene
		, ElementInitialiserT initialise
		, ElementCleanerT clean
		, ElementMergerT merge
		, ElementAttacherT attach
		, ElementDetacherT detach )
		: ElementObjectCacheT{ scene
			, nullptr
			, nullptr
			, nullptr
			, castor::move( initialise )
			, castor::move( clean )
			, castor::move( merge )
			, castor::move( attach )
			, castor::move( detach ) }
	{
		auto node = castor::makeUnique< SceneNode >( Scene::RootNode, scene );
		m_rootNode = add( Scene::RootNode, node, false );
		node = castor::makeUnique< SceneNode >( Scene::ObjectRootNode, scene );
		m_rootObjectNode = add( Scene::ObjectRootNode, node, false );
		node = castor::makeUnique< SceneNode >( Scene::CameraRootNode, scene );
		m_rootCameraNode = add( Scene::CameraRootNode, node, false );

		m_rootCameraNode->attachTo( *m_rootNode );
		m_rootObjectNode->attachTo( *m_rootNode );
	}

	SceneNodeCache::ElementObsT ObjectCacheT< SceneNode, castor::String, SceneNodeCacheTraits >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto result = this->doAddNoLock( name
			, element
			, initialise );
		return result;
	}

	SceneNodeCache::ElementObsT ObjectCacheT< SceneNode, castor::String, SceneNodeCacheTraits >::add( ElementKeyT const & name )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto result = this->doAddNoLockT( name
			, *getScene() );
		return result;
	}

	//*********************************************************************************************
}
