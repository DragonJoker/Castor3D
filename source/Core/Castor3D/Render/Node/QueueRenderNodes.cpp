#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Technique/Visibility/VisibilityResolvePass.hpp"
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
	namespace queuerndnd
	{
		template< typename NodeT
			, typename OnSubmeshFuncT >
		static void doTraverseNodes( ObjectNodesPtrByPipelineMapT< NodeT > & nodes
			, OnSubmeshFuncT onSubmesh )
		{
			for ( auto & itPipelines : nodes )
			{
				auto & pipeline = *itPipelines.first;

				for ( auto & itBuffers : itPipelines.second )
				{
					for ( auto & itPass : itBuffers.second )
					{
						for ( auto & itSubmeshes : itPass.second )
						{
							onSubmesh( pipeline
								, *itPass.first
								, *itSubmeshes.first
								, itSubmeshes.second );
						}
					}
				}
			}
		}

		static VkDeviceSize compareOffsets( ObjectBufferOffset const & lhs
			, ObjectBufferOffset const & rhs )
		{
			auto result = lhs.getOffset( SubmeshFlag::ePositions ) >= rhs.getOffset( SubmeshFlag::ePositions );
			return result;
		}

		template< typename NodeT >
		static VkDeviceSize compareOffsets( NodeT const & lhs
			, NodeT const & rhs )
		{
			return compareOffsets( lhs.getFinalBufferOffsets()
				, rhs.getFinalBufferOffsets() );
		}

		template< typename NodeT >
		static void doAddRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, uint32_t drawCount
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( *pipeline.getOwner()
				, node.data
				, *node.pass
				, flags.hasInvertNormals() );

			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			NodePtrByBufferMapT< NodeT > & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, NodePtrByBufferMapT< NodeT >{} ) ).first->second.second;
			NodePtrArrayT< NodeT > & bufferMap = pipelineMap.emplace( buffer, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( bufferMap.begin()
				, bufferMap.end()
				, [&node]( CountedNodeT< NodeT > const & lookup )
				{
					return compareOffsets( *lookup.node, node );
				} );
			CU_Require( buffer );
			bufferMap.emplace( it, CountedNodeT< NodeT >{ &node, drawCount } );
		}

		template< typename NodeT >
		static void doAddRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, uint32_t drawCount
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( *pipeline.getOwner()
				, node.data
				, *node.pass
				, flags.hasInvertNormals() );

			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			ObjectNodesPtrByBufferMapT< NodeT > & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, ObjectNodesPtrByBufferMapT< NodeT >{} ) ).first->second.second;
			ObjectNodesPtrByPassT< NodeT > & bufferMap = pipelineMap.emplace( buffer, ObjectNodesPtrByPassT< NodeT >{} ).first->second;
			ObjectNodesPtrMapT< NodeT > & passMap = bufferMap.emplace( node.pass, ObjectNodesPtrMapT< NodeT >{} ).first->second;
			NodePtrArrayT< NodeT > & objectMap = passMap.emplace( &node.data, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( objectMap.begin()
				, objectMap.end()
				, [&node]( CountedNodeT< NodeT > const & lookup )
				{
					return compareOffsets( *lookup.node, node );
				} );
			CU_Require( buffer );
			objectMap.emplace( it, CountedNodeT< NodeT >{ &node, drawCount } );
		}

		//*****************************************************************************************

		template< typename NodeT >
		static uint32_t doBindPipeline( ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller const & culler
			, RenderPipeline const & pipeline
			, ashes::BufferBase const & buffer
			, NodeT const & node
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor )
		{
			commandBuffer.bindPipeline( pipeline.getPipeline() );

			if ( viewport )
			{
				commandBuffer.setViewport( *viewport );
			}

			if ( scissor )
			{
				commandBuffer.setScissor( *scissor );
			}

			if ( pipeline.hasAdditionalDescriptorSetLayout() )
			{
				commandBuffer.bindDescriptorSet( pipeline.getAdditionalDescriptorSet(), pipeline.getPipelineLayout() );
			}

			uint32_t pipelineId{};

			if ( pipeline.getRenderSystem().hasFeature( GpuFeature::eBindless ) )
			{
				commandBuffer.bindDescriptorSet( *pipeline.getOwner()->getCuller().getScene().getBindlessTexDescriptorSet()
					, pipeline.getPipelineLayout() );

				pipelineId = culler.getPipelineNodesIndex( nodesPass
					, node.data
					, *node.pass
					, buffer
					, pipeline.getFlags().hasInvertNormals() );

				if ( !pipeline.hasMeshletDescriptorSetLayout() )
				{
					DrawConstants constants{ pipelineId };
					commandBuffer.pushConstants( pipeline.getPipelineLayout()
						, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( DrawConstants )
						, &constants );
				}
			}

			return pipelineId;
		}

		static bool isBufferEnabled( uint32_t submeshFlagIndex
			, PipelineFlags const & pipelineFlags )
		{
			if ( submeshFlagIndex == getIndex( SubmeshFlag::ePositions ) )
			{
				return pipelineFlags.enablePosition();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eNormals ) )
			{
				return pipelineFlags.enableNormal();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTangents ) )
			{
				return pipelineFlags.enableTangentSpace();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords0 ) )
			{
				return pipelineFlags.enableTexcoord0();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords1 ) )
			{
				return pipelineFlags.enableTexcoord1();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords2 ) )
			{
				return pipelineFlags.enableTexcoord2();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords3 ) )
			{
				return pipelineFlags.enableTexcoord3();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eColours ) )
			{
				return pipelineFlags.enableColours();
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eVelocity ) )
			{
				return pipelineFlags.enableVelocity();
			}

			return true;
		}

		template< typename NodeT >
		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t drawCount
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			GeometryBuffers const & geometryBuffers = node.getGeometryBuffers( pipeline.getFlags() );
			uint32_t currentLayout = 0u;

			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eCount ); ++i )
			{
				if ( geometryBuffers.bufferOffset.buffers[i].buffer
					&& isBufferEnabled( i, pipeline.getFlags() ) )
				{
					commandBuffer.bindVertexBuffer( geometryBuffers.layouts[currentLayout].get().vertexBindingDescriptions[0].binding
						, geometryBuffers.bufferOffset.buffers[i].getBuffer()
						, 0u );
					++currentLayout;
				}
			}

			for ( uint32_t i = 0u; i < geometryBuffers.other.size(); ++i )
			{
				commandBuffer.bindVertexBuffer( geometryBuffers.layouts[currentLayout].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.other[i]
					, geometryBuffers.otherOffsets[i] );
				++currentLayout;
			}

			if ( geometryBuffers.indexOffset.hasData() )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.indexOffset.getBuffer()
					, 0u
					, VK_INDEX_TYPE_UINT32 );
				commandBuffer.drawIndexedIndirect( indirectIndexedCommands.getBuffer()
					, idxIndex * sizeof( VkDrawIndexedIndirectCommand )
					, drawCount
					, sizeof( VkDrawIndexedIndirectCommand ) );
				idxIndex += drawCount;
			}
			else
			{
				commandBuffer.drawIndirect( indirectCommands.getBuffer()
					, nidxIndex * sizeof( VkDrawIndirectCommand )
					, drawCount
					, sizeof( VkDrawIndirectCommand ) );
				nidxIndex += drawCount;
			}
		}

		template< typename NodeT >
		static void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller & culler
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			for ( auto & pipelines : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelines.second.first;

				for ( auto & buffers : pipelines.second.second )
				{
					auto pipelineId = doBindPipeline( commandBuffer
						, nodesPass
						, culler
						, pipeline
						, *buffers.first
						, *buffers.second.front().node
						, viewport
						, scissor );
					doAddGeometryNodeCommands( pipeline
						, *buffers.second.front().node
						, commandBuffer
						, nodesPass
						, indirectIndexedCommands
						, indirectCommands
						, uint32_t( buffers.second.size() )
						, idxIndex
						, nidxIndex );

					if constexpr ( VisibilityResolvePass::useCompute )
					{
						for ( auto & nodeIt : buffers.second )
						{
							culler.registerNodePipeline( nodesPass, nodeIt.node->getId(), pipelineId );
						}
					}
				}
			}
		}

#if VK_NV_mesh_shader

		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, uint32_t pipelineId
			, uint32_t drawOffset
			, uint32_t drawCount
			, uint32_t & mshIndex )
		{
			if ( pipeline.hasMeshletDescriptorSetLayout() )
			{
				commandBuffer.bindDescriptorSet( node.getMeshletDescriptorSet()
					, pipeline.getPipelineLayout() );
			}

			MeshletDrawConstants constants{ pipelineId
				, drawOffset
				, node.getInstanceCount() };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_TASK_BIT_NV |  VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( MeshletDrawConstants )
				, &constants );
			commandBuffer.drawMeshTasksIndirect( indirectMeshCommands.getBuffer()
				, mshIndex * sizeof( VkDrawMeshTasksIndirectCommandNV )
				, drawCount
				, sizeof( VkDrawMeshTasksIndirectCommandNV ) );
			mshIndex += drawCount;
		}

		template< typename NodeT >
		static void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller & culler
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			for ( auto & pipelineIt : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelineIt.second.first;
				NodePtrByBufferMapT< NodeT > & buffers = pipelineIt.second.second;

				for ( auto & bufferIt : buffers )
				{
					NodePtrArrayT< NodeT > & nodes = bufferIt.second;
					auto pipelineId = doBindPipeline( commandBuffer
						, nodesPass
						, culler
						, pipeline
						, *bufferIt.first
						, *nodes.front().node
						, viewport
						, scissor );

					if ( nodesPass.isMeshShading()
						&& pipeline.hasMeshletDescriptorSetLayout() )
					{
						uint32_t drawOffset{};

						for ( auto & nodeIt : nodes )
						{
							doAddGeometryNodeCommands( pipeline
								, *nodeIt.node
								, commandBuffer
								, nodesPass
								, indirectMeshCommands
								, pipelineId
								, drawOffset
								, nodeIt.drawCount
								, mshIndex );
							drawOffset += nodeIt.drawCount;

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								culler.registerNodePipeline( nodesPass, nodeIt.node->getId(), pipelineId );
							}
						}
					}
					else
					{
						doAddGeometryNodeCommands( pipeline
							, *nodes.front().node
							, commandBuffer
							, nodesPass
							, indirectIndexedCommands
							, indirectCommands
							, uint32_t( nodes.size() )
							, idxIndex
							, nidxIndex );

						if constexpr ( VisibilityResolvePass::useCompute )
						{
							for ( auto & nodeIt : nodes )
							{
								culler.registerNodePipeline( nodesPass, nodeIt.node->getId(), pipelineId );
							}
						}
					}
				}
			}
		}

		static void doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller & culler
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			for ( auto & pipelineIt : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelineIt.second.first;
				ObjectNodesPtrByBufferMapT< SubmeshRenderNode > & buffers = pipelineIt.second.second;

				for ( auto & bufferIt : buffers )
				{
					ObjectNodesPtrByPassT< SubmeshRenderNode > & passes = bufferIt.second;

					for ( auto & passIt : passes )
					{
						ObjectNodesPtrMapT< SubmeshRenderNode > & submeshes = passIt.second;

						for ( auto & submeshIt : submeshes )
						{
							auto pipelineId = doBindPipeline( commandBuffer
								, nodesPass
								, culler
								, pipeline
								, *bufferIt.first
								, *submeshIt.second.front().node
								, viewport
								, scissor );

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								for ( auto & nodeIt : submeshIt.second )
								{
									culler.registerNodePipeline( nodesPass, nodeIt.node->getId(), pipelineId );
								}
							}

							auto & node = *submeshIt.second.front().node;

							if ( nodesPass.isMeshShading()
								&& pipeline.hasMeshletDescriptorSetLayout() )
							{
								doAddGeometryNodeCommands( pipeline
									, node
									, commandBuffer
									, nodesPass
									, indirectMeshCommands
									, pipelineId
									, 0u
									, node.getInstanceCount()
									, mshIndex );
							}
							else
							{
								doAddGeometryNodeCommands( pipeline
									, node
									, commandBuffer
									, nodesPass
									, indirectIndexedCommands
									, indirectCommands
									, 1u
									, idxIndex
									, nidxIndex );
							}
						}
					}
				}
			}
		}

#else

		template< typename NodeT >
		static void doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller & culler
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			for ( auto & pipelineIt : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelineIt.second.first;

				for ( auto & bufferIt : pipelineIt.second.second )
				{
					for ( auto & passIt : bufferIt.second )
					{
						for ( auto & submeshIt : passIt.second )
						{
							auto pipelineId = doBindPipeline( commandBuffer
								, nodesPass
								, culler
								, pipeline
								, *bufferIt.first
								, *submeshIt.second.front()
								, viewport
								, scissor );
							doAddGeometryNodeCommands( pipeline
								, *submeshIt.second.front()
								, commandBuffer
								, nodesPass
								, indirectIndexedCommands
								, indirectCommands
								, 1u
								, idxIndex
								, nidxIndex );

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								for ( auto & nodeIt : submeshIt.second )
								{
									culler.registerNodePipeline( nodesPass, nodeIt.node->getId(), pipelineId );
								}
							}
						}
					}
				}
			}
		}

#endif
	}

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
	{
	}

	void QueueRenderNodes::sort( ShadowMapLightTypeArray & shadowMaps )
	{
		submeshNodes.clear();
		instancedSubmeshNodes.clear();
		billboardNodes.clear();

		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		auto & culler = queue.getCuller();
		auto & scene = culler.getScene();

		for ( auto itPipeline : culler.getSubmeshNodes( renderPass ) )
		{
			for ( auto itBuffer : itPipeline.second )
			{
				for ( auto sidedCulled : itBuffer.second )
				{
					auto culled = sidedCulled.first;

					if ( culled.visible )
					{
						auto culledNode = culled.node;
						auto & submesh = culledNode->data;
						auto & pass = *culledNode->pass;
						auto material = pass.getOwner();
						auto submeshFlags = submesh.getSubmeshFlags( &pass );
						auto programFlags = submesh.getProgramFlags( *material );
						auto sceneFlags = scene.getFlags();
						auto textures = pass.getTexturesMask();
						auto pipelineFlags = renderPass.createPipelineFlags( pass
							, textures
							, submeshFlags
							, programFlags
							, sceneFlags
							, submesh.getTopology()
							, sidedCulled.second
							, submesh.getMorphTargets() );
						auto vertexLayouts = culledNode->getGeometryBuffers( pipelineFlags ).layouts;
						auto & pipeline = sidedCulled.second
							? renderPass.prepareFrontPipeline( pipelineFlags
								, vertexLayouts
								, culledNode->getMeshletDescriptorLayout() )
							: renderPass.prepareBackPipeline( pipelineFlags
								, vertexLayouts
								, culledNode->getMeshletDescriptorLayout() );
						queuerndnd::doAddRenderNode( pipeline
							, *culledNode
							, culled.instanceCount
							, submeshNodes );
						renderPass.initialiseAdditionalDescriptor( pipeline
							, shadowMaps
							, submesh.getMorphTargets() );

						if ( pipelineFlags.usesMesh() )
						{
							culledNode->createMeshletDescriptorSet();
						}
					}
				}
			}
		}

		for ( auto itPipeline : culler.getInstancedSubmeshNodes( renderPass ) )
		{
			for ( auto itBuffer : itPipeline.second )
			{
				for ( auto itPass : itBuffer.second )
				{
					for ( auto itSubmesh : itPass.second )
					{
						for ( auto sidedCulled : itSubmesh.second )
						{
							auto culled = sidedCulled.first;

							if ( culled.visible )
							{
								auto culledNode = culled.node;
								auto & submesh = culledNode->data;
								auto & pass = *culledNode->pass;
								auto material = pass.getOwner();
								auto submeshFlags = submesh.getSubmeshFlags( &pass );
								auto programFlags = submesh.getProgramFlags( *material );
								auto sceneFlags = scene.getFlags();
								auto textures = pass.getTexturesMask();
								auto pipelineFlags = renderPass.createPipelineFlags( pass
									, textures
									, submeshFlags
									, programFlags
									, sceneFlags
									, submesh.getTopology()
									, sidedCulled.second
									, submesh.getMorphTargets() );
								auto vertexLayouts = culledNode->getGeometryBuffers( pipelineFlags ).layouts;
								auto & pipeline = sidedCulled.second
									? renderPass.prepareFrontPipeline( pipelineFlags
										, vertexLayouts
										, culledNode->getMeshletDescriptorLayout() )
									: renderPass.prepareBackPipeline( pipelineFlags
										, vertexLayouts
										, culledNode->getMeshletDescriptorLayout() );
								queuerndnd::doAddRenderNode( pipeline
									, *culledNode
									, culled.instanceCount
									, instancedSubmeshNodes );
								renderPass.initialiseAdditionalDescriptor( pipeline
									, shadowMaps
									, submesh.getMorphTargets() );

								if ( pipelineFlags.usesMesh() )
								{
									culledNode->createMeshletDescriptorSet();
								}
							}
						}
					}
				}
			}
		}

		for ( auto itPipeline : culler.getBillboardNodes( renderPass ) )
		{
			for ( auto itBuffer : itPipeline.second )
			{
				for ( auto sidedCulled : itBuffer.second )
				{
					auto culled = sidedCulled.first;

					if ( culled.visible )
					{
						auto culledNode = culled.node;
						auto & billboard = culledNode->data;
						auto & pass = *culledNode->pass;
						auto submeshFlags = billboard.getSubmeshFlags();
						auto programFlags = billboard.getProgramFlags();
						auto sceneFlags = scene.getFlags();
						auto textures = pass.getTexturesMask();
						auto pipelineFlags = renderPass.createPipelineFlags( pass
							, textures
							, submeshFlags
							, programFlags
							, sceneFlags
							, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
							, sidedCulled.second
							, {} );
						auto & pipeline = renderPass.prepareBackPipeline( pipelineFlags
							, billboard.getGeometryBuffers().layouts
							, nullptr );
						queuerndnd::doAddRenderNode( pipeline
							, *culledNode
							, culled.instanceCount
							, billboardNodes );
						renderPass.initialiseAdditionalDescriptor( pipeline
							, shadowMaps
							, {} );
					}
				}
			}
		}
	}

	void QueueRenderNodes::prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors )
	{
		auto & queue = *getOwner();
		auto & rp = *queue.getOwner();
		auto & culler = queue.getCuller();

		ashes::CommandBuffer const & cb = queue.getCommandBuffer();
		cb.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( rp.getRenderPass()
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );

#if VK_NV_mesh_shader

		auto & submeshMshCommands = culler.getSubmeshMeshletCommands( rp );
		auto & submeshIdxCommands = culler.getSubmeshIdxCommands( rp );
		auto & submeshNIdxCommands = culler.getSubmeshNIdxCommands( rp );
		uint32_t mshIndex{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		queuerndnd::doParseRenderNodesCommands( submeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshMshCommands
			, submeshIdxCommands
			, submeshNIdxCommands
			, mshIndex
			, idxIndex
			, nidxIndex );
		queuerndnd::doParseRenderNodesCommands( instancedSubmeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshMshCommands
			, submeshIdxCommands
			, submeshNIdxCommands
			, mshIndex
			, idxIndex
			, nidxIndex );

#else

		auto & submeshIdxCommands = culler.getSubmeshIdxCommands( rp );
		auto & submeshNIdxCommands = culler.getSubmeshNIdxCommands( rp );
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		queuerndnd::doParseRenderNodesCommands( submeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshIdxCommands
			, submeshNIdxCommands
			, idxIndex
			, nidxIndex );
		queuerndnd::doParseRenderNodesCommands( instancedSubmeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshIdxCommands
			, submeshNIdxCommands
			, idxIndex
			, nidxIndex );

#endif

		auto & billboardCommands = culler.getBillboardCommands( rp );
		idxIndex = 0u;
		nidxIndex = 0u;
		queuerndnd::doParseRenderNodesCommands( billboardNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshIdxCommands
			, billboardCommands
			, idxIndex
			, nidxIndex );

		cb.end();
	}

	SubmeshRenderNode & QueueRenderNodes::createNode( Pass & pass
		, Submesh & data
		, Geometry & instance
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( pass
			, data
			, instance
			, mesh
			, skeleton );
	}

	BillboardRenderNode & QueueRenderNodes::createNode( Pass & pass
		, BillboardBase & instance )
	{
		return getOwner()->getCuller().getScene().getRenderNodes().createNode( pass
			, instance );
	}

	//*********************************************************************************************
}
