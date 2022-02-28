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
	namespace
	{
		uint64_t getPipelineHash( RenderPipeline const & pipeline )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( flags.programFlags
				, flags.passFlags
				, uint32_t( flags.textures.size() )
				, flags.texturesFlags );
			uint64_t result = uint64_t( intptr_t( &pipeline ) ) & 0x00000000FFFFFFFF;
			return ( uint64_t( baseHash ) << 32 ) | result;
		}

		template< typename NodeT
			, typename OnSubmeshFuncT >
			void doTraverseNodes( ObjectNodesPtrByPipelineMapT< NodeT > & nodes
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

		VkDeviceSize compareOffsets( ObjectBufferOffset const & lhs
			, ObjectBufferOffset const & rhs )
		{
			auto result = lhs.getVertexOffset() >= rhs.getVertexOffset();
			return result;
		}
		template< typename NodeT >
		VkDeviceSize compareOffsets( NodeT const & lhs
			, NodeT const & rhs )
		{
			return compareOffsets( lhs.data.getBufferOffsets(), rhs.data.getBufferOffsets() );
		}

		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( flags.programFlags
				, flags.passFlags
				, uint32_t( flags.textures.size() )
				, flags.texturesFlags );

			auto & bufferOffsets = node.data.getBufferOffsets();
			auto & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, NodePtrByBufferMapT< NodeT >{} ) ).first->second.second;
			auto & bufferMap = pipelineMap.emplace( bufferOffsets.vtxBuffer, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( bufferMap.begin()
				, bufferMap.end()
				, [&node]( NodeT const * lookup )
				{
					return compareOffsets( *lookup, node );
				} );
			CU_Require( bufferOffsets.vtxBuffer );
			bufferMap.emplace( it, &node );
		}

		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes )
		{
			auto & flags = pipeline.getFlags();
			auto baseHash = getPipelineBaseHash( flags.programFlags
				, flags.passFlags
				, uint32_t( flags.textures.size() )
				, flags.texturesFlags );

			auto & bufferOffsets = node.data.getBufferOffsets();
			auto & pipelineMap = nodes.emplace( baseHash, std::make_pair( &pipeline, ObjectNodesPtrByBufferMapT< NodeT >{} ) ).first->second.second;
			auto & bufferMap = pipelineMap.emplace( bufferOffsets.vtxBuffer, ObjectNodesPtrByPassT< NodeT >{} ).first->second;
			auto & passMap = bufferMap.emplace( &node.pass, ObjectNodesPtrMapT< NodeT >{} ).first->second;
			auto & objectMap = passMap.emplace( &node.data, NodePtrArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( objectMap.begin()
				, objectMap.end()
				, [&node]( NodeT const * lookup )
				{
					return compareOffsets( *lookup, node );
				} );
			CU_Require( bufferOffsets.vtxBuffer );
			objectMap.emplace( it, &node );
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doBindPipeline( ashes::CommandBuffer const & commandBuffer
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

			if ( pipeline.hasDescriptorSetLayout() )
			{
				commandBuffer.bindDescriptorSet( pipeline.getAdditionalDescriptorSet(), pipeline.getPipelineLayout() );
			}

			if ( pipeline.getRenderSystem().hasFeature( GpuFeature::eBindless ) )
			{
				commandBuffer.bindDescriptorSet( *pipeline.getOwner()->getCuller().getScene().getBindlessTexDescriptorSet()
					, pipeline.getPipelineLayout() );

				auto pipelineId = culler.getPipelineNodesIndex( nodesPass
					, node.data
					, node.pass
					, buffer
					, checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInvertNormals ) );
				commandBuffer.pushConstants( pipeline.getPipelineLayout()
					, VK_SHADER_STAGE_VERTEX_BIT
					, 0u
					, 4u
					, &pipelineId );
			}
		}

		template< typename NodeT >
		void doAddGeometryNodeCommands( RenderPipeline const & pipeline
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
				, pipeline.getFlags().programFlags
				, *node.pass.getOwner()
				, node.pass.getTexturesMask()
				, checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eForceTexCoords ) );
			commandBuffer.bindVertexBuffer( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
				, geometryBuffers.bufferOffset.getVertexBuffer()
				, 0u );

			for ( uint32_t i = 0u; i < geometryBuffers.other.size(); ++i )
			{
				commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i + 1u].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.other[i]
					, geometryBuffers.otherOffsets[i] );
			}

			if ( geometryBuffers.bufferOffset.idxBuffer )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.bufferOffset.getIndexBuffer()
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
		void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
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

		template< typename NodeT >
		void doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< NodeT > & inputNodes
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
					auto culledNode = sidedCulled.first;
					auto & submesh = culledNode->data;
					auto & pass = culledNode->pass;
					auto material = pass.getOwner();
					auto programFlags = submesh.getProgramFlags( material );
					auto sceneFlags = scene.getFlags();
					auto textures = pass.getTexturesMask();
					auto pipelineFlags = renderPass.createPipelineFlags( pass
						, textures
						, programFlags
						, sceneFlags
						, submesh.getTopology()
						, sidedCulled.second );
					auto vertexLayouts = submesh.getGeometryBuffers( renderPass.getShaderFlags()
						, pipelineFlags.programFlags
						, material
						, textures
						, checkFlag( pipelineFlags.programFlags, ProgramFlag::eForceTexCoords ) ).layouts;
					auto & pipeline = sidedCulled.second
						? renderPass.prepareFrontPipeline( pipelineFlags
							, vertexLayouts )
						: renderPass.prepareBackPipeline( pipelineFlags
							, vertexLayouts );
					doAddRenderNode( pipeline
						, *culledNode
						, submeshNodes );
					renderPass.initialiseAdditionalDescriptor( pipeline
						, shadowMaps );
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
							auto culledNode = sidedCulled.first;
							auto & submesh = culledNode->data;
							auto & pass = culledNode->pass;
							auto material = pass.getOwner();
							auto programFlags = submesh.getProgramFlags( material );
							auto sceneFlags = scene.getFlags();
							auto textures = pass.getTexturesMask();
							auto pipelineFlags = renderPass.createPipelineFlags( pass
								, textures
								, programFlags
								, sceneFlags
								, submesh.getTopology()
								, sidedCulled.second );
							auto vertexLayouts = submesh.getGeometryBuffers( renderPass.getShaderFlags()
								, pipelineFlags.programFlags
								, material
								, textures
								, checkFlag( pipelineFlags.programFlags, ProgramFlag::eForceTexCoords ) ).layouts;
							auto & pipeline = sidedCulled.second
								? renderPass.prepareFrontPipeline( pipelineFlags
									, vertexLayouts )
								: renderPass.prepareBackPipeline( pipelineFlags
									, vertexLayouts );
							doAddRenderNode( pipeline
								, *culledNode
								, instancedSubmeshNodes );
							renderPass.initialiseAdditionalDescriptor( pipeline
								, shadowMaps );
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
					auto culledNode = sidedCulled.first;
					auto & billboard = culledNode->data;
					auto & pass = culledNode->pass;
					auto programFlags = billboard.getProgramFlags();
					auto sceneFlags = scene.getFlags();
					auto textures = pass.getTexturesMask();
					auto pipelineFlags = renderPass.createPipelineFlags( pass
						, textures
						, programFlags
						, sceneFlags
						, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
						, sidedCulled.second );
					auto & pipeline = renderPass.prepareBackPipeline( pipelineFlags
						, billboard.getGeometryBuffers().layouts );
					doAddRenderNode( pipeline
						, *culledNode
						, billboardNodes );
					renderPass.initialiseAdditionalDescriptor( pipeline
						, shadowMaps );
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
		auto & submeshIdxCommands = culler.getSubmeshIdxCommands( rp );
		auto & submeshNIdxCommands = culler.getSubmeshNIdxCommands( rp );
		auto & billboardCommands = culler.getBillboardCommands( rp );

		ashes::CommandBuffer const & cb = queue.getCommandBuffer();
		cb.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( rp.getRenderPass()
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		doParseRenderNodesCommands( submeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshIdxCommands
			, submeshNIdxCommands
			, idxIndex
			, nidxIndex );
		doParseRenderNodesCommands( instancedSubmeshNodes
			, cb
			, rp
			, culler
			, viewport
			, scissors
			, submeshIdxCommands
			, submeshNIdxCommands
			, idxIndex
			, nidxIndex );

		idxIndex = 0u;
		nidxIndex = 0u;
		doParseRenderNodesCommands( billboardNodes
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

	bool PipelineComp::operator()( RenderPipeline const * lhs, RenderPipeline const * rhs )const
	{
		return getPipelineHash( *lhs ) < getPipelineHash( *rhs );
	}

	//*********************************************************************************************
}
