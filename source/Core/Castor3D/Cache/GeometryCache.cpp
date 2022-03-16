#include "Castor3D/Cache/GeometryCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementCUSmartPtr( castor3d, GeometryCache )

namespace castor3d
{
	const castor::String ObjectCacheTraitsT< Geometry, castor::String >::Name = cuT( "Geometry" );

	//*********************************************************************************************

	size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		size_t result = std::hash< castor::String >{}( geometry.getName() );
		castor::hashCombine( result, submesh.getOwner()->getName() );
		castor::hashCombine( result, submesh.getId() );
		castor::hashCombine( result, pass.getOwner()->getName() );
		castor::hashCombine( result, pass.getIndex() );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this, &scene]( ElementT & element )
			{
				auto mesh = element.getMesh().lock();

				if ( mesh )
				{
					auto & nodes = scene.getRenderNodes();

					for ( auto & submesh : *mesh )
					{
						auto material = element.getMaterial( *submesh );
						auto submeshFlags = submesh->getProgramFlags( material );
						auto animMesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
							? std::static_pointer_cast< AnimatedMesh >( findAnimatedObject( scene, element.getName() + cuT( "_Mesh" ) ) )
							: nullptr;
						auto animSkeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
							? std::static_pointer_cast< AnimatedSkeleton >( findAnimatedObject( scene, element.getName() + cuT( "_Skeleton" ) ) )
							: nullptr;

						for ( auto & pass : *material )
						{
							nodes.createNode( *pass
								, *submesh
								, element
								, animMesh.get()
								, animSkeleton.get() );
						}
					}
				}

				scene.getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [&element, this]( RenderDevice const & device
						, QueueData const & queueData )
					{
						element.prepare( m_faceCount, m_vertexCount );
					} ) );
			}
			, []( ElementT & element )
			{
			}
			, MovableMergerT< GeometryCache >{ scene.getName() }
			, MovableAttacherT< GeometryCache >{}
			, MovableDetacherT< GeometryCache >{} }
	{
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::fillInfo( RenderInfo & info )const
	{
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto element : *this )
		{
			if ( auto mesh = element.second->getMesh().lock() )
			{
				info.m_totalObjectsCount += mesh->getSubmeshCount();
				info.m_totalVertexCount += mesh->getVertexCount();
				info.m_totalFaceCount += mesh->getFaceCount();
			}
		}
	}

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::add( ElementPtrT element )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		ElementObjectCacheT::doAddNoLock( element->getName(), element );
	}

	//*********************************************************************************************
}
