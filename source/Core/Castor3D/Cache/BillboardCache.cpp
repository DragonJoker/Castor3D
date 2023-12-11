#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, BillboardListCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< BillboardList, castor::String >::Name = cuT( "BillboardList" );

	//*********************************************************************************************

	size_t hash( BillboardBase const & billboard
		, Pass const & pass )
	{
		size_t result = std::hash< BillboardBase const * >{}( &billboard );
		castor::hashCombine( result, pass.getHash() );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::ObjectCacheT( Scene & scene
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
				auto material = element.getMaterial();

				for ( auto & pass : *material )
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
