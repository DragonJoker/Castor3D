#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/RenderQueue.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Mesh/Mesh.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Skeleton/Skeleton.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>

using namespace castor;

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	namespace
	{
		template< typename MapType, typename FuncType >
		void doTraverseNodes( MapType & nodes
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeType, typename MapType, typename CulledType >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeType && node
			, CulledType const & culled
			, MapType & nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			ObjectRenderNodesArray tmp;
			auto itPipeline = nodes.emplace( &pipeline, std::move( tmp ) ).first;
			itPipeline->second.emplace( &culled, std::move( node ) );
		}

		template< typename NodeType, typename MapType, typename CulledType >
		void doAddRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeType && node
			, Submesh & object
			, CulledType const & culled
			, MapType & nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap{} ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap{} ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray{} ).first;
			itObject->second.emplace( &culled, std::move( node ) );
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeType * node
			, MapType & nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			ObjectRenderNodesArray tmp;
			auto itPipeline = nodes.emplace( &pipeline, std::move( tmp ) ).first;
			itPipeline->second.emplace_back( node );
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeType * node
			, Submesh & object
			, MapType & nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap{} ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap{} ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray{} ).first;
			itObject->second.emplace_back( node );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene const & scene
			, String const & name )
		{
			AnimatedObjectSPtr result;
			auto & cache = scene.getAnimatedObjectGroupCache();
			using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
			LockType lock{ makeUniqueLock( cache ) };

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

		template< typename CreatorFunc
			, typename NodesType
			, typename CulledType
			, typename ... Params >
		void doAddNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, NodesType & nodes
			, VkPrimitiveTopology topology
			, CulledType const & culled
			, CreatorFunc creator )
		{
			if ( pass.IsTwoSided() || pass.hasAlphaBlending() )
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

		void doAddSkinningNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton
			, SceneCuller::CulledSubmesh const & culled
			, SceneRenderNodes::SkinnedNodesMap & animated
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						doAddRenderNode( pass
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
					doAddRenderNode( pass
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

		void doAddMorphingNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh
			, SceneCuller::CulledSubmesh const & culled
			, SceneRenderNodes::MorphingNodesMap & animated )
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

		void doAddStaticNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, SceneCuller::CulledSubmesh const & culled
			, SceneRenderNodes::StaticNodesMap & statics
			, SceneRenderNodes::InstantiatedStaticNodesMap & instanced )
		{
			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( flags );

					if ( pipeline )
					{
						auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
						doAddRenderNode( pass
							, *pipeline
							, node
							, submesh
							, culled
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( flags );

				if ( pipeline )
				{
					auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
					doAddRenderNode( pass
						, *pipeline
						, node
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

		void doAddBillboardNode( RenderPass & renderPass
			, PipelineFlags const & flags
			, Pass & pass
			, BillboardBase & billboard
			, SceneCuller::CulledBillboard const & culled
			, SceneRenderNodes::BillboardNodesMap & nodes )
		{
			doAddNode( renderPass
				, flags
				, pass
				, nodes
				, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
				, culled
				, [&renderPass, &pass, &billboard]( RenderPipeline & pipeline )
				{
					return renderPass.createBillboardNode( pass
						, pipeline
						, billboard );
				} );
		}

		template< typename MapType >
		void doInitialiseNodes( RenderPass & renderPass
			, MapType & pipelineNodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNode : pipelineNodes )
			{
				pipelineNode.first->createDescriptorPools( uint32_t( pipelineNode.second.size() ) );

				for ( auto & node : pipelineNode.second )
				{
					renderPass.initialiseUboDescriptor( pipelineNode.first->getDescriptorPool( 0u ), node.second );
					Pass & pass = node.second.passNode.pass;

					if ( pipelineNode.first->hasDescriptorPool( 1u ) )
					{
						renderPass.initialiseTextureDescriptor( pipelineNode.first->getDescriptorPool( 1u )
							, node.second
							, shadowMaps );
					}
				}
			}
		}

		template< typename MapType >
		void doInitialiseInstancedNodes( RenderPass & renderPass
			, MapType & pipelineNodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			for ( auto & pipelineNode : pipelineNodes )
			{
				uint32_t size = 0u;
				RenderPipeline & pipeline = *pipelineNode.first;

				for ( auto & passNodes : pipelineNode.second )
				{
					size += uint32_t( passNodes.second.size() );
				}

				pipelineNode.first->createDescriptorPools( size );
				renderPass.initialiseUboDescriptor( pipeline.getDescriptorPool( 0u )
					, pipelineNode.second );

				if ( pipeline.hasDescriptorPool( 1u ) )
				{
					renderPass.initialiseTextureDescriptor( pipeline.getDescriptorPool( 1u )
						, pipelineNode.second
						, shadowMaps );
				}
			}
		}

		struct AnimatedObjects
		{
			AnimatedMeshSPtr mesh;
			AnimatedSkeletonSPtr skeleton;
		};

		AnimatedObjects doAdjustFlags( ProgramFlags & programFlags
			, TextureFlags & textures
			, uint32_t & texturesCount
			, PassFlags const & passFlags
			, SceneFlags const & sceneFlags
			, Scene const & scene
			, Pass const & pass
			, RenderPass const & renderPass
			, castor::String const & name )
		{
			auto submeshFlags = programFlags;
			remFlag( programFlags, ProgramFlag::eSkinning );
			remFlag( programFlags, ProgramFlag::eMorphing );
			auto mesh = std::static_pointer_cast< AnimatedMesh >( doFindAnimatedObject( scene, name + cuT( "_Mesh" ) ) );
			auto skeleton = std::static_pointer_cast< AnimatedSkeleton >( doFindAnimatedObject( scene, name + cuT( "_Skeleton" ) ) );

			if ( skeleton )
			{
				addFlag( programFlags, ProgramFlag::eSkinning );
			}

			if ( mesh )
			{
				addFlag( programFlags, ProgramFlag::eMorphing );
			}

			if ( checkFlag( submeshFlags, ProgramFlag::eInstantiation )
				&& ( !pass.hasAlphaBlending() || renderPass.isOrderIndependent() )
				&& !pass.hasEnvironmentMapping() )
			{
				addFlag( programFlags, ProgramFlag::eInstantiation );
			}
			else
			{
				remFlag( programFlags, ProgramFlag::eInstantiation );
			}

			return { mesh, skeleton };
		}

		void doAddRenderNode( PipelineFlags const & flags
			, AnimatedObjects const & animated
			, SceneCuller::CulledSubmesh const & culledNode
			, Geometry & instance
			, Pass & pass
			, Submesh & submesh
			, RenderPass & renderPass
			, SceneRenderNodes & nodes )
		{
			if ( !isShadowMapProgram( flags.programFlags )
				|| instance.isShadowCaster() )
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
						, nodes.skinnedNodes
						, nodes.instancedSkinnedNodes );
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
						, nodes.morphingNodes );
				}
				else
				{
					doAddStaticNode( renderPass
						, flags
						, pass
						, submesh
						, instance
						, culledNode
						, nodes.staticNodes
						, nodes.instancedStaticNodes );
				}
			}
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool opaque
			, SceneNode const * ignored
			, SceneRenderNodes & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			auto & scene = nodes.scene;

			for ( auto & culledNode : renderPass.getCuller().getAllSubmeshes( opaque ) )
			{
				auto & submesh = culledNode.data;
				auto pass = culledNode.pass;
				auto & instance = culledNode.instance;
				auto material = pass->getOwner()->shared_from_this();

				if ( ignored != &culledNode.sceneNode )
				{
					pass->prepareTextures();
					auto passFlags = pass->getPassFlags();

					if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != opaque )
					{
						auto programFlags = submesh.getProgramFlags( material );
						auto sceneFlags = scene.getFlags();
						auto textures = pass->getTextures();
						auto texturesCount = pass->getNonEnvTextureUnitsCount();
						auto animated = doAdjustFlags( programFlags
							, textures
							, texturesCount
							, passFlags
							, sceneFlags
							, scene
							, *pass
							, renderPass
							, instance.getName() );
						auto flags = renderPass.prepareBackPipeline( pass->getColourBlendMode()
							, pass->getAlphaBlendMode()
							, pass->getAlphaFunc()
							, passFlags
							, textures
							, texturesCount
							, pass->getHeightTextureIndex()
							, programFlags
							, sceneFlags
							, submesh.getTopology()
							, submesh.getGeometryBuffers( material ).layouts );
						doAddRenderNode( flags
							, animated
							, culledNode
							, instance
							, *pass
							, submesh
							, renderPass
							, nodes );

						auto needsFront = !opaque
							|| pass->IsTwoSided()
							|| checkFlag( textures, TextureFlag::eOpacity );

						if ( needsFront )
						{
							auto flags = renderPass.prepareFrontPipeline( pass->getColourBlendMode()
								, pass->getAlphaBlendMode()
								, pass->getAlphaFunc()
								, passFlags
								, textures
								, texturesCount
								, pass->getHeightTextureIndex()
								, programFlags
								, sceneFlags
								, submesh.getTopology()
								, submesh.getGeometryBuffers( material ).layouts );
							doAddRenderNode( flags
								, animated
								, culledNode
								, instance
								, *pass
								, submesh
								, renderPass
								, nodes );
						}
					}
				}
			}
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool opaque
			, Scene const & scene
			, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & nodes
			, ShadowMapLightTypeArray & shadowMaps )
		{
			for ( auto & culledNode : renderPass.getCuller().getAllBillboards( opaque ) )
			{
				auto & billboard = culledNode.data;
				auto & pass = culledNode.pass;

				pass->prepareTextures();
				auto sceneFlags = scene.getFlags();
				auto passFlags = pass->getPassFlags();
				auto programFlags = billboard.getProgramFlags();
				auto textures = pass->getTextures();
				auto texturesCount = pass->getNonEnvTextureUnitsCount();
				addFlag( programFlags, ProgramFlag::eBillboards );
				auto flags = renderPass.prepareBackPipeline( pass->getColourBlendMode()
					, pass->getAlphaBlendMode()
					, pass->getAlphaFunc()
					, passFlags
					, textures
					, texturesCount
					, pass->getHeightTextureIndex()
					, programFlags
					, sceneFlags
					, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
					, billboard.getGeometryBuffers().layouts );

				if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != opaque
					&& !isShadowMapProgram( programFlags ) )
				{
					doAddBillboardNode( renderPass
						, flags
						, *pass
						, billboard
						, culledNode
						, nodes );
				}
			}
		}

		GeometryBuffers const & getGeometryBuffers( Submesh const & submesh
			, MaterialSPtr material )
		{
			return submesh.getGeometryBuffers( material );
		}

		GeometryBuffers const & getGeometryBuffers( BillboardBase const & billboard
			, MaterialSPtr material )
		{
			return billboard.getGeometryBuffers();
		}

		template< typename CulledMapType
			, typename AllMapType >
			void doAddRenderNodes( CulledMapType & outputNodes
				, ashes::CommandBuffer const & commandBuffer
				, RenderPipeline & pipeline
				, Pass & pass
				, AllMapType & renderNodes
				, std::vector< SceneCuller::CulledSubmesh * > const & culledNodes )
		{
			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.begin()
					, culledNodes.end()
					, node.first );

				if ( it != culledNodes.end() )
				{
					doAddRenderNode( pass, pipeline, &node.second, ( *it )->data, outputNodes );
				}
			}
		}

		template< typename MapType
			, typename CulledMapType
			, typename CulledArrayType >
			void doParseRenderNodes( MapType & inputNodes
				, CulledMapType & outputNodes
				, CulledArrayType const & culledNodes
				, ashes::CommandBuffer const & commandBuffer )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					auto it = std::find( culledNodes.begin()
						, culledNodes.end()
						, node.first );

					if ( it != culledNodes.end() )
					{
						doAddRenderNode( *pipelines.first, &node.second, outputNodes );
					}
				}
			}
		}

		template< typename NodeType >
		void doAddRenderNodeCommand( RenderPipeline & pipeline
			, NodeType const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceCount = 1u )
		{
			if ( instanceCount )
			{
				GeometryBuffers const & geometryBuffers = getGeometryBuffers( node.data, node.passNode.pass.getOwner()->shared_from_this() );

				commandBuffer.bindPipeline( pipeline.getPipeline() );

				if ( viewport )
				{
					commandBuffer.setViewport( *viewport );
				}

				if ( scissor )
				{
					commandBuffer.setScissor( *scissor );
				}

				commandBuffer.bindDescriptorSet( *node.uboDescriptorSet, pipeline.getPipelineLayout() );

				if ( node.texDescriptorSet )
				{
					commandBuffer.bindDescriptorSet( *node.texDescriptorSet, pipeline.getPipelineLayout() );
				}

				for ( uint32_t i = 0; i < geometryBuffers.vbo.size(); ++i )
				{
					commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i].get().vertexBindingDescriptions[0].binding
						, geometryBuffers.vbo[i]
						, geometryBuffers.vboOffsets[i] );
				}

				if ( geometryBuffers.ibo )
				{
					commandBuffer.bindIndexBuffer( *geometryBuffers.ibo
						, geometryBuffers.iboOffset
						, VK_INDEX_TYPE_UINT32 );
					commandBuffer.drawIndexed( geometryBuffers.idxCount
						, instanceCount );
				}
				else
				{
					commandBuffer.draw( geometryBuffers.vtxCount
						, instanceCount );
				}
			}
		}

		template< typename NodeType >
		void doAddRenderNodeCommand( Pass & pass
			, RenderPipeline & pipeline
			, NodeType const & node
			, Submesh & object
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceCount = 1u )
		{
			if ( instanceCount )
			{
				GeometryBuffers const & geometryBuffers = object.getGeometryBuffers( pass.getOwner()->shared_from_this() );

				commandBuffer.bindPipeline( pipeline.getPipeline() );

				if ( viewport )
				{
					commandBuffer.setViewport( *viewport );
				}

				if ( scissor )
				{
					commandBuffer.setScissor( *scissor );
				}

				commandBuffer.bindDescriptorSet( *node.uboDescriptorSet, pipeline.getPipelineLayout() );

				if ( node.texDescriptorSet )
				{
					commandBuffer.bindDescriptorSet( *node.texDescriptorSet, pipeline.getPipelineLayout() );
				}

				for ( uint32_t i = 0; i < geometryBuffers.vbo.size(); ++i )
				{
					commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i].get().vertexBindingDescriptions[0].binding
						, geometryBuffers.vbo[i]
						, geometryBuffers.vboOffsets[i] );
				}

				if ( geometryBuffers.ibo )
				{
					commandBuffer.bindIndexBuffer( *geometryBuffers.ibo
						, geometryBuffers.iboOffset
						, VK_INDEX_TYPE_UINT32 );
					commandBuffer.drawIndexed( geometryBuffers.idxCount
						, instanceCount );
				}
				else
				{
					commandBuffer.draw( geometryBuffers.vtxCount
						, instanceCount );
				}
			}
		}

		template< typename CulledMapType >
		void doAddRenderNodesCommands( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, CulledMapType & renderNodes )
		{
			doAddRenderNodeCommand( pass
				, pipeline
				, *renderNodes[0]
				, submesh
				, commandBuffer
				, viewport
				, scissor
				, uint32_t( renderNodes.size() ) );
		}

		template< typename MapType >
		void doParseRenderNodesCommands( MapType & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					doAddRenderNodeCommand( *pipelines.first, *node, commandBuffer, viewport, scissor );
				}
			}
		}

		template<>
		void doParseRenderNodesCommands( BillboardRenderNodesPtrByPipelineMap & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					doAddRenderNodeCommand( *pipelines.first, *node, commandBuffer, viewport, scissor, node->instance.getCount() );
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderPass & renderPass
		, bool opaque
		, SceneNode const * ignored )
		: OwnedBy< RenderPass >{ renderPass }
		, m_opaque{ opaque }
		, m_ignoredNode{ ignored }
		, m_culler{ getOwner()->getCuller() }
		, m_onCullerCompute( m_culler.onCompute.connect( [this]( SceneCuller const & culler )
			{
				doOnCullerCompute( culler );
			} ) )
		, m_renderNodes{ std::make_unique< SceneRenderNodes >( m_culler.getScene() ) }
		, m_culledRenderNodes{ std::make_unique< SceneCulledRenderNodes >( m_culler.getScene() ) }
		, m_viewport{ castor::makeChangeTracked< ashes::Optional< VkViewport > >( ashes::nullopt ) }
		, m_scissor{ castor::makeChangeTracked< ashes::Optional< VkRect2D > >( ashes::nullopt ) }
	{
		getOwner()->getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & device = getCurrentRenderDevice( *getOwner()->getEngine() );
				m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_SECONDARY );
			} ) );
	}

	void RenderQueue::cleanup()
	{
		m_culledRenderNodes.reset();
		m_renderNodes.reset();
		m_commandBuffer.reset();
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps )
	{
		if ( m_allChanged )
		{
			doParseAllRenderNodes( shadowMaps );
			m_allChanged = false;
		}

		bool commandBuffersChanged = m_culledChanged
			|| m_viewport.isDirty()
			|| m_scissor.isDirty();

		if ( m_culledChanged )
		{
			doParseCulledRenderNodes();
			m_culledChanged = false;
		}

		if ( commandBuffersChanged )
		{
			// Force regeneration of CommandBuffers if running.
			m_preparation = ( m_preparation == Preparation::eWaiting )
				? Preparation::eWaiting
				: Preparation::eDone;
		}

		if ( commandBuffersChanged
			&& m_preparation == Preparation::eDone )
		{
			m_preparation = Preparation::eWaiting;
			getOwner()->getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [this]()
				{
					m_preparation = Preparation::eRunning;
					m_commandBuffer->reset();
					doPrepareCommandBuffer();
					m_preparation = ( m_preparation == Preparation::eWaiting )
						? Preparation::eWaiting
						: Preparation::eDone;
				} ) );
		}
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, VkViewport const & viewport
		, VkRect2D const & scissor )
	{
		m_viewport = viewport;
		m_scissor = scissor;
		update( shadowMaps );
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, VkRect2D const & scissor )
	{
		m_scissor = scissor;
		update( shadowMaps );
	}

	void RenderQueue::doPrepareCommandBuffer()
	{
		auto & culledNodes = getCulledRenderNodes();
		auto viewport = m_viewport.value();
		auto scissor = m_scissor.value();
		m_viewport.reset();
		m_scissor.reset();

		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( VkRenderPass( getOwner()->getRenderPass() )
				, 0u
				, VkFramebuffer( VK_NULL_HANDLE )
				, VkBool32( VK_FALSE )
				, 0u
				, 0u ) );

		doTraverseNodes( culledNodes.instancedStaticNodes.frontCulled
			, [this, &viewport, &scissor]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodesCommands( *m_commandBuffer
					, viewport
					, scissor
					, pipeline
					, pass
					, submesh
					, nodes );
			} );
		doTraverseNodes( culledNodes.instancedStaticNodes.backCulled
			, [this, &viewport, &scissor]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodesCommands< StaticRenderNodePtrArray >( *m_commandBuffer
					, viewport
					, scissor
					, pipeline
					, pass
					, submesh
					, nodes );
			} );
		doTraverseNodes( culledNodes.instancedSkinnedNodes.frontCulled
			, [this, &viewport, &scissor]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodesCommands< SkinningRenderNodePtrArray >( *m_commandBuffer
					, viewport
					, scissor
					, pipeline
					, pass
					, submesh
					, nodes );
			} );
		doTraverseNodes( culledNodes.instancedSkinnedNodes.backCulled
			, [this, &viewport, &scissor]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodesCommands< SkinningRenderNodePtrArray >( *m_commandBuffer
					, viewport
					, scissor
					, pipeline
					, pass
					, submesh
					, nodes );
			} );

		doParseRenderNodesCommands( culledNodes.staticNodes.frontCulled
			, *m_commandBuffer
			, viewport
			, scissor );
		doParseRenderNodesCommands( culledNodes.staticNodes.backCulled
			, *m_commandBuffer
			, viewport
			, scissor );

		doParseRenderNodesCommands( culledNodes.skinnedNodes.frontCulled
			, *m_commandBuffer
			, viewport
			, scissor );
		doParseRenderNodesCommands( culledNodes.skinnedNodes.backCulled
			, *m_commandBuffer
			, viewport
			, scissor );

		doParseRenderNodesCommands( culledNodes.morphingNodes.frontCulled
			, *m_commandBuffer
			, viewport
			, scissor );
		doParseRenderNodesCommands( culledNodes.morphingNodes.backCulled
			, *m_commandBuffer
			, viewport
			, scissor );

		doParseRenderNodesCommands( culledNodes.billboardNodes.frontCulled
			, *m_commandBuffer
			, viewport
			, scissor );
		doParseRenderNodesCommands( culledNodes.billboardNodes.backCulled
			, *m_commandBuffer
			, viewport
			, scissor );

		m_commandBuffer->end();
		m_viewport.reset();
		m_scissor.reset();
	}

	void RenderQueue::doParseAllRenderNodes( ShadowMapLightTypeArray & shadowMaps )
	{
		auto & culledNodes = getCulledRenderNodes();
		auto & allNodes = getAllRenderNodes();
		allNodes.instancedStaticNodes.backCulled.clear();
		allNodes.instancedStaticNodes.frontCulled.clear();
		allNodes.staticNodes.backCulled.clear();
		allNodes.staticNodes.frontCulled.clear();
		allNodes.skinnedNodes.backCulled.clear();
		allNodes.skinnedNodes.frontCulled.clear();
		allNodes.instancedSkinnedNodes.backCulled.clear();
		allNodes.instancedSkinnedNodes.frontCulled.clear();
		allNodes.morphingNodes.backCulled.clear();
		allNodes.morphingNodes.frontCulled.clear();
		allNodes.billboardNodes.backCulled.clear();
		allNodes.billboardNodes.frontCulled.clear();

		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, m_ignoredNode
			, allNodes
			, shadowMaps );
		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, allNodes.scene
			, allNodes.billboardNodes
			, shadowMaps );

		auto & renderPass = *getOwner();
		renderPass.getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [&renderPass, &allNodes, shadowMaps]()
			{
				doInitialiseNodes( renderPass, allNodes.staticNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.staticNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.skinnedNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.skinnedNodes.backCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.morphingNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.morphingNodes.backCulled, shadowMaps );

				doInitialiseInstancedNodes( renderPass, allNodes.instancedStaticNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, allNodes.instancedStaticNodes.backCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, allNodes.instancedSkinnedNodes.frontCulled, shadowMaps );
				doInitialiseInstancedNodes( renderPass, allNodes.instancedSkinnedNodes.backCulled, shadowMaps );
			} ) );
		renderPass.getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [&renderPass, &allNodes, shadowMaps]()
			{
				doInitialiseNodes( renderPass, allNodes.billboardNodes.frontCulled, shadowMaps );
				doInitialiseNodes( renderPass, allNodes.billboardNodes.backCulled, shadowMaps );
			} ) );
	}

	void RenderQueue::doParseCulledRenderNodes()
	{
		auto & culledNodes = getCulledRenderNodes();
		auto & allNodes = getAllRenderNodes();
		culledNodes.instancedStaticNodes.backCulled.clear();
		culledNodes.instancedStaticNodes.frontCulled.clear();
		culledNodes.staticNodes.backCulled.clear();
		culledNodes.staticNodes.frontCulled.clear();
		culledNodes.skinnedNodes.backCulled.clear();
		culledNodes.skinnedNodes.frontCulled.clear();
		culledNodes.instancedSkinnedNodes.backCulled.clear();
		culledNodes.instancedSkinnedNodes.frontCulled.clear();
		culledNodes.morphingNodes.backCulled.clear();
		culledNodes.morphingNodes.frontCulled.clear();
		culledNodes.billboardNodes.backCulled.clear();
		culledNodes.billboardNodes.frontCulled.clear();
		auto & culler = getOwner()->getCuller();

		doTraverseNodes( allNodes.instancedStaticNodes.frontCulled
			, [this, &culledNodes, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodes( culledNodes.instancedStaticNodes.frontCulled
					, *m_commandBuffer
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( m_opaque ) );
			} );
		doTraverseNodes( allNodes.instancedStaticNodes.backCulled
			, [this, &culledNodes, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodes( culledNodes.instancedStaticNodes.backCulled
					, *m_commandBuffer
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( m_opaque ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.frontCulled
			, [this, &culledNodes, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodes( culledNodes.instancedSkinnedNodes.frontCulled
					, *m_commandBuffer
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( m_opaque ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.backCulled
			, [this, &culledNodes, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doAddRenderNodes( culledNodes.instancedSkinnedNodes.backCulled
					, *m_commandBuffer
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( m_opaque ) );
			} );

		doParseRenderNodes( allNodes.staticNodes.frontCulled
			, culledNodes.staticNodes.frontCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );
		doParseRenderNodes( allNodes.staticNodes.backCulled
			, culledNodes.staticNodes.backCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );

		doParseRenderNodes( allNodes.skinnedNodes.frontCulled
			, culledNodes.skinnedNodes.frontCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );
		doParseRenderNodes( allNodes.skinnedNodes.backCulled
			, culledNodes.skinnedNodes.backCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );

		doParseRenderNodes( allNodes.morphingNodes.frontCulled
			, culledNodes.morphingNodes.frontCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );
		doParseRenderNodes( allNodes.morphingNodes.backCulled
			, culledNodes.morphingNodes.backCulled
			, culler.getCulledSubmeshes( m_opaque )
			, *m_commandBuffer );

		doParseRenderNodes( allNodes.billboardNodes.frontCulled
			, culledNodes.billboardNodes.frontCulled
			, culler.getCulledBillboards( m_opaque )
			, *m_commandBuffer );
		doParseRenderNodes( allNodes.billboardNodes.backCulled
			, culledNodes.billboardNodes.backCulled
			, culler.getCulledBillboards( m_opaque )
			, *m_commandBuffer );
	}

	void RenderQueue::doOnCullerCompute( SceneCuller const & culler )
	{
		m_allChanged = culler.areAllChanged();
		m_culledChanged = m_allChanged || culler.areCulledChanged();
	}
}
