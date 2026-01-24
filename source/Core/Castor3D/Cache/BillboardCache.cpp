#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( c3d, BillboardListCache )

namespace c3d
{
	const String ObjectCacheTraitsT< BillboardList, String >::Name = cuT( "BillboardList" );

	//*********************************************************************************************

	size_t hash( BillboardBase const & billboard
		, Pass const & pass )
	{
		size_t result = std::hash< BillboardBase const * >{}( &billboard );
		hashCombine( result, pass.getHash() );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< BillboardList, String, BillboardCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeRPtr rootNode
		, SceneNodeRPtr rootCameraNode
		, SceneNodeRPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [&scene]( ElementT & element )
			{
				auto & nodes = scene.getRenderNodes();

				for ( auto const & pass : *element.getMaterial() )
				{
					nodes.createNode( *pass
						, element );
				}

				scene.getListener().postEvent( makeGpuInitialiseEvent( element ) );
			}
			, [&scene]( ElementT & element )
			{
				scene.getListener().postEvent( makeGpuCleanupEvent( element ) );
			}
			, MovableMergerT< BillboardListCache >{ scene.getName() }
			, MovableAttacherT< BillboardListCache >{}
			, MovableDetacherT< BillboardListCache >{} }
	{
	}

	//*********************************************************************************************
}
