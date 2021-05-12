#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

CU_ImplementCUSmartPtr( castor3d, QueueRenderNodes )

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	namespace
	{
		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT * node
			, NodeCulledT< NodeT > const & culled
			, NodeByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, NodeMapT< NodeT >{} ).first->second;
			pipelineMap.emplace( &culled, node );
		}

		template< typename NodeT >
		void doAddInstantiatedRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeT * node
			, Submesh & object
			, NodeCulledT< NodeT > const & culled
			, ObjectNodesByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, ObjectNodesByPassMapT< NodeT >{} ).first->second;
			auto & passMap = pipelineMap.emplace( &pass, ObjectNodesMapT< NodeT >{} ).first->second;
			auto & objectMap = passMap.emplace( &object, NodeMapT< NodeT >{} ).first->second;
			objectMap.emplace( &culled, node );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene const & scene
			, castor::String const & name )
		{
			AnimatedObjectSPtr result;
			auto & cache = scene.getAnimatedObjectGroupCache();
			using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };

			for ( auto group : cache )
			{
				if ( !result )
				{
					auto it = group.second->getObjects().find( name );

					if ( it != group.second->getObjects().end() )
					{
						result = it->second;
					}
				}
			}

			return result;
		}

		template< typename ContT
			, typename CulledT
			, typename CreatorT >
		void doAddNode( SceneRenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, ContT & nodes
			, VkPrimitiveTopology topology
			, CulledT const & culled
			, CreatorT creator )
		{
			if ( pass.isTwoSided()
				|| renderPass.forceTwoSided()
				|| pass.hasAlphaBlending() )
			{
				auto pipeline = renderPass.getPipelineFront( flags );

				if ( pipeline )
				{
					doAddRenderNode( *pipeline
						, creator( *pipeline )
						, culled
						, nodes.frontCulled );
				}
			}

			auto pipeline = renderPass.getPipelineBack( flags );

			if ( pipeline )
			{
				doAddRenderNode( *pipeline
					, creator( *pipeline )
					, culled
					, nodes.backCulled );
			}
		}

		void doAddSkinningNode( SceneRenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton
			, CulledSubmesh const & culled
			, QueueRenderNodes::SkinnedNodesMap & animated
			, QueueRenderNodes::InstantiatedSkinnedNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.isTwoSided()
					|| renderPass.forceTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						doAddInstantiatedRenderNode( pass
							, *pipeline
							, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
							, submesh
							, culled
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( flags );

				if ( pipeline )
				{
					doAddInstantiatedRenderNode( pass
						, *pipeline
						, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
						, submesh
						, culled
						, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, flags
					, pass
					, animated
					, submesh.getTopology()
					, culled
					, [&renderPass, &pass, &submesh, &primitive, &skeleton]( RenderPipeline & pipeline )
					{
						return renderPass.createSkinningNode( pass
							, pipeline
							, submesh
							, primitive
							, skeleton );
					} );
			}
		}

		void doAddMorphingNode( SceneRenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh
			, CulledSubmesh const & culled
			, QueueRenderNodes::MorphingNodesMap & animated )
		{
			doAddNode( renderPass
				, flags
				, pass
				, animated
				, submesh.getTopology()
				, culled
				, [&renderPass, &pass, &submesh, &primitive, &mesh]( RenderPipeline & pipeline )
				{
					return renderPass.createMorphingNode( pass
						, pipeline
						, submesh
						, primitive
						, mesh );
				} );
		}

		void doAddStaticNode( SceneRenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, CulledSubmesh const & culled
			, QueueRenderNodes::StaticNodesMap & statics
			, QueueRenderNodes::InstantiatedStaticNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.isTwoSided()
					|| renderPass.forceTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						doAddInstantiatedRenderNode( pass
							, *pipeline
							, renderPass.createStaticNode( pass, *pipeline, submesh, primitive )
							, submesh
							, culled
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( flags );

				if ( pipeline )
				{
					doAddInstantiatedRenderNode( pass
						, *pipeline
						, renderPass.createStaticNode( pass, *pipeline, submesh, primitive )
						, submesh
						, culled
						, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, flags
					, pass
					, statics
					, submesh.getTopology()
					, culled
					, [&renderPass, &pass, &submesh, &primitive]( RenderPipeline & pipeline )
					{
						return renderPass.createStaticNode( pass
							, pipeline
							, submesh
							, primitive );
					} );
			}
		}

		QueueRenderNodes::AnimatedObjects doAdjustFlags( RenderSystem const & renderSystem
			, ProgramFlags & programFlags
			, SceneFlags const & sceneFlags
			, Scene const & scene
			, Pass const & pass
			, SceneRenderPass const & renderPass
			, castor::String const & name )
		{
			auto submeshFlags = programFlags;
			remFlag( programFlags, ProgramFlag::eSkinning );
			remFlag( programFlags, ProgramFlag::eMorphing );
			auto mesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
				? std::static_pointer_cast< AnimatedMesh >( doFindAnimatedObject( scene, name + cuT( "_Mesh" ) ) )
				: nullptr;
			auto skeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
				? std::static_pointer_cast< AnimatedSkeleton >( doFindAnimatedObject( scene, name + cuT( "_Skeleton" ) ) )
				: nullptr;

			if ( skeleton  )
			{
				addFlag( programFlags, ProgramFlag::eSkinning );
			}

			if ( mesh )
			{
				addFlag( programFlags, ProgramFlag::eMorphing );
			}

			if ( checkFlag( submeshFlags, ProgramFlag::eInstantiation )
				&& ( !pass.hasOnlyAlphaBlending() || renderPass.isOrderIndependent() )
				&& !pass.hasEnvironmentMapping() )
			{
				if ( !checkFlag( programFlags, ProgramFlag::eSkinning )
					|| renderSystem.getGpuInformations().hasShaderStorageBuffers() )
				{
					addFlag( programFlags, ProgramFlag::eInstantiation );
				}
				else
				{
					remFlag( programFlags, ProgramFlag::eInstantiation );
				}
			}
			else
			{
				remFlag( programFlags, ProgramFlag::eInstantiation );
			}

			return { mesh, skeleton };
		}

		//*****************************************************************************************

		void doSortRenderNodes( SceneRenderPass & renderPass
			, RenderMode mode
			, SceneNode const * ignored
			, QueueRenderNodes & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			uint32_t instanceMult = renderPass.getInstanceMult();
			auto & scene = nodes.getOwner()->getCuller().getScene();

			for ( auto & culledNode : renderPass.getCuller().getAllSubmeshes( mode ).objects )
			{
				auto & submesh = culledNode.data;
				auto pass = culledNode.pass;
				auto & instance = culledNode.instance;
				auto material = pass->getOwner()->shared_from_this();

				if ( ignored != &culledNode.sceneNode )
				{
					pass->prepareTextures();

					if ( renderPass.isValidPass( *pass ) )
					{
						auto programFlags = submesh.getProgramFlags( material );
						auto sceneFlags = scene.getFlags();
						auto textures = pass->getTexturesMask();
						auto animated = doAdjustFlags( *renderPass.getEngine()->getRenderSystem()
							, programFlags
							, sceneFlags
							, scene
							, *pass
							, renderPass
							, instance.getName() );
						auto flags = renderPass.prepareBackPipeline( *pass
							, textures
							, programFlags
							, sceneFlags
							, submesh.getTopology()
							, submesh.getGeometryBuffers( renderPass.getShaderFlags(), material, instanceMult, textures ).layouts
							, nodes.getDescriptorSetLayouts( *pass
								, submesh
								, animated.mesh.get()
								, animated.skeleton.get() ) );
						nodes.addRenderNode( flags
							, animated
							, culledNode
							, instance
							, *pass
							, submesh
							, renderPass );

						auto needsFront = ( mode == RenderMode::eTransparentOnly )
							|| pass->isTwoSided()
							|| renderPass.forceTwoSided()
							|| checkFlags( textures, TextureFlag::eOpacity ) != textures.end();

						if ( needsFront )
						{
							auto flags = renderPass.prepareFrontPipeline( *pass
								, textures
								, programFlags
								, sceneFlags
								, submesh.getTopology()
								, submesh.getGeometryBuffers( renderPass.getShaderFlags(), material, instanceMult, textures ).layouts
								, nodes.getDescriptorSetLayouts( *pass
									, submesh
									, animated.mesh.get()
									, animated.skeleton.get() ) );
							nodes.addRenderNode( flags
								, animated
								, culledNode
								, instance
								, *pass
								, submesh
								, renderPass );
						}
					}
				}
			}

			for ( auto & culledNode : renderPass.getCuller().getAllBillboards( mode ).objects )
			{
				auto & billboard = culledNode.data;
				auto & pass = culledNode.pass;

				pass->prepareTextures();
				auto programFlags = billboard.getProgramFlags();
				addFlag( programFlags, ProgramFlag::eBillboards );

				if ( renderPass.isValidPass( *pass )
					&& !isShadowMapProgram( programFlags ) )
				{
					auto sceneFlags = scene.getFlags();
					auto textures = pass->getTexturesMask();
					auto flags = renderPass.prepareBackPipeline( *pass
						, textures
						, programFlags
						, sceneFlags
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
						, billboard.getGeometryBuffers().layouts
						, nodes.getDescriptorSetLayouts( *pass
							, billboard ) );

					nodes.addRenderNode( flags
						, culledNode
						, *pass
						, billboard
						, renderPass );
				}
			}
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doInitialiseNodes( SceneRenderPass & renderPass
			, NodeByPipelineMapT< NodeT > & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNodes : nodes )
			{
				RenderPipeline & pipeline = *pipelineNodes.first;
				pipeline.createDescriptorPools( uint32_t( pipelineNodes.second.size() ) );

				for ( auto & culledNode : pipelineNodes.second )
				{
					if ( pipeline.hasDescriptorSetLayout() )
					{
						renderPass.initialiseAdditionalDescriptor( pipeline
							, pipeline.getDescriptorPool()
							, *culledNode.second
							, shadowMaps );
					}
				}
			}
		}

		template< typename NodeT >
		void doInitialiseInstancedNodes( SceneRenderPass & renderPass
			, ObjectNodesByPipelineMapT< NodeT > & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNodes : nodes )
			{
				uint32_t size = 0u;
				RenderPipeline & pipeline = *pipelineNodes.first;

				for ( auto & passNodes : pipelineNodes.second )
				{
					size += uint32_t( passNodes.second.size() );
				}

				pipeline.createDescriptorPools( size );

				if ( pipeline.getDescriptorSetLayout() )
				{
					renderPass.initialiseAdditionalDescriptor( pipeline
						, pipeline.getDescriptorPool()
						, pipelineNodes.second
						, shadowMaps );
				}
			}
		}
	}

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
	{
	}

	void QueueRenderNodes::parse( ShadowMapLightTypeArray & shadowMaps )
	{
		auto & queue = *getOwner();
		instancedStaticNodes.backCulled.clear();
		instancedStaticNodes.frontCulled.clear();
		staticNodes.backCulled.clear();
		staticNodes.frontCulled.clear();
		skinnedNodes.backCulled.clear();
		skinnedNodes.frontCulled.clear();
		instancedSkinnedNodes.backCulled.clear();
		instancedSkinnedNodes.frontCulled.clear();
		morphingNodes.backCulled.clear();
		morphingNodes.frontCulled.clear();
		billboardNodes.backCulled.clear();
		billboardNodes.frontCulled.clear();

		castor3d::doSortRenderNodes( *queue.getOwner()
			, queue.getMode()
			, queue.getIgnoredNode()
			, *this
			, shadowMaps );

		auto & renderPass = *queue.getOwner();
		renderPass.getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [&renderPass, this, shadowMaps]( RenderDevice const & device )
			{
				getOwner()->getCuller().getScene().getRenderNodes().initialiseNodes( device );

				doInitialiseNodes( renderPass, staticNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, staticNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, skinnedNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, morphingNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, billboardNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, billboardNodes.backCulled, shadowMaps );

				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedStaticNodes.backCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, instancedSkinnedNodes.backCulled, shadowMaps );
			} ) );
	}

	void QueueRenderNodes::addRenderNode( PipelineFlags const & flags
		, AnimatedObjects const & animated
		, CulledSubmesh const & culledNode
		, Geometry & instance
		, Pass & pass
		, Submesh & submesh
		, SceneRenderPass & renderPass )
	{
		if ( instance.isShadowCaster()
			|| ( !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( animated.skeleton )
			{
				doAddSkinningNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, *animated.skeleton
					, culledNode
					, skinnedNodes
					, instancedSkinnedNodes );
			}
			else if ( animated.mesh )
			{
				doAddMorphingNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, *animated.mesh
					, culledNode
					, morphingNodes );
			}
			else
			{
				doAddStaticNode( renderPass
					, flags
					, pass
					, submesh
					, instance
					, culledNode
					, staticNodes
					, instancedStaticNodes );
			}
		}
	}

	void QueueRenderNodes::addRenderNode( PipelineFlags const & flags
		, CulledBillboard const & culledNode
		, Pass & pass
		, BillboardBase & billboard
		, SceneRenderPass & renderPass )
	{
		doAddNode( renderPass
			, flags
			, pass
			, billboardNodes
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
			, culledNode
			, [&renderPass, &pass, &billboard]( RenderPipeline & pipeline )
			{
				return renderPass.createBillboardNode( pass
					, pipeline
					, billboard );
			} );
	}

	bool QueueRenderNodes::hasNodes()const
	{
		return !staticNodes.backCulled.empty()
			|| !staticNodes.frontCulled.empty()
			|| !skinnedNodes.backCulled.empty()
			|| !skinnedNodes.frontCulled.empty()
			|| !instancedStaticNodes.backCulled.empty()
			|| !instancedStaticNodes.frontCulled.empty()
			|| !instancedSkinnedNodes.backCulled.empty()
			|| !instancedSkinnedNodes.frontCulled.empty()
			|| !morphingNodes.backCulled.empty()
			|| !morphingNodes.frontCulled.empty()
			|| !billboardNodes.backCulled.empty()
			|| !billboardNodes.frontCulled.empty();
	}

	SubmeshRenderNode & QueueRenderNodes::createNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( passNode
			, std::move( modelBuffer )
			, std::move( modelInstancesBuffer )
			, buffers
			, sceneNode
			, data
			, instance
			, mesh
			, skeleton );
	}

	BillboardRenderNode & QueueRenderNodes::createNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & instance
		, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( passNode
			, std::move( modelBuffer )
			, std::move( modelInstancesBuffer )
			, buffers
			, sceneNode
			, instance
			, billboardBuffer );
	}

	ashes::DescriptorSetLayoutCRefArray QueueRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, BillboardBase const & billboard )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().getDescriptorSetLayouts( pass
			, billboard );
	}

	ashes::DescriptorSetLayoutCRefArray QueueRenderNodes::getDescriptorSetLayouts( Pass const & pass
		, Submesh const & submesh
		, AnimatedMesh const * mesh
		, AnimatedSkeleton const * skeleton )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().getDescriptorSetLayouts( pass
			, submesh
			, mesh
			, skeleton );
	}
}
