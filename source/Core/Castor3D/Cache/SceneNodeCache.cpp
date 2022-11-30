#include "Castor3D/Cache/SceneNodeCache.hpp"

#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementCUSmartPtr( castor3d, SceneNodeCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< SceneNode, castor::String >::Name = cuT( "SceneNode" );

	//*********************************************************************************************

	ObjectCacheT< SceneNode, castor::String, SceneNodeCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeRPtr rootNode
		, SceneNodeRPtr rootCameraNode
		, SceneNodeRPtr rootObjectNode
		, ElementInitialiserT initialise
		, ElementCleanerT clean
		, ElementMergerT merge
		, ElementAttacherT attach
		, ElementDetacherT detach )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootObjectNode
			, initialise
			, clean
			, merge
			, attach
			, detach }
	{
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
