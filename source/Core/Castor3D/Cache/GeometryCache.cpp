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
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
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

CU_ImplementSmartPtr( castor3d, GeometryCache )

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
		castor::hashCombine( result, pass.getHash() );
		return result;
	}

	//*********************************************************************************************

	ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::ObjectCacheT( Scene & scene
		, SceneNodeRPtr rootNode
		, SceneNodeRPtr rootCameraNode
		, SceneNodeRPtr rootObjectNode )
		: ElementObjectCacheT{ scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, [this, &scene]( ElementT & element )
			{
				auto mesh = element.getMesh();

				if ( mesh )
				{
					auto & nodes = scene.getRenderNodes();

					for ( auto & submesh : *mesh )
					{
						auto material = element.getMaterial( *submesh );
						auto animMesh = submesh->hasMorphComponent()
							? static_cast< AnimatedMesh * >( findAnimatedObject( scene, element.getName() + cuT( "_Mesh" ) ) )
							: nullptr;
						auto animSkeleton = submesh->hasSkinComponent()
							? static_cast< AnimatedSkeleton * >( findAnimatedObject( scene, element.getName() + cuT( "_Skeleton" ) ) )
							: nullptr;

						if ( submesh->hasComponent( PassMasksComponent::TypeName ) )
						{
							if ( auto pass = material->getPass( 0u ) )
							{
								nodes.createNode( *pass
									, *submesh
									, element
									, animMesh
									, animSkeleton );
							}
						}
						else
						{
							for ( auto & pass : *material )
							{
								nodes.createNode( *pass
									, *submesh
									, element
									, animMesh
									, animSkeleton );
							}
						}
					}
				}

				scene.getListener().postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
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

	void ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >::add( ElementPtrT element )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		ElementObjectCacheT::doAddNoLock( element->getName(), element );
	}

	//*********************************************************************************************
}
