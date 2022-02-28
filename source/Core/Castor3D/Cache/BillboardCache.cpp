#include "Castor3D/Cache/BillboardCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
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

CU_ImplementCUSmartPtr( castor3d, BillboardListCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< BillboardList, castor::String >::Name = cuT( "BillboardList" );

	//*********************************************************************************************

	size_t hash( BillboardBase const & billboard
		, Pass const & pass )
	{
		size_t result = std::hash< BillboardBase const * >{}( &billboard );
		castor::hashCombinePtr( result, pass );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this]( ElementT & element )
			{
				auto scene = getScene();
				auto & nodes = scene->getRenderNodes();
				auto material = element.getMaterial();

				for ( auto & pass : *material )
				{
					nodes.createNode( *pass
						, element );
				}

				scene->getListener().postEvent( makeGpuInitialiseEvent( element ) );
			}
			, [this]( ElementT & element )
			{
				getScene()->getListener().postEvent( makeGpuCleanupEvent( element ) );
			}
			, MovableMergerT< BillboardListCache >{ scene.getName() }
			, MovableAttacherT< BillboardListCache >{}
			, MovableDetacherT< BillboardListCache >{} }
	{
	}

	//*********************************************************************************************
}
