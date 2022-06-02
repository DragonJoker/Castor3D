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
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
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
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( *pipeline.getOwner()
				, node.data
				, *node.pass
				, checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) );

			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			auto & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, NodePtrByBufferMapT< NodeT >{} ) ).first->second.second;
			auto & bufferMap = pipelineMap.emplace( buffer, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( bufferMap.begin()
				, bufferMap.end()
				, [&node]( NodeT const * lookup )
				{
					return compareOffsets( *lookup, node );
				} );
			CU_Require( buffer );
			bufferMap.emplace( it, &node );
		}

		template< typename NodeT >
		static void doAddRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( *pipeline.getOwner()
				, node.data
				, *node.pass
				, checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) );

			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			auto & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, ObjectNodesPtrByBufferMapT< NodeT >{} ) ).first->second.second;
			auto & bufferMap = pipelineMap.emplace( buffer, ObjectNodesPtrByPassT< NodeT >{} ).first->second;
			auto & passMap = bufferMap.emplace( node.pass, ObjectNodesPtrMapT< NodeT >{} ).first->second;
			auto & objectMap = passMap.emplace( &node.data, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( objectMap.begin()
				, objectMap.end()
				, [&node]( NodeT const * lookup )
				{
					return compareOffsets( *lookup, node );
				} );
			CU_Require( buffer );
			objectMap.emplace( it, &node );
		}

		//*****************************************************************************************

		template< typename NodeT >
		static void doBindPipeline( ashes::CommandBuffer const & commandBuffer
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

			if ( pipeline.getRenderSystem().hasFeature( GpuFeature::eBindless ) )
			{
				commandBuffer.bindDescriptorSet( *pipeline.getOwner()->getCuller().getScene().getBindlessTexDescriptorSet()
					, pipeline.getPipelineLayout() );

				auto pipelineId = culler.getPipelineNodesIndex( nodesPass
					, node.data
					, *node.pass
					, buffer
					, checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInvertNormals ) );

				if ( !pipeline.hasMeshletDescriptorSetLayout() )
				{
					commandBuffer.pushConstants( pipeline.getPipelineLayout()
						, VK_SHADER_STAGE_VERTEX_BIT
						, 0u
						, 4u
						, &pipelineId );
				}
				else
				{
					commandBuffer.pushConstants( pipeline.getPipelineLayout()
						, VK_SHADER_STAGE_MESH_BIT_NV
						, 0u
						, 4u
						, &pipelineId );
				}
			}
		}

		static bool isBufferEnabled( uint32_t submeshFlagIndex
			, ShaderFlags shaderFlags
			, PipelineFlags const & pipelineFlags )
		{
			if ( submeshFlagIndex == getIndex( SubmeshFlag::ePositions ) )
			{
				return checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::ePositions );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eNormals ) )
			{
				return checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eNormals );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTangents ) )
			{
				return checkFlag( shaderFlags, ShaderFlag::eTangentSpace );
			}

			bool hasTextures = !pipelineFlags.textures.empty()
				|| checkFlag( pipelineFlags.programFlags, ProgramFlag::eForceTexCoords );

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords0 ) )
			{
				return hasTextures
					&& checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eTexcoords0 );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords1 ) )
			{
				return hasTextures
					&& checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eTexcoords1 );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords2 ) )
			{
				return hasTextures
					&& checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eTexcoords2 );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eTexcoords3 ) )
			{
				return hasTextures
					&& checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eTexcoords3 );
			}

			if ( submeshFlagIndex == getIndex( SubmeshFlag::eColours ) )
			{
				return checkFlag( pipelineFlags.submeshFlags, SubmeshFlag::eColours );
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
			GeometryBuffers const & geometryBuffers = node.getGeometryBuffers( nodesPass.getShaderFlags()
				, pipeline.getFlags().submeshFlags
				, pipeline.getFlags().programFlags
				, node.pass->getTexturesMask() );
			uint32_t currentLayout = 0u;

			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eCount ); ++i )
			{
				if ( geometryBuffers.bufferOffset.buffers[i].buffer
					&& isBufferEnabled( i, pipeline.getOwner()->getShaderFlags(), pipeline.getFlags() ) )
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
			, SceneCuller const & culler
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
					doBindPipeline( commandBuffer
						, nodesPass
						, culler
						, pipeline
						, *buffers.first
						, *buffers.second.front()
						, viewport
						, scissor );
					doAddGeometryNodeCommands( pipeline
						, *buffers.second.front()
						, commandBuffer
						, nodesPass
						, indirectIndexedCommands
						, indirectCommands
						, uint32_t( buffers.second.size() )
						, idxIndex
						, nidxIndex );
				}
			}
		}

#if VK_NV_mesh_shader

		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, uint32_t drawCount
			, uint32_t & mshIndex )
		{
			if ( pipeline.hasMeshletDescriptorSetLayout() )
			{
				commandBuffer.bindDescriptorSet( node.getMeshletDescriptorSet()
					, pipeline.getPipelineLayout() );
			}

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
			, SceneCuller const & culler
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			for ( auto & pipelines : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelines.second.first;

				for ( auto & buffers : pipelines.second.second )
				{
					doBindPipeline( commandBuffer
						, nodesPass
						, culler
						, pipeline
						, *buffers.first
						, *buffers.second.front()
						, viewport
						, scissor );

					if ( pipeline.hasMeshletDescriptorSetLayout() )
					{
						doAddGeometryNodeCommands( pipeline
							, *buffers.second.front()
							, commandBuffer
							, nodesPass
							, indirectMeshCommands
							, uint32_t( buffers.second.size() )
							, mshIndex );
					}
					else
					{
						doAddGeometryNodeCommands( pipeline
							, *buffers.second.front()
							, commandBuffer
							, nodesPass
							, indirectIndexedCommands
							, indirectCommands
							, uint32_t( buffers.second.size() )
							, idxIndex
							, nidxIndex );
					}
				}
			}
		}

		template< typename NodeT >
		static void doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, SceneCuller const & culler
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

				for ( auto & bufferIt : pipelineIt.second.second )
				{
					for ( auto & passIt : bufferIt.second )
					{
						for ( auto & submeshIt : passIt.second )
						{
							doBindPipeline( commandBuffer
								, nodesPass
								, culler
								, pipeline
								, *bufferIt.first
								, *submeshIt.second.front()
								, viewport
								, scissor );

							if ( pipeline.hasMeshletDescriptorSetLayout() )
							{
								doAddGeometryNodeCommands( pipeline
									, *submeshIt.second.front()
									, commandBuffer
									, nodesPass
									, indirectMeshCommands
									, 1u
									, mshIndex );
							}
							else
							{
								doAddGeometryNodeCommands( pipeline
									, *submeshIt.second.front()
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
			, SceneCuller const & culler
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
							doBindPipeline( commandBuffer
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
						auto vertexLayouts = culledNode->getGeometryBuffers( renderPass.getShaderFlags()
							, pipelineFlags.submeshFlags
							, pipelineFlags.programFlags
							, textures ).layouts;
						auto & pipeline = sidedCulled.second
							? renderPass.prepareFrontPipeline( pipelineFlags
								, vertexLayouts
								, culledNode->getMeshletDescriptorLayout() )
							: renderPass.prepareBackPipeline( pipelineFlags
								, vertexLayouts
								, culledNode->getMeshletDescriptorLayout() );
						queuerndnd::doAddRenderNode( pipeline
							, *culledNode
							, submeshNodes );
						renderPass.initialiseAdditionalDescriptor( pipeline
							, shadowMaps
							, submesh.getMorphTargets() );

						if ( checkFlag( pipelineFlags.programFlags, ProgramFlag::eHasMesh ) )
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
								auto vertexLayouts = culledNode->getGeometryBuffers( renderPass.getShaderFlags()
									, pipelineFlags.submeshFlags
									, pipelineFlags.programFlags
									, textures ).layouts;
								auto & pipeline = sidedCulled.second
									? renderPass.prepareFrontPipeline( pipelineFlags
										, vertexLayouts
										, culledNode->getMeshletDescriptorLayout() )
									: renderPass.prepareBackPipeline( pipelineFlags
										, vertexLayouts
										, culledNode->getMeshletDescriptorLayout() );
								queuerndnd::doAddRenderNode( pipeline
									, *culledNode
									, instancedSubmeshNodes );
								renderPass.initialiseAdditionalDescriptor( pipeline
									, shadowMaps
									, submesh.getMorphTargets() );

								if ( checkFlag( pipelineFlags.programFlags, ProgramFlag::eHasMesh ) )
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
