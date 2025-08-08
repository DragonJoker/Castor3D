#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
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

#include <CastorUtils/Miscellaneous/BlockTimer.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/RecordContext.hpp>
#include <RenderGraph/RunnablePass.hpp>

#include <ashespp/Command/CommandBuffer.hpp>

CU_ImplementSmartPtr( c3d, QueueRenderNodes )

using ashes::operator==;
using ashes::operator!=;

#pragma warning( disable:4189 )

namespace c3d
{
	namespace queuerndnd
	{
		static bool constexpr C3D_PrintNodesCounts = false;
		static bool constexpr C3D_PrintNodesFullCounts = false;

		static VkDrawIndirectCommand getCommand( ObjectBufferOffset const &
			, ObjectBufferOffset::GpuBufferChunk const & bufferChunk
			, CulledNodeT< BillboardRenderNode > const & culled )
		{
			return VkDrawIndirectCommand{ .vertexCount = bufferChunk.getCount< BillboardVertex >()
				, .instanceCount = culled.instanceCount
				, .firstVertex = bufferChunk.getFirst< BillboardVertex >()
				, .firstInstance = 0u };
		}

		static VkDrawIndexedIndirectCommand getCommand( ObjectBufferOffset const & bufferOffsets
			, ObjectBufferOffset::GpuBufferChunk const & bufferChunk
			, CulledNodeT< SubmeshRenderNode > const & culled )
		{
			auto & indexOffset = bufferOffsets.getBufferChunk( SubmeshData::eIndex );
			return VkDrawIndexedIndirectCommand{ .indexCount = indexOffset.hasData() ? culled.indexCount : culled.vertexCount
				, .instanceCount = culled.instanceCount
				, .firstIndex = indexOffset.hasData() ? indexOffset.getFirst< uint32_t >() : 0u
				, .vertexOffset = int32_t( bufferChunk.getFirst< Point4f >() )
				, .firstInstance = 0u };
		}

		template< typename NodeT, template< typename NodeU > typename PipelinesNodesContainerT >
		static bool addRenderNode( PipelineAndID pipeline
			, CulledNodeT< NodeT > const & culled
			, bool isFrontCulled
			, PipelinesNodesContainerT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & node = *culled.node;

			if ( auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions );
				bufferChunk.buffer )
			{
				auto & posBuffer = bufferChunk.buffer->getBuffer();
				auto idxChunk = node.getSourceBufferOffsets().getBufferChunk( SubmeshData::eIndex );
				auto idxBuffer = idxChunk.hasData()
					? &idxChunk.getBuffer()
					: nullptr;

				if constexpr ( std::is_same_v< BillboardRenderNode, NodeT > )
				{
					nodes.emplace( pipeline
						, posBuffer
						, idxBuffer
						, culled
						, getCommand( node.getSourceBufferOffsets(), bufferChunk, culled )
						, isFrontCulled );
				}
				else
				{
					nodes.emplace( pipeline
						, posBuffer
						, idxBuffer
						, culled
						, getCommand( node.getSourceBufferOffsets(), bufferChunk, culled )
						, isFrontCulled );
				}

				registerPipelineNodes( pipeline.pipeline->getFlagsHash(), posBuffer, idxBuffer, nodesIds );
				return true;
			}

			return false;
		}

		//*****************************************************************************************

		template< typename NodeT >
		RenderedNodeT< NodeT > const * hasVisibleNode( NodesViewT< NodeT > const & nodes )
		{
			auto it = std::find_if( nodes.begin()
				, nodes.end()
				, []( RenderedNodeT< NodeT > const & node )
				{
					return node.visible;
				} );
			return it != nodes.end() ? &( *it ) : nullptr;
		}

		static bool hasVisibleInstance( HashSet< Geometry const * > const & instances )
		{
			return std::any_of( instances.begin()
				, instances.end()
				, []( Geometry const * lookup )
				{
					return lookup->getParent()->isVisible();
				} );
		}

		static uint32_t bindPipeline( ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, RenderPipeline const & pipeline
			, BufferBase const & posBuffer
			, BufferBase const * idxBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, bool hasDrawId )
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

				pipelineId = queueNodes.getPipelineNodesIndex( pipeline.getFlagsHash(), posBuffer, idxBuffer );

				if ( hasDrawId
					&& !pipeline.hasMeshletDescriptorSetLayout()
					&& !pipeline.hasVertexPullingDescriptorSetLayout() )
				{
					DrawConstants constants{ pipelineId, 0u };
					commandBuffer.pushConstants( pipeline.getPipelineLayout()
						, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( DrawConstants )
						, &constants );
				}
			}

			queueNodes.registerPipelineId( pipelineId );
			return pipelineId;
		}

		template< typename NodeT >
		static void registerIndirectNodeCommands( RenderPipeline const & pipeline
			, RenderedNodeT< NodeT > const & node
			, ashes::CommandBuffer const & commandBuffer
			, BufferT< VkDrawIndexedIndirectCommand > const * indirectIndexedCommands
			, BufferT< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t pipelineId
			, uint32_t drawId
			, uint32_t drawCount
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			auto & geometryBuffers = node.node->getGeometryBuffers( pipeline.getFlags() );

			if ( auto descriptorSet = node.node->getVertexPullingDescriptorSet() )
			{
				commandBuffer.bindDescriptorSet( *descriptorSet
					, pipeline.getPipelineLayout() );
			}
			else
			{
				commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.buffers
					, geometryBuffers.offsets );
			}

			DrawConstants constants{ pipelineId, int32_t( drawId ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( DrawConstants )
				, &constants );

			if ( geometryBuffers.indexOffset.hasData() && indirectIndexedCommands )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.indexOffset.getBuffer().getBuffer()
					, 0u
					, VK_INDEX_TYPE_UINT32 );
				commandBuffer.drawIndexedIndirect( indirectIndexedCommands->getBuffer()
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

		static void registerDirectNodeCommands( RenderPipeline const & pipeline
			, RenderedNodeT< SubmeshRenderNode > const & node
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t instanceCount
			, uint32_t pipelineId
			, uint32_t drawId
			, uint32_t *& pipelinesBuffer
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			auto & geometryBuffers = node.node->getGeometryBuffers( pipeline.getFlags() );

			if ( pipelinesBuffer )
			{
				( *pipelinesBuffer ) = node.node->getId();
				++pipelinesBuffer;
			}

			if ( auto descriptorSet = node.node->getVertexPullingDescriptorSet() )
			{
				commandBuffer.bindDescriptorSet( *descriptorSet
					, pipeline.getPipelineLayout() );
			}
			else
			{
				commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.buffers
					, geometryBuffers.offsets );
			}

			DrawConstants constants{ pipelineId, int32_t( drawId ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( DrawConstants )
				, &constants );

			if ( geometryBuffers.indexOffset.hasData() )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.indexOffset.getBuffer().getBuffer()
					, 0u
					, VK_INDEX_TYPE_UINT32 );
				commandBuffer.drawIndexed( node.command.indexCount
					, instanceCount
					, geometryBuffers.indexOffset.getFirst< uint32_t >()
					, node.node->getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions ).getFirst< Point4f >()
					, 0u );
				++idxIndex;
			}
			else
			{
				commandBuffer.draw( node.command.indexCount
					, instanceCount
					, node.node->getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions ).getFirst< Point4f >()
					, 0u );
				++nidxIndex;
			}
		}

		static void registerDirectNodeCommands( RenderPipeline const & pipeline
			, RenderedNodeT< BillboardRenderNode > const & node
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t instanceCount
			, uint32_t pipelineId
			, uint32_t drawId
			, uint32_t *& pipelinesBuffer
			, uint32_t & index )
		{
			( *pipelinesBuffer ) = node.node->getId();
			++pipelinesBuffer;
			auto & geometryBuffers = node.node->getGeometryBuffers( pipeline.getFlags() );

			if ( auto descriptorSet = node.node->getVertexPullingDescriptorSet() )
			{
				commandBuffer.bindDescriptorSet( *descriptorSet
					, pipeline.getPipelineLayout() );
			}
			else
			{
				commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.buffers
					, geometryBuffers.offsets );
			}

			DrawConstants constants{ pipelineId, int32_t( drawId ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( DrawConstants )
				, &constants );
			commandBuffer.draw( node.command.vertexCount
				, instanceCount
				, 0u
				, 0u );
			++index;
		}

#if VK_NV_mesh_shader

		static void registerMeshletNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, BufferT< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, uint32_t pipelineId
			, uint32_t drawOffset
			, uint32_t drawCount
			, uint32_t & mshIndex )
		{
			commandBuffer.bindDescriptorSet( node.getMeshletDescriptorSet()
				, pipeline.getPipelineLayout() );
			MeshletDrawConstants constants{ pipelineId
				, 0u
				, drawOffset
				, node.getSourceBufferOffsets().getFirst< Meshlet >( SubmeshData::eMeshlets ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_TASK_BIT_NV |  VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( MeshletDrawConstants )
				, &constants );
			commandBuffer.drawMeshTasksIndirectNV( *indirectMeshCommands.buffer
				, mshIndex * sizeof( VkDrawMeshTasksIndirectCommandNV )
				, drawCount
				, sizeof( VkDrawMeshTasksIndirectCommandNV ) );
			mshIndex += drawCount;
		}

#endif
#if VK_EXT_mesh_shader

		static void registerMeshletNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, BufferT< VkDrawMeshTasksIndirectCommandEXT > const & indirectMeshCommands
			, uint32_t pipelineId
			, uint32_t drawOffset
			, uint32_t drawCount
			, uint32_t & mshIndex )
		{
			commandBuffer.bindDescriptorSet( node.getMeshletDescriptorSet()
				, pipeline.getPipelineLayout() );
			MeshletDrawConstants constants{ pipelineId
				, 0u
				, drawOffset
				, node.getSourceBufferOffsets().getFirst< Meshlet >( SubmeshData::eMeshlets ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_TASK_BIT_NV |  VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( MeshletDrawConstants )
				, &constants );
			commandBuffer.drawMeshTasksIndirect( *indirectMeshCommands.buffer
				, mshIndex * sizeof( VkDrawMeshTasksIndirectCommandEXT )
				, drawCount
				, sizeof( VkDrawMeshTasksIndirectCommandEXT ) );
			mshIndex += drawCount;
		}

#endif

		//*****************************************************************************************

		static void fillNodeIndirectCommand( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, uint32_t instanceCount )
		{
			*indirectIndexedCommands = node.command;
			indirectIndexedCommands->instanceCount = instanceCount;
			++indirectIndexedCommands;
		}

		static void fillNodeIndirectCommand( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			indirectCommands->vertexCount = node.command.indexCount;
			indirectCommands->instanceCount = instanceCount;
			indirectCommands->firstVertex = 0u;
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillNodeIndirectCommand( RenderedNodeT< BillboardRenderNode > const & node
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount
			, uint32_t *& pipelinesBuffer )
		{
			*indirectCommands = node.command;
			indirectCommands->instanceCount = instanceCount;
			++indirectCommands;
			( *pipelinesBuffer ) = node.node->getId();
			++pipelinesBuffer;
		}

		static void fillNodeIndirectCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.node->getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillNodeIndirectCommand( node
					, indirectIdxBuffer
					, instanceCount );
			}
			else
			{
				fillNodeIndirectCommand( node
					, indirectNIdxBuffer
					, instanceCount );
			}
		}

		static void fillNodeIndirectCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeIndirectCommands( node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
			( *pipelinesBuffer ) = node.node->getId();
			++pipelinesBuffer;
		}

#if VK_NV_mesh_shader

		static void fillNodeMeshletCommand( RenderedNodeT< SubmeshRenderNode > const &
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshCommands
			, uint32_t instanceCount
			, uint32_t taskCount )
		{
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->taskCount = taskCount;
				indirectMeshCommands->firstTask = 0u;
				++indirectMeshCommands;
			}
		}

		static void fillNodeMeshletCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.node->data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillNodeMeshletCommand( node
					, indirectMeshBuffer
					, node.node->data.isDynamic() ? 1u : instanceCount
					, node.node->data.getMeshletsCount() );
			}
			else if ( node.node->getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillNodeIndirectCommand( node
					, indirectIdxBuffer
					, instanceCount );
			}
			else
			{
				fillNodeIndirectCommand( node
					, indirectNIdxBuffer
					, instanceCount );
			}
		}

		static void fillNodeMeshletCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeMeshletCommands( node
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
			( *pipelinesBuffer ) = node.node->getId();
			++pipelinesBuffer;
		}

#endif
#if VK_EXT_mesh_shader

		static void fillNodeIndirectCommand( RenderedNodeT< SubmeshRenderNode > const &
			, VkDrawMeshTasksIndirectCommandEXT *& indirectMeshCommands
			, uint32_t instanceCount
			, uint32_t taskCount )
		{
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->groupCountX = taskCount;
				indirectMeshCommands->groupCountY = 1u;
				indirectMeshCommands->groupCountZ = 1u;
				++indirectMeshCommands;
			}
		}

		static void fillNodeIndirectCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawMeshTasksIndirectCommandEXT *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.node->data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillNodeIndirectCommand( node
					, indirectMeshBuffer
					, node.node->data.isDynamic() ? 1u : instanceCount
					, node.node->data.getMeshletsCount() );
			}
			else if ( node.node->getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillNodeIndirectCommand( node
					, indirectIdxBuffer
					, instanceCount );
			}
			else
			{
				fillNodeIndirectCommand( node
					, indirectNIdxBuffer
					, instanceCount );
			}
		}

		static void fillNodeIndirectCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawMeshTasksIndirectCommandEXT *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeIndirectCommands( node
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
			( *pipelinesBuffer ) = node.node->getId();
			++pipelinesBuffer;
		}

#endif

		static size_t makeHash( SubmeshRenderNode const & node
			, bool frontCulled )
		{
			auto const & submesh = node.data;
			auto const & pass = *node.pass;
			size_t hash = std::hash< Submesh const * >{}( &submesh );
			hash = hashCombine( hash, pass.getHash() );
			hash = hashCombine( hash, node.isInstanced() );
			hash = hashCombine( hash, frontCulled );
			return hash;
		}

		static size_t makeHash( BillboardRenderNode const & node
			, bool )
		{
			auto const & billboard = node.data;
			auto const & pass = *node.pass;
			size_t hash = std::hash< BillboardBase const * >{}( &billboard );
			hash = hashCombine( hash, pass.getHash() );
			return hash;
		}

		template< typename NodeT >
		std::tuple< size_t, QueueRenderNodes::PipelineMap::iterator, PipelineFlags > getPipeline( RenderNodesPass const & renderPass
			, NodeT const & node
			, bool frontCulled
			, QueueRenderNodes::PipelineMap & pipelines
			, uint32_t vertexStride )
		{
			auto const & engine = *renderPass.getEngine();
			auto & components = engine.getSubmeshComponentsRegister();
			auto hash = makeHash( node, frontCulled );
			auto it = pipelines.find( hash );
			Pass const & pass = *node.pass;
			auto const & scene = renderPass.getCuller().getScene();
			auto submeshData = node.getRenderData();
			auto pipelineFlags = renderPass.createPipelineFlags( pass
				, pass.getTexturesMask()
				, components.getSubmeshComponentCombine( node.getComponentCombineID() )
				, node.getProgramFlags()
				, scene.getFlags()
				, submeshData ? submeshData->getPrimitiveTopology() : node.getPrimitiveTopology()
				, frontCulled
				, node.getMorphTargets()
				, node.getRenderData()
				, vertexStride );

			if ( submeshData )
			{
				pipelineFlags.submeshDataBindings = submeshData->getBindingCount();
				pipelineFlags.patchVertices = submeshData->getPatchVertices();
			}

			if ( it != pipelines.end()
				&& pipelineFlags != it->second.pipeline->getFlags() )
			{
				pipelines.erase( it );
				it = pipelines.end();
			}

			return { hash, it, pipelineFlags };
		}
	}

	//*************************************************************************************************

	template< typename NodeT >
	inline String getNodesTypeName();

	template<>
	inline String getNodesTypeName< SubmeshRenderNode >()
	{
		return cuT( "Submeshes" );
	}

	template<>
	inline String getNodesTypeName< BillboardRenderNode >()
	{
		return cuT( "Billboards" );
	}

	template< typename NodeT >
	static OutputStream & operator<<( OutputStream & stream, PipelinesNodesT< NodeT > const & rhs )
	{
		if constexpr ( queuerndnd::C3D_PrintNodesFullCounts )
		{
			stream << "    All " << getNodesTypeName< NodeT >() << " - " << rhs.size() << " pipelines\n";
			uint32_t maxBuffersCount{};
			uint32_t maxNodesCount{};
			uint32_t totalBuffersCount{};
			uint32_t totalNodesCount{};

			for ( auto & [id, pipeline] : rhs )
			{
				maxBuffersCount = std::max( maxBuffersCount, uint32_t( pipeline.nodes.size() ) );
				totalBuffersCount += uint32_t( pipeline.nodes.size() );
				stream << "        Pipeline 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << pipeline.pipeline.pipeline;
				stream << ": " << std::dec << pipeline.nodes.size() << " buffers\n";

				for ( auto & [buffer, nodes] : pipeline.nodes )
				{
					if ( queuerndnd::hasVisibleNode( nodes ) )
					{
						maxNodesCount = std::max( maxNodesCount, uint32_t( nodes.size() ) );
						totalNodesCount += uint32_t( nodes.size() );
						stream << "            Buffer 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << buffer;
						stream << ": " << std::dec << nodes.size() << " nodes\n";
					}
				}
			}

			stream << "\n        Max. buffers per pipeline: " << maxBuffersCount;
			stream << "\n        Max. nodes per buffer: " << maxNodesCount;
			stream << "\n        Total buffers: " << totalBuffersCount;
			stream << "\n        Total nodes: " << totalNodesCount;
			stream << "\n        Occupancy: " << rhs.occupancy();
		}
		else
		{
			stream << "    Single " << getNodesTypeName< NodeT >() << " - " << rhs.size() << " pipelines";
			uint32_t maxBuffersCount{};
			uint32_t maxNodesCount{};
			uint32_t totalBuffersCount{};
			uint32_t totalNodesCount{};

			for ( auto & [id, pipeline] : rhs )
			{
				maxBuffersCount = std::max( maxBuffersCount, uint32_t( pipeline.nodes.size() ) );
				totalBuffersCount += uint32_t( pipeline.nodes.size() );

				for ( auto & [buffer, nodes] : pipeline.nodes )
				{
					if ( queuerndnd::hasVisibleNode( nodes ) )
					{
						maxNodesCount = std::max( maxNodesCount, uint32_t( nodes.size() ) );
						totalNodesCount += uint32_t( nodes.size() );
					}
				}
			}

			stream << "\n        Max. buffers per pipeline: " << maxBuffersCount;
			stream << "\n        Max. nodes per buffer: " << maxNodesCount;
			stream << "\n        Total buffers: " << totalBuffersCount;
			stream << "\n        Total nodes: " << totalNodesCount;
			stream << "\n        Occupancy: " << rhs.occupancy();
		}

		return stream;
	}

	template< typename NodeT >
	static OutputStream & operator<<( OutputStream & stream, InstantiatedPipelinesNodesT< NodeT > const & rhs )
	{
		if constexpr ( queuerndnd::C3D_PrintNodesFullCounts )
		{
			stream << "    Instantiated " << getNodesTypeName< NodeT >() << " - " << rhs.size() << " pipelines\n";
			uint32_t maxBuffersCount{};
			uint32_t maxObjectsCount{};
			uint32_t maxNodesCount{};
			uint32_t totalBuffersCount{};
			uint32_t totalObjectsCount{};
			uint32_t totalNodesCount{};

			for ( auto & [id, pipeline] : rhs )
			{
				maxBuffersCount = std::max( maxBuffersCount, uint32_t( pipeline.nodes.size() ) );
				totalBuffersCount += uint32_t( pipeline.nodes.size() );
				stream << "        Pipeline 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << pipeline.pipeline.pipeline;
				stream << ": " << std::dec << pipeline.nodes.size() << " buffers\n";

				for ( auto & [buffer, submeshes] : pipeline.nodes )
				{
					maxObjectsCount = std::max( maxObjectsCount, uint32_t( submeshes.size() ) );
					totalObjectsCount += uint32_t( submeshes.size() );
					stream << "                Buffer 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << buffer;
					stream << ": " << std::dec << submeshes.size() << " submeshes\n";

					for ( auto & [submesh, node] : submeshes )
					{
						maxNodesCount = std::max( maxNodesCount, node.node->getInstanceCount() );
						totalNodesCount += node.node->getInstanceCount();
						stream << "                    Submesh 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << node.node;
						stream << ": " << std::dec << node.node->getInstanceCount() << "\n";
					}
				}
			}

			stream << "\n        Max.buffers per pipeline: " << maxBuffersCount;
			stream << "\n        Max.objects per pass: " << maxObjectsCount;
			stream << "\n        Max.nodes per object: " << maxNodesCount;
			stream << "\n        Total buffers: " << totalBuffersCount;
			stream << "\n        Total objects: " << totalObjectsCount;
			stream << "\n        Total nodes: " << totalNodesCount;
			stream << "\n        Occupancy: " << rhs.occupancy();
		}
		else
		{
			stream << "    Instantiated " << getNodesTypeName< NodeT >() << " - " << rhs.size() << " pipelines\n";
			uint32_t maxBuffersCount{};
			uint32_t maxObjectsCount{};
			uint32_t maxNodesCount{};
			uint32_t totalBuffersCount{};
			uint32_t totalObjectsCount{};
			uint32_t totalNodesCount{};
			for ( auto & [id, pipeline] : rhs )
			{
				maxBuffersCount = std::max( maxBuffersCount, uint32_t( pipeline.nodes.size() ) );
				totalBuffersCount += uint32_t( pipeline.nodes.size() );

				for ( auto & [buffer, submeshes] : pipeline.nodes )
				{
					maxObjectsCount = std::max( maxObjectsCount, uint32_t( submeshes.size() ) );
					totalObjectsCount += uint32_t( submeshes.size() );

					for ( auto & [submesh, node] : submeshes )
					{
						maxNodesCount = std::max( maxNodesCount, node.node->getInstanceCount() );
						totalNodesCount += node.node->getInstanceCount();
					}
				}
			}

			stream << "\n        Max.buffers per pipeline: " << maxBuffersCount;
			stream << "\n        Max.objects per buffer: " << maxObjectsCount;
			stream << "\n        Max.nodes per object: " << maxNodesCount;
			stream << "\n        Total buffers: " << totalBuffersCount;
			stream << "\n        Total objects: " << totalObjectsCount;
			stream << "\n        Total nodes: " << totalNodesCount;
			stream << "\n        Occupancy: " << rhs.occupancy();
		}

		return stream;
	}

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue
		, RenderDevice const & device
		, String const & typeName
		, bool meshShading )
		: OwnedBy< RenderQueue const >{ queue }
		, m_onSubmeshChanged{ queue.getCuller().onSubmeshChanged.connect( [this]( SceneCuller const &
				, CulledNodeT< SubmeshRenderNode > const & node
				, bool )
			{
				doAddSubmesh( node );
			} ) }
		, m_onSubmeshRemoved{ queue.getCuller().onSubmeshRemoved.connect( [this]( SceneCuller const &
				, CulledNodeT< SubmeshRenderNode > const & node
				, bool )
			{
				doRemoveSubmesh( node );
			} ) }
		, m_onBillboardChanged{ queue.getCuller().onBillboardChanged.connect( [this]( SceneCuller const &
				, CulledNodeT< BillboardRenderNode > const & node
				, bool )
			{
				doAddBillboard( node );
			} ) }
		, m_onBillboardRemoved{ queue.getCuller().onBillboardRemoved.connect( [this]( SceneCuller const &
				, CulledNodeT< BillboardRenderNode > const & node
				, bool )
			{
				doRemoveBillboard( node );
			} ) }
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader

		if ( meshShading )
		{
#	if defined( VK_EXT_mesh_shader ) && defined( VK_NV_mesh_shader )
			if ( device.prefersMeshShaderEXT() )
			{
				m_submeshMeshletIndirectCommandsEXT = makeBuffer< VkDrawMeshTasksIndirectCommandEXT >( device
					, getOwner()->getCuller().getScene().getResources()
					, MaxSubmeshMeshletDrawIndirectCommand
					, BufferUsageFlags::eIndirectBuffer
					, MemoryPropertyFlags::eHostVisible
					, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
			}
			else
			{
				m_submeshMeshletIndirectCommandsNV = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
					, getOwner()->getCuller().getScene().getResources()
					, MaxSubmeshMeshletDrawIndirectCommand
					, BufferUsageFlags::eIndirectBuffer
					, MemoryPropertyFlags::eHostVisible
					, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
			}
#	elif VK_EXT_mesh_shader
			m_submeshMeshletIndirectCommandsEXT = makeBuffer< IndexedMeshDrawCommandsBufferEXT >( device
					, getOwner()->getCuller().getScene().getResources()
				, MaxSubmeshMeshletDrawIndirectCommand
				, BufferUsageFlags::eIndirectBuffer
				, MemoryPropertyFlags::eHostVisible
				, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
#	else
			m_submeshMeshletIndirectCommandsNV = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
					, getOwner()->getCuller().getScene().getResources()
				, MaxSubmeshMeshletDrawIndirectCommand
				, BufferUsageFlags::eIndirectBuffer
				, MemoryPropertyFlags::eHostVisible
				, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
#	endif
		}

#endif
		m_submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
			, getOwner()->getCuller().getScene().getResources()
			, MaxSubmeshIdxDrawIndirectCommand
			, BufferUsageFlags::eIndirectBuffer
			, MemoryPropertyFlags::eHostVisible
			, typeName + cuT( "/SubmeshIndexedIndirectBuffer" ) );
		m_submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
			, getOwner()->getCuller().getScene().getResources()
			, MaxSubmeshNIdxDrawIndirectCommand
			, BufferUsageFlags::eIndirectBuffer
			, MemoryPropertyFlags::eHostVisible
			, typeName + cuT( "/SubmeshIndirectBuffer" ) );
		m_billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
			, getOwner()->getCuller().getScene().getResources()
			, MaxBillboardDrawIndirectCommand
			, BufferUsageFlags::eIndirectBuffer
			, MemoryPropertyFlags::eHostVisible
			, typeName + cuT( "/BillboardIndirectBuffer" ) );
		m_pipelinesNodes = makeBuffer< PipelineNodes >( device
			, getOwner()->getCuller().getScene().getResources()
			, MaxPipelinesNodes
			, BufferUsageFlags::eStorageBuffer
			, MemoryPropertyFlags::eHostVisible
			, typeName + cuT( "/NodesIDs" ) );
	}

	QueueRenderNodes::~QueueRenderNodes()noexcept
	{
		m_pipelinesNodes->destroy();
		m_pipelinesNodes.reset();
		m_billboardIndirectCommands->destroy();
		m_billboardIndirectCommands.reset();
		m_submeshNIdxIndirectCommands->destroy();
		m_submeshNIdxIndirectCommands.reset();
		m_submeshIdxIndirectCommands->destroy();
		m_submeshIdxIndirectCommands.reset();
#if VK_EXT_mesh_shader
		if ( m_submeshMeshletIndirectCommandsEXT )
		{
			m_submeshMeshletIndirectCommandsEXT->destroy();
			m_submeshMeshletIndirectCommandsEXT.reset();
		}
#endif
#if VK_NV_mesh_shader
		if ( m_submeshMeshletIndirectCommandsNV )
		{
			m_submeshMeshletIndirectCommandsNV->destroy();
			m_submeshMeshletIndirectCommandsNV.reset();
		}
#endif
	}

	void QueueRenderNodes::fillConfig( crg::ru::Config & config )const
	{
		config.prePassAction( [this]( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t )
			{
				context.memoryBarrier( commandBuffer
					, m_pipelinesNodes->bufferViewId
					, HostWriteState, DrawIndirectCommandState );
				context.memoryBarrier( commandBuffer
					, m_submeshIdxIndirectCommands->bufferViewId
					, HostWriteState, DrawIndirectCommandState );
				context.memoryBarrier( commandBuffer
					, m_submeshNIdxIndirectCommands->bufferViewId
					, HostWriteState, DrawIndirectCommandState );
				context.memoryBarrier( commandBuffer
					, m_billboardIndirectCommands->bufferViewId
					, HostWriteState, DrawIndirectCommandState );

				if ( m_submeshMeshletIndirectCommandsEXT )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsEXT->bufferViewId
						, HostWriteState, DrawIndirectCommandState );
				}

				if ( m_submeshMeshletIndirectCommandsNV )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsNV->bufferViewId 
						, HostWriteState, DrawIndirectCommandState );
				}
			} );
		config.postPassAction( [this]( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t )
			{
				context.memoryBarrier( commandBuffer
					, m_pipelinesNodes->bufferViewId
					, DrawIndirectCommandState, HostWriteState );
				context.memoryBarrier( commandBuffer
					, m_submeshIdxIndirectCommands->bufferViewId
					, DrawIndirectCommandState, HostWriteState );
				context.memoryBarrier( commandBuffer
					, m_submeshNIdxIndirectCommands->bufferViewId
					, DrawIndirectCommandState, HostWriteState );
				context.memoryBarrier( commandBuffer
					, m_billboardIndirectCommands->bufferViewId
					, DrawIndirectCommandState, HostWriteState );

				if ( m_submeshMeshletIndirectCommandsEXT )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsEXT->bufferViewId
						, DrawIndirectCommandState, HostWriteState );
				}

				if ( m_submeshMeshletIndirectCommandsNV )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsNV->bufferViewId
						, DrawIndirectCommandState, HostWriteState );
				}
			} );
	}

	void QueueRenderNodes::clear()noexcept
	{
		m_pipelines.clear();
		m_submeshNodes.clear();
		m_instancedSubmeshNodes.clear();
		m_billboardNodes.clear();
	}

	void QueueRenderNodes::checkEmpty()
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		auto & culler = queue.getCuller();
		auto submeshesIt = std::find_if( culler.getSubmeshes().begin()
			, culler.getSubmeshes().end()
			, [&renderPass]( CulledNodePtrT< SubmeshRenderNode > const & lookup )
			{
				auto & node = *lookup->node;
				return renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() )
					&& ( !node.isInstanced() || node.instance.getParent()->isVisible() );
			} );
		auto billboardsIt = std::find_if( culler.getBillboards().begin()
			, culler.getBillboards().end()
			, [&renderPass]( CulledNodePtrT< BillboardRenderNode > const & lookup )
			{
				auto & node = *lookup->node;
				return renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() );
			} );

		m_hasNodes = submeshesIt != culler.getSubmeshes().end()
			|| billboardsIt != culler.getBillboards().end();
	}

	bool QueueRenderNodes::sortNodes( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
			C3D_DebugTime( renderPass.getTypeName() );

			auto & culler = queue.getCuller();
			m_hasNodes = false;
			m_nodesIds.clear();
			m_submeshNodes.clear();
			m_instancedSubmeshNodes.clear();
			m_billboardNodes.clear();
			m_pendingSubmeshes.clear();
			m_pendingBillboards.clear();

			auto count = culler.getSubmeshes().size();
			m_nodesIds.reserve( count );

			for ( auto & culled : culler.getSubmeshes() )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() ) )
				{
					doAddSubmesh( shadowMaps, shadowBuffer, *culled );
				}
			}

			for ( auto & culled : culler.getBillboards() )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() ) )
				{
					doAddBillboard( shadowMaps, shadowBuffer, *culled );
				}
			}
		}
		renderPass.onSortNodes( renderPass );
		return m_pendingSubmeshes.empty() && m_pendingBillboards.empty();
	}

	bool QueueRenderNodes::updateNodes( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
			C3D_DebugTime( renderPass.getTypeName() );
			auto pendingSubmeshes = c3d::move( m_pendingSubmeshes );
			auto pendingBillboards = c3d::move( m_pendingBillboards );

			for ( auto culled : pendingSubmeshes )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() ) )
				{
					doAddSubmesh( shadowMaps, shadowBuffer, *culled );
				}
			}

			for ( auto culled : pendingBillboards )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() ) )
				{
					doAddBillboard( shadowMaps, shadowBuffer, *culled );
				}
			}
		}
		renderPass.onSortNodes( renderPass );
		return m_pendingSubmeshes.empty() && m_pendingBillboards.empty();
	}

	uint32_t QueueRenderNodes::prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, ashes::CommandBuffer const & commandBuffer )
	{
		uint32_t result{};
		m_visible = {};
		m_maxPipelineId = {};
		auto & queue = *getOwner();
		auto const & renderPass = *queue.getOwner();

		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass.getRenderPass( 0u )
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );

		if ( m_pipelinesNodes
			&& !m_nodesIds.empty() )
		{
			{
				C3D_DebugTime( renderPass.getTypeName() + " - Overall" );
				auto maxNodesCount = m_pipelinesNodes->getCount();
				auto nodesIdsBuffer = m_pipelinesNodes->lock();

				if ( !m_submeshNodes.empty()
					|| !m_instancedSubmeshNodes.empty() )
				{
					uint32_t idxIndex{};
					uint32_t nidxIndex{};
					uint32_t mshIndex{};

					auto const & submeshIdxCommands = *m_submeshIdxIndirectCommands;
					auto origIndirectIdxBuffer = submeshIdxCommands.lock();
					auto indirectIdxBuffer = origIndirectIdxBuffer;
					auto const & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
					auto origIndirectNIdxBuffer = submeshNIdxCommands.lock();
					auto indirectNIdxBuffer = origIndirectNIdxBuffer;
#if VK_EXT_mesh_shader
					auto origIndirectMshBufferEXT = ( renderPass.isMeshShading() && m_submeshMeshletIndirectCommandsEXT )
						? m_submeshMeshletIndirectCommandsEXT->lock()
						: nullptr;
					auto indirectMshBufferEXT = origIndirectMshBufferEXT;
#endif
#if VK_NV_mesh_shader
					auto origIndirectMshBufferNV = ( renderPass.isMeshShading() && m_submeshMeshletIndirectCommandsNV )
						? m_submeshMeshletIndirectCommandsNV->lock()
						: nullptr;
					auto indirectMshBufferNV = origIndirectMshBufferNV;
#endif
					{
						C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - SingleSubmeshes" );
						for ( auto const & [_, pipelinesNodes] : m_submeshNodes )
						{
							auto const & pipeline = *pipelinesNodes.pipeline.pipeline;

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
							if ( renderPass.isMeshShading()
								&& pipeline.hasMeshletDescriptorSetLayout() )
							{
#	if defined( VK_EXT_mesh_shader ) && defined( VK_NV_mesh_shader )
								if ( renderPass.getEngine()->getRenderDevice()->prefersMeshShaderEXT() )
								{
									result += doParseSimpleSubmeshesMeshletsEXT( commandBuffer, viewport, scissors
										, nodesIdsBuffer, maxNodesCount
										, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
										, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
										, *m_submeshMeshletIndirectCommandsEXT, origIndirectMshBufferEXT, indirectMshBufferEXT
										, pipelinesNodes.nodes, pipeline, mshIndex );
								}
								else
								{
									result += doParseSimpleSubmeshesMeshletsNV( commandBuffer, viewport, scissors
										, nodesIdsBuffer, maxNodesCount
										, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
										, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
										, *m_submeshMeshletIndirectCommandsNV, origIndirectMshBufferNV, indirectMshBufferNV
										, pipelinesNodes.nodes, pipeline, mshIndex );
								}
#	elif defined( VK_EXT_mesh_shader )
								result += doParseSimpleSubmeshesMeshletsEXT( commandBuffer, viewport, scissors
									, nodesIdsBuffer, maxNodesCount
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, *m_submeshMeshletIndirectCommandsEXT, origIndirectMshBufferEXT, indirectMshBufferEXT
									, pipelinesNodes.nodes, pipeline, mshIndex );
#	else
								result += doParseSimpleSubmeshesMeshletsNV( commandBuffer, viewport, scissors
									, nodesIdsBuffer, maxNodesCount
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, *m_submeshMeshletIndirectCommandsNV, origIndirectMshBufferNV, indirectMshBufferNV
									, pipelinesNodes.nodes, pipeline, mshIndex );
#	endif
							}
							else
#endif
							if ( getOwner()->getOwner()->getEngine()->getRenderDevice()->hasDrawId() )
							{
								result += doParseSimpleSubmeshesIndirect( commandBuffer, viewport, scissors
									, nodesIdsBuffer, maxNodesCount
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, pipelinesNodes.nodes, pipeline, idxIndex, nidxIndex );
							}
							else
							{
								result += doParseSimpleSubmeshesDirect( commandBuffer, viewport, scissors
									, nodesIdsBuffer, maxNodesCount
									, pipelinesNodes.nodes, pipeline, idxIndex, nidxIndex );
							}
						}
					}
					{
						C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - InstantiatedSubmeshes" );
						for ( auto const & [_, pipelinesNodes] : m_instancedSubmeshNodes )
						{
							auto const & pipeline = *pipelinesNodes.pipeline.pipeline;

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
							if ( renderPass.isMeshShading()
								&& pipeline.hasMeshletDescriptorSetLayout() )
							{
#	if defined( VK_EXT_mesh_shader ) && defined( VK_NV_mesh_shader )
								if ( renderPass.getEngine()->getRenderDevice()->prefersMeshShaderEXT() )
								{
									result += doParseInstantiatedSubmeshesMeshletsEXT( commandBuffer, viewport, scissors
										, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
										, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
										, *m_submeshMeshletIndirectCommandsEXT, origIndirectMshBufferEXT, indirectMshBufferEXT
										, pipelinesNodes.nodes, pipeline, mshIndex );
								}
								else
								{
									result += doParseInstantiatedSubmeshesMeshletsNV( commandBuffer, viewport, scissors
										, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
										, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
										, *m_submeshMeshletIndirectCommandsNV, origIndirectMshBufferNV, indirectMshBufferNV
										, pipelinesNodes.nodes, pipeline, mshIndex );
								}
#	elif defined( VK_EXT_mesh_shader )
								result += doParseInstantiatedSubmeshesMeshletsEXT( commandBuffer, viewport, scissors
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, *m_submeshMeshletIndirectCommandsEXT, origIndirectMshBufferEXT, indirectMshBufferEXT
									, pipelinesNodes.nodes, pipeline, mshIndex );
#	else
								result += doParseInstantiatedSubmeshesMeshletsNV( commandBuffer, viewport, scissors
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, *m_submeshMeshletIndirectCommandsNV, origIndirectMshBufferNV, indirectMshBufferNV
									, pipelinesNodes.nodes, pipeline, mshIndex );
#	endif
							}
							else
#endif
							if ( getOwner()->getOwner()->getEngine()->getRenderDevice()->hasDrawId() )
							{
								result += doParseInstantiatedSubmeshesIndirect( commandBuffer, viewport, scissors
									, submeshIdxCommands, origIndirectIdxBuffer, indirectIdxBuffer
									, submeshNIdxCommands, origIndirectNIdxBuffer, indirectNIdxBuffer
									, pipelinesNodes.nodes, pipeline, idxIndex, nidxIndex );
							}
							else
							{
								result += doParseInstantiatedSubmeshesDirect( commandBuffer, viewport, scissors
									, pipelinesNodes.nodes, pipeline, idxIndex, nidxIndex );
							}
						}
					}

#if VK_NV_mesh_shader
					if ( origIndirectMshBufferNV )
					{
						m_submeshMeshletIndirectCommandsNV->flush();
						m_submeshMeshletIndirectCommandsNV->unlock();
					}
#endif
#if VK_EXT_mesh_shader
					if ( origIndirectMshBufferEXT )
					{
						m_submeshMeshletIndirectCommandsEXT->flush();
						m_submeshMeshletIndirectCommandsEXT->unlock();
					}
#endif
					submeshIdxCommands.flush();
					submeshIdxCommands.unlock();
					submeshNIdxCommands.flush();
					submeshNIdxCommands.unlock();
				}

				if ( !m_billboardNodes.empty() )
				{
					C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Billboards" );
					uint32_t idxIndex{};
					uint32_t nidxIndex{};

					auto const & billboardCommands = *m_billboardIndirectCommands;
					auto origIndirectBuffer = billboardCommands.lock();
					auto indirectBuffer = origIndirectBuffer;

					for ( auto const & [_, pipelinesNodes] : m_billboardNodes )
					{
						auto const & pipeline = pipelinesNodes.pipeline;

						if ( renderPass.getEngine()->getRenderDevice()->hasDrawId() )
						{
							result += doParseBillboardsIndirect( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount
								, billboardCommands, origIndirectBuffer, indirectBuffer
								, pipelinesNodes.nodes, *pipeline.pipeline, idxIndex, nidxIndex );
						}
						else
						{
							result += doParseBillboardsDirect( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount
								, pipelinesNodes.nodes, *pipeline.pipeline, nidxIndex );
						}
					}

					billboardCommands.flush();
					billboardCommands.unlock();
				}

				m_pipelinesNodes->flush();
				m_pipelinesNodes->unlock();
			}

			if constexpr ( queuerndnd::C3D_PrintNodesCounts )
			{
				log::debug << renderPass.getName() << ":\n";
				log::debug << m_submeshNodes << "\n";
				log::debug << m_instancedSubmeshNodes << "\n";
				log::debug << m_billboardNodes << "\n";
			}
		}

		commandBuffer.end();
		return result;
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

	uint32_t QueueRenderNodes::getMaxPipelineId()const
	{
		return m_maxPipelineId;
	}

	void QueueRenderNodes::registerPipelineId( uint32_t pipelineId )
	{
		m_maxPipelineId = std::max( m_maxPipelineId, pipelineId );
	}

	PipelineBufferArray const & QueueRenderNodes::getPassPipelineNodes()const
	{
		return m_nodesIds;
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( PipelineBaseHash const & hash
		, BufferBase const & posBuffer
		, BufferBase const * idxBuffer )const
	{
		return getPipelineNodeIndex( hash
			, posBuffer
			, idxBuffer
			, getPassPipelineNodes() );
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( Submesh const & submesh
		, Pass const & pass
		, BufferBase const & posBuffer
		, BufferBase const * idxBuffer
		, bool isFrontCulled )const
	{
		auto const & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, submesh, pass, isFrontCulled )
			, posBuffer
			, idxBuffer );
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( BillboardBase const & billboard
		, Pass const & pass
		, BufferBase const & posBuffer
		, BufferBase const * idxBuffer
		, bool isFrontCulled )const
	{
		auto const & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, billboard, pass, isFrontCulled )
			, posBuffer
			, idxBuffer );
	}

	PipelineAndID QueueRenderNodes::doGetPipeline( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, SubmeshRenderNode const & node
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto [hash, it, pipelineFlags] = queuerndnd::getPipeline( renderPass
			, node
			, frontCulled
			, m_pipelines
			, 0u );

		if ( pipelineFlags.usesMesh() )
		{
			node.createMeshletDescriptorSet();
		}
		else
		{
			node.createVertexPullingDescriptorSet();
		}

		if ( it == m_pipelines.end() )
		{
			auto result = frontCulled
				? renderPass.prepareFrontPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
					, node.getVertexPullingDescriptorLayout()
					, node.getMeshletDescriptorLayout() )
				: renderPass.prepareBackPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
					, node.getVertexPullingDescriptorLayout()
					, node.getMeshletDescriptorLayout() );
			it = m_pipelines.try_emplace( hash, result ).first;
			renderPass.initialiseAdditionalDescriptor( *result.pipeline
				, shadowMaps
				, shadowBuffer );
		}

		return it->second;
	}

	PipelineAndID QueueRenderNodes::doGetPipeline( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, BillboardRenderNode const & node )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto [hash, it, pipelineFlags] = queuerndnd::getPipeline( renderPass
			, node
			, false
			, m_pipelines
			, node.data.getVertexStride() );

		if ( it == m_pipelines.end() )
		{
			auto result = renderPass.prepareBackPipeline( pipelineFlags
				, node.getGeometryBuffers( pipelineFlags ).layouts
				, node.getVertexPullingDescriptorLayout()
				, nullptr );
			it = m_pipelines.try_emplace( hash, result ).first;
			renderPass.initialiseAdditionalDescriptor( *result.pipeline
				, shadowMaps
				, shadowBuffer );
		}

		return it->second;
	}

	void QueueRenderNodes::doAddSubmesh( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted )
	{
		auto const & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto passFlags = node.pass->getPassFlags();
		bool needsFront = !node.pass->hasComponent< AttenuationComponent >()
			&& ( ( !checkFlag( renderPass.getRenderFilters(), RenderFilter::eAlphaBlend ) )
				|| ( passFlags.hasTransmissionFlag && !checkFlag( renderPass.getRenderFilters(), RenderFilter::eTransmission ) )
				|| renderPass.forceTwoSided()
				|| node.pass->isTwoSided()
				|| passFlags.hasAlphaBlendingFlag );

		if ( node.isInstanced() )
		{
			if ( node.instance.getParent()->isVisible()
				&& node.pass->isVisible() )
			{
				doAddInstancedSubmesh( shadowMaps
					, shadowBuffer
					, counted
					, false );

				if ( needsFront )
				{
					doAddInstancedSubmesh( shadowMaps
						, shadowBuffer
						, counted
						, true );
				}
			}
		}
		else
		{
			doAddSingleSubmesh( shadowMaps
				, shadowBuffer
				, counted
				, false );

			if ( needsFront )
			{
				doAddSingleSubmesh( shadowMaps
					, shadowBuffer
					, counted
					, true );
			}
		}
	}

	void QueueRenderNodes::doAddBillboard( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< BillboardRenderNode > const & counted )
	{
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node );
		m_hasNodes = true;

		if ( !queuerndnd::addRenderNode( pipelineId
			, counted
			, false
			, m_billboardNodes
			, m_nodesIds ) )
		{
			m_pendingBillboards.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddSingleSubmesh( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );
		m_hasNodes = true;

		if ( !queuerndnd::addRenderNode( pipelineId
			, counted
			, frontCulled
			, m_submeshNodes
			, m_nodesIds ) )
		{
			m_pendingSubmeshes.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddInstancedSubmesh( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );
		m_hasNodes = true;

		if ( !queuerndnd::addRenderNode( pipelineId
			, counted
			, frontCulled
			, m_instancedSubmeshNodes
			, m_nodesIds ) )
		{
			m_pendingSubmeshes.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddSubmesh( CulledNodeT< SubmeshRenderNode > const & node )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

		if ( renderPass.isValidPass( *node.node->pass )
			&& renderPass.isValidRenderable( node.node->instance )
			&& renderPass.isValidNode( *node.node->instance.getParent() ) )
		{
			m_pendingSubmeshes.insert( &node );
		}
	}

	void QueueRenderNodes::doAddBillboard( CulledNodeT< BillboardRenderNode > const & node )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

		if ( renderPass.isValidPass( *node.node->pass )
			&& renderPass.isValidRenderable( node.node->instance )
			&& renderPass.isValidNode( *node.node->instance.getNode() ) )
		{
			m_pendingBillboards.insert( &node );
		}
	}

	void QueueRenderNodes::doRemoveSubmesh( CulledNodeT< SubmeshRenderNode > const & node )
	{
		auto it = m_pendingSubmeshes.find( &node );

		if ( it != m_pendingSubmeshes.end() )
		{
			m_pendingSubmeshes.erase( it );
		}
	}

	void QueueRenderNodes::doRemoveBillboard( CulledNodeT< BillboardRenderNode > const & node )
	{
		auto it = m_pendingBillboards.find( &node );

		if ( it != m_pendingBillboards.end() )
		{
			m_pendingBillboards.erase( it );
		}
	}

	uint32_t QueueRenderNodes::doParseSimpleSubmeshesDirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & idxIndex
		, uint32_t & nidxIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();
				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, false );
				uint32_t visibleNodesCount{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::registerDirectNodeCommands( pipeline
							, node
							, commandBuffer
							, instanceCount
							, pipelineId
							, visibleNodesCount
							, pipelinesBuffer
							, idxIndex
							, nidxIndex );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
						m_visible.objectCount += instanceCount;
						m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
						m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
						++visibleNodesCount;
					}
				}

				++result;
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseInstantiatedSubmeshesDirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & idxIndex
		, uint32_t & nidxIndex )
	{
		uint32_t result{};
		uint32_t * pipelinesBuffer = nullptr;

		for ( auto const & [posBuffer, idxBuffer, submeshes] : buffersNodes )
		{
			auto pipelineId = queuerndnd::bindPipeline( commandBuffer
				, *this
				, pipeline
				, *posBuffer
				, idxBuffer
				, viewport
				, scissors
				, false );

			for ( auto const & [submesh, node] : submeshes )
			{
				if ( queuerndnd::hasVisibleInstance( node.second ) )
				{
					auto instanceCount = node.first.node->getInstanceCount();
					queuerndnd::registerDirectNodeCommands( pipeline
						, node.first
						, commandBuffer
						, instanceCount
						, pipelineId
						, 0u
						, pipelinesBuffer
						, idxIndex
						, nidxIndex );
					m_visible.objectCount += instanceCount;
					m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
					m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
					++result;
				}
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseSimpleSubmeshesIndirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & idxIndex
		, uint32_t & nidxIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();
				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, true );
				uint32_t visibleNodesCount{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::fillNodeIndirectCommands( node
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, instanceCount
							, pipelinesBuffer );
						m_visible.objectCount += instanceCount;
						m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
						m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
						CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
						++visibleNodesCount;
					}
				}

				queuerndnd::registerIndirectNodeCommands( pipeline
					, *firstVisibleNode
					, commandBuffer
					, &submeshIdxCommands
					, submeshNIdxCommands
					, pipelineId
					, 0u
					, visibleNodesCount
					, idxIndex
					, nidxIndex );
				++result;
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseInstantiatedSubmeshesIndirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & idxIndex
		, uint32_t & nidxIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, submeshes] : buffersNodes )
		{
			auto pipelineId = queuerndnd::bindPipeline( commandBuffer
				, *this
				, pipeline
				, *posBuffer
				, idxBuffer
				, viewport
				, scissors
				, true );

			for ( auto const & [submesh, node] : submeshes )
			{
				if ( queuerndnd::hasVisibleInstance( node.second ) )
				{
					auto instanceCount = node.first.node->getInstanceCount();
					queuerndnd::fillNodeIndirectCommands( node.first
						, indirectIdxBuffer
						, indirectNIdxBuffer
						, instanceCount );
					m_visible.objectCount += instanceCount;
					m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
					m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
					CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
					CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
					queuerndnd::registerIndirectNodeCommands( pipeline
						, node.first
						, commandBuffer
						, &submeshIdxCommands
						, submeshNIdxCommands
						, pipelineId
						, 0u
						, 1u
						, idxIndex
						, nidxIndex );
					++result;
				}
			}
		}

		return result;
	}

#if VK_EXT_mesh_shader

	uint32_t QueueRenderNodes::doParseSimpleSubmeshesMeshletsEXT( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, BufferT< VkDrawMeshTasksIndirectCommandEXT > const & submeshMshCommands
		, VkDrawMeshTasksIndirectCommandEXT * origIndirectMshBuffer
		, VkDrawMeshTasksIndirectCommandEXT *& indirectMshBuffer
		, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & mshIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();

				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, true );
				uint32_t drawOffset{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::fillNodeIndirectCommands( node
							, indirectMshBuffer
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, instanceCount
							, pipelinesBuffer );
						m_visible.objectCount += instanceCount;
						m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
						m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
						CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= submeshMshCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );

						queuerndnd::registerMeshletNodeCommands( pipeline
							, *node.node
							, commandBuffer
							, submeshMshCommands
							, pipelineId
							, drawOffset
							, instanceCount
							, mshIndex );
						drawOffset += instanceCount;
						++result;
					}
				}
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseInstantiatedSubmeshesMeshletsEXT( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, BufferT< VkDrawMeshTasksIndirectCommandEXT > const & submeshMshCommands
		, VkDrawMeshTasksIndirectCommandEXT * origIndirectMshBuffer
		, VkDrawMeshTasksIndirectCommandEXT *& indirectMshBuffer
		, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & mshIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, submeshes] : buffersNodes )
		{
			auto pipelineId = queuerndnd::bindPipeline( commandBuffer
				, *this
				, pipeline
				, *posBuffer
				, idxBuffer
				, viewport
				, scissors
				, true );

			for ( auto const & [submesh, node] : submeshes )
			{
				if ( queuerndnd::hasVisibleInstance( node.second ) )
				{
					auto instanceCount = node.first.node->getInstanceCount();
					queuerndnd::fillNodeIndirectCommands( node.first
						, indirectMshBuffer
						, indirectIdxBuffer
						, indirectNIdxBuffer
						, instanceCount );
					m_visible.objectCount += instanceCount;
					m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
					m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
					CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= submeshMshCommands.getCount() );
					CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
					CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );

					queuerndnd::registerMeshletNodeCommands( pipeline
						, *node.first.node
						, commandBuffer
						, submeshMshCommands
						, pipelineId
						, 0u
						, instanceCount
						, mshIndex );
					++result;
				}
			}
		}

		return result;
	}

#endif
#if VK_NV_mesh_shader

	uint32_t QueueRenderNodes::doParseSimpleSubmeshesMeshletsNV( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, BufferT< VkDrawMeshTasksIndirectCommandNV > const & submeshMshCommands
		, VkDrawMeshTasksIndirectCommandNV * origIndirectMshBuffer
		, VkDrawMeshTasksIndirectCommandNV *& indirectMshBuffer
		, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & mshIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();
				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, true );
				uint32_t drawOffset{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::fillNodeMeshletCommands( node
							, indirectMshBuffer
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, instanceCount
							, pipelinesBuffer );
						m_visible.objectCount += instanceCount;
						m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
						m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
						CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= submeshMshCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );

						queuerndnd::registerMeshletNodeCommands( pipeline
							, *node.node
							, commandBuffer
							, submeshMshCommands
							, pipelineId
							, drawOffset
							, instanceCount
							, mshIndex );
						drawOffset += instanceCount;
						++result;
					}
				}
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseInstantiatedSubmeshesMeshletsNV( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, BufferT< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
		, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
		, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
		, BufferT< VkDrawIndirectCommand > const & submeshNIdxCommands
		, VkDrawIndirectCommand * origIndirectNIdxBuffer
		, VkDrawIndirectCommand *& indirectNIdxBuffer
		, BufferT< VkDrawMeshTasksIndirectCommandNV > const & submeshMshCommands
		, VkDrawMeshTasksIndirectCommandNV * origIndirectMshBuffer
		, VkDrawMeshTasksIndirectCommandNV *& indirectMshBuffer
		, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & mshIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, submeshes] : buffersNodes )
		{
			auto pipelineId = queuerndnd::bindPipeline( commandBuffer
				, *this
				, pipeline
				, *posBuffer
				, idxBuffer
				, viewport
				, scissors
				, true );

			for ( auto const & [submesh, node] : submeshes )
			{
				if ( queuerndnd::hasVisibleInstance( node.second ) )
				{
					auto instanceCount = node.first.node->getInstanceCount();
					queuerndnd::fillNodeMeshletCommands( node.first
						, indirectMshBuffer
						, indirectIdxBuffer
						, indirectNIdxBuffer
						, instanceCount );
					m_visible.objectCount += instanceCount;
					m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
					m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
					CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= submeshMshCommands.getCount() );
					CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
					CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );

					queuerndnd::registerMeshletNodeCommands( pipeline
						, *node.first.node
						, commandBuffer
						, submeshMshCommands
						, pipelineId
						, 0u
						, instanceCount
						, mshIndex );
					++result;
				}
			}
		}

		return result;
	}

#endif

	uint32_t QueueRenderNodes::doParseBillboardsDirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BuffersNodesViewT< BillboardRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & nidxIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();
				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, true );
				uint32_t visibleNodesCount{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::registerDirectNodeCommands( pipeline
							, node
							, commandBuffer
							, instanceCount
							, pipelineId
							, visibleNodesCount
							, pipelinesBuffer
							, nidxIndex );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
						m_visible.billboardCount += node.node->data.getCount();
						++visibleNodesCount;
					}
				}

				++result;
			}
		}

		return result;
	}

	uint32_t QueueRenderNodes::doParseBillboardsIndirect( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount
		, BufferT< VkDrawIndirectCommand > const & billboardCommands
		, VkDrawIndirectCommand * origIndirectBuffer
		, VkDrawIndirectCommand *& indirectBuffer
		, BuffersNodesViewT< BillboardRenderNode > const & buffersNodes
		, RenderPipeline const & pipeline
		, uint32_t & idxIndex
		, uint32_t & nidxIndex )
	{
		uint32_t result{};

		for ( auto const & [posBuffer, idxBuffer, nodes] : buffersNodes )
		{
			if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
			{
				auto & pipelineNodes = getPipelineNodes( pipeline.getFlagsHash()
					, *posBuffer
					, idxBuffer
					, m_nodesIds
					, nodesIdsBuffer
					, maxNodesCount );
				auto pipelinesBuffer = pipelineNodes.data();
				auto pipelineId = queuerndnd::bindPipeline( commandBuffer
					, *this
					, pipeline
					, *posBuffer
					, idxBuffer
					, viewport
					, scissors
					, true );
				uint32_t visibleNodesCount{};

				for ( auto const & node : nodes )
				{
					if ( node.visible )
					{
						auto instanceCount = node.node->getInstanceCount();
						queuerndnd::fillNodeIndirectCommand( node
							, indirectBuffer
							, instanceCount
							, pipelinesBuffer );
						m_visible.billboardCount += node.node->data.getCount();
						CU_Require( size_t( std::distance( origIndirectBuffer, indirectBuffer ) ) <= billboardCommands.getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
						++visibleNodesCount;
					}
				}

				queuerndnd::registerIndirectNodeCommands( pipeline
					, *firstVisibleNode
					, commandBuffer
					, nullptr
					, billboardCommands
					, pipelineId
					, 0u
					, visibleNodesCount
					, idxIndex
					, nidxIndex );
				++result;
			}
		}

		return result;
	}

	//*********************************************************************************************
}
