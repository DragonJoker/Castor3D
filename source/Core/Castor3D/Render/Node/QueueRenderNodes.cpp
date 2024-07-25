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

CU_ImplementSmartPtr( castor3d, QueueRenderNodes )

using ashes::operator==;
using ashes::operator!=;

#pragma warning( disable:4189 )

namespace castor3d
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
				, .vertexOffset = int32_t( bufferChunk.getFirst< castor::Point4f >() )
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
				auto & buffer = bufferChunk.buffer->getBuffer();

				if constexpr ( std::is_same_v< BillboardRenderNode, NodeT > )
				{
					nodes.emplace( pipeline
						, buffer
						, culled
						, getCommand( node.getSourceBufferOffsets(), bufferChunk, culled )
						, isFrontCulled );
				}
				else
				{
					nodes.emplace( pipeline
						, buffer
						, culled
						, getCommand( node.getSourceBufferOffsets(), bufferChunk, culled )
						, isFrontCulled );
				}

				registerPipelineNodes( pipeline.pipeline->getFlagsHash(), buffer, nodesIds );
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

		static bool hasVisibleInstance( castor::UnorderedSet< Geometry const * > const & instances )
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
			, ashes::BufferBase const & buffer
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

				pipelineId = queueNodes.getPipelineNodesIndex( pipeline.getFlagsHash()
					, buffer );

				if ( hasDrawId
					&& !pipeline.hasMeshletDescriptorSetLayout() )
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
		static void registerNodeCommands( RenderPipeline const & pipeline
			, RenderedNodeT< NodeT > const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const * indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t drawCount
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			auto & geometryBuffers = node.node->getGeometryBuffers( pipeline.getFlags() );
			commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
				, geometryBuffers.buffers
				, geometryBuffers.offsets );

			if ( geometryBuffers.indexOffset.hasData() && indirectIndexedCommands )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.indexOffset.getBuffer()
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

		static void registerNodeCommands( RenderPipeline const & pipeline
			, RenderedNodeT< SubmeshRenderNode > const & node
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t instanceCount
			, uint32_t pipelineId
			, uint32_t drawId
			, uint32_t *& pipelinesBuffer
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			if ( pipelinesBuffer )
			{
				( *pipelinesBuffer ) = node.node->getId();
				++pipelinesBuffer;
			}

			auto & geometryBuffers = node.node->getGeometryBuffers( pipeline.getFlags() );
			commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
				, geometryBuffers.buffers
				, geometryBuffers.offsets );
			DrawConstants constants{ pipelineId, int32_t( drawId ) };
			commandBuffer.pushConstants( pipeline.getPipelineLayout()
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( DrawConstants )
				, &constants );

			if ( geometryBuffers.indexOffset.hasData() )
			{
				commandBuffer.bindIndexBuffer( geometryBuffers.indexOffset.getBuffer()
					, 0u
					, VK_INDEX_TYPE_UINT32 );
				commandBuffer.drawIndexed( node.command.indexCount
					, instanceCount
					, geometryBuffers.indexOffset.getFirst< uint32_t >()
					, node.node->getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions ).getFirst< castor::Point4f >()
					, 0u );
				++idxIndex;
			}
			else
			{
				commandBuffer.draw( node.command.indexCount
					, instanceCount
					, node.node->getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions ).getFirst< castor::Point4f >()
					, 0u );
				++nidxIndex;
			}
		}

		static void registerNodeCommands( RenderPipeline const & pipeline
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
			commandBuffer.bindVertexBuffers( geometryBuffers.layouts[0].get().vertexBindingDescriptions[0].binding
				, geometryBuffers.buffers
				, geometryBuffers.offsets );
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

		static void registerNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
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
			commandBuffer.drawMeshTasksIndirectNV( indirectMeshCommands.getBuffer()
				, mshIndex * sizeof( VkDrawMeshTasksIndirectCommandNV )
				, drawCount
				, sizeof( VkDrawMeshTasksIndirectCommandNV ) );
			mshIndex += drawCount;
		}

#endif
#if VK_EXT_mesh_shader

		static void registerNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandEXT > const & indirectMeshCommands
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
			commandBuffer.drawMeshTasksIndirect( indirectMeshCommands.getBuffer()
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
			indirectCommands->firstVertex = uint32_t( node.command.vertexOffset );
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

		static void fillNodeIndirectCommand( RenderedNodeT< SubmeshRenderNode > const &
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

		static void fillNodeIndirectCommands( RenderedNodeT< SubmeshRenderNode > const & node
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
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
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
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
			hash = castor::hashCombine( hash, pass.getHash() );
			hash = castor::hashCombine( hash, node.isInstanced() );
			hash = castor::hashCombine( hash, frontCulled );
			return hash;
		}

		static size_t makeHash( BillboardRenderNode const & node
			, bool )
		{
			auto const & billboard = node.data;
			auto const & pass = *node.pass;
			size_t hash = std::hash< BillboardBase const * >{}( &billboard );
			hash = castor::hashCombine( hash, pass.getHash() );
			return hash;
		}

		template< typename NodeT >
		std::tuple< size_t, QueueRenderNodes::PipelineMap::iterator, PipelineFlags > getPipeline( RenderNodesPass const & renderPass
			, NodeT const & node
			, bool frontCulled
			, QueueRenderNodes::PipelineMap & pipelines )
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
				, node.getRenderData() );

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
	inline castor::String getNodesTypeName();

	template<>
	inline castor::String getNodesTypeName< SubmeshRenderNode >()
	{
		return cuT( "Submeshes" );
	}

	template<>
	inline castor::String getNodesTypeName< BillboardRenderNode >()
	{
		return cuT( "Billboards" );
	}

	template< typename NodeT >
	static castor::OutputStream & operator<<( castor::OutputStream & stream, PipelinesNodesT< NodeT > const & rhs )
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
	static castor::OutputStream & operator<<( castor::OutputStream & stream, InstantiatedPipelinesNodesT< NodeT > const & rhs )
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
		, castor::String const & typeName
		, bool meshShading )
		: castor::OwnedBy< RenderQueue const >{ queue }
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
					, MaxSubmeshMeshletDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
			}
			else
			{
				m_submeshMeshletIndirectCommandsNV = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
					, MaxSubmeshMeshletDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
			}
#	elif VK_EXT_mesh_shader
			m_submeshMeshletIndirectCommandsEXT = makeBuffer< VkDrawMeshTasksIndirectCommandEXT >( device
				, MaxSubmeshMeshletDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
#	else
			m_submeshMeshletIndirectCommandsNV = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
				, MaxSubmeshMeshletDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, typeName + cuT( "/SubmeshMeshletIndirectBuffer" ) );
#	endif
		}

#endif
		m_submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
			, MaxSubmeshIdxDrawIndirectCommand
			, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, typeName + cuT( "/SubmeshIndexedIndirectBuffer" ) );
		m_submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
			, MaxSubmeshNIdxDrawIndirectCommand
			, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, typeName + cuT( "/SubmeshIndirectBuffer" ) );
		m_billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
			, MaxBillboardDrawIndirectCommand
			, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, typeName + cuT( "/BillboardIndirectBuffer" ) );
		m_pipelinesNodes = makeBuffer< PipelineNodes >( device
			, MaxPipelinesNodes
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, typeName + cuT( "/NodesIDs" ) );
	}

	QueueRenderNodes::~QueueRenderNodes()noexcept
	{
		m_pipelinesNodes.reset();
		m_billboardIndirectCommands.reset();
		m_submeshNIdxIndirectCommands.reset();
		m_submeshIdxIndirectCommands.reset();
#if VK_EXT_mesh_shader
		m_submeshMeshletIndirectCommandsEXT.reset();
#endif
#if VK_NV_mesh_shader
		m_submeshMeshletIndirectCommandsNV.reset();
#endif
	}

	void QueueRenderNodes::fillConfig( crg::ru::Config & config )const
	{
		config.prePassAction( [this]( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t )
			{
				context.memoryBarrier( commandBuffer
					, m_pipelinesNodes->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_pipelinesNodes->getBuffer().getSize() }
					, VK_ACCESS_HOST_WRITE_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
				context.memoryBarrier( commandBuffer
					, m_submeshIdxIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_submeshIdxIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_HOST_WRITE_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
				context.memoryBarrier( commandBuffer
					, m_submeshNIdxIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_submeshNIdxIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_HOST_WRITE_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
				context.memoryBarrier( commandBuffer
					, m_billboardIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_billboardIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_HOST_WRITE_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );

				if ( m_submeshMeshletIndirectCommandsEXT )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsEXT->getBuffer()
						, crg::BufferSubresourceRange{ 0u, m_submeshMeshletIndirectCommandsEXT->getBuffer().getSize() }
						, VK_ACCESS_HOST_WRITE_BIT
						, VK_PIPELINE_STAGE_HOST_BIT
						, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
				}

				if ( m_submeshMeshletIndirectCommandsNV )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsNV->getBuffer()
						, crg::BufferSubresourceRange{ 0u, m_submeshMeshletIndirectCommandsNV->getBuffer().getSize() }
						, VK_ACCESS_HOST_WRITE_BIT
						, VK_PIPELINE_STAGE_HOST_BIT
						, { VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
				}
			} );
		config.postPassAction( [this]( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t )
			{
				context.memoryBarrier( commandBuffer
					, m_pipelinesNodes->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_pipelinesNodes->getBuffer().getSize() }
					, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
					, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
				context.memoryBarrier( commandBuffer
					, m_submeshIdxIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_submeshIdxIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
					, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
				context.memoryBarrier( commandBuffer
					, m_submeshNIdxIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_submeshNIdxIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
					, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
				context.memoryBarrier( commandBuffer
					, m_billboardIndirectCommands->getBuffer()
					, crg::BufferSubresourceRange{ 0u, m_billboardIndirectCommands->getBuffer().getSize() }
					, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
					, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );

				if ( m_submeshMeshletIndirectCommandsEXT )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsEXT->getBuffer()
						, crg::BufferSubresourceRange{ 0u, m_submeshMeshletIndirectCommandsEXT->getBuffer().getSize() }
						, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
						, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
				}

				if ( m_submeshMeshletIndirectCommandsNV )
				{
					context.memoryBarrier( commandBuffer
						, m_submeshMeshletIndirectCommandsNV->getBuffer()
						, crg::BufferSubresourceRange{ 0u, m_submeshMeshletIndirectCommandsNV->getBuffer().getSize() }
						, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
						, { VK_ACCESS_HOST_WRITE_BIT, VK_PIPELINE_STAGE_HOST_BIT } );
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
			auto pendingSubmeshes = castor::move( m_pendingSubmeshes );
			auto pendingBillboards = castor::move( m_pendingBillboards );

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
				auto nodesIdsBuffer = m_pipelinesNodes->lock( 0u, ashes::WholeSize, 0u );

				if ( !m_submeshNodes.empty()
					|| !m_instancedSubmeshNodes.empty() )
				{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
					if ( renderPass.isMeshShading() )
					{
#	if defined( VK_EXT_mesh_shader ) && defined( VK_NV_mesh_shader )
						if ( renderPass.getEngine()->getRenderDevice()->prefersMeshShaderEXT() )
						{
							result += doPrepareMeshModernCommandBuffersEXT( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
						}
						else
						{
							result += doPrepareMeshModernCommandBuffersNV( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
						}
#	elif defined( VK_EXT_mesh_shader )
						result += doPrepareMeshModernCommandBuffersEXT( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
#	else
						result += doPrepareMeshModernCommandBuffersNV( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
#	endif
					}
					else
#endif
					if ( renderPass.getEngine()->getRenderDevice()->hasDrawId() )
					{
						result += doPrepareMeshTraditionalCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
					}
					else 
					{
						result += doPrepareMeshTraditionalNoDrawIDCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
					}
				}

				if ( !m_billboardNodes.empty() )
				{
					if ( renderPass.getEngine()->getRenderDevice()->hasDrawId() )
					{
						result += doPrepareBillboardCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
					}
					else
					{
						result += doPrepareBillboardNoDrawIDCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
					}
				}

				m_pipelinesNodes->flush( 0u, ashes::WholeSize );
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
		, ashes::BufferBase const & buffer )const
	{
		return getPipelineNodeIndex( hash
			, buffer
			, getPassPipelineNodes() );
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( Submesh const & submesh
		, Pass const & pass
		, ashes::BufferBase const & buffer
		, bool isFrontCulled )const
	{
		auto const & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, submesh, pass, isFrontCulled )
			, buffer );
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( BillboardBase const & billboard
		, Pass const & pass
		, ashes::BufferBase const & buffer
		, bool isFrontCulled )const
	{
		auto const & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, billboard, pass, isFrontCulled )
			, buffer );
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
			, m_pipelines );

		if ( pipelineFlags.usesMesh() )
		{
			node.createMeshletDescriptorSet();
		}

		if ( it == m_pipelines.end() )
		{
			auto result = frontCulled
				? renderPass.prepareFrontPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
					, node.getMeshletDescriptorLayout() )
				: renderPass.prepareBackPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
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
			, m_pipelines );

		if ( it == m_pipelines.end() )
		{
			auto result = renderPass.prepareBackPipeline( pipelineFlags
				, node.getGeometryBuffers( pipelineFlags ).layouts
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
			if ( node.instance.getParent()->isVisible() )
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

	uint32_t QueueRenderNodes::doPrepareMeshTraditionalNoDrawIDCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Single" );
			for ( auto const & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
				{
					if ( queuerndnd::hasVisibleNode( nodes ) )
					{
						auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
							, *buffer
							, m_nodesIds
							, nodesIdsBuffer
							, maxNodesCount );
						auto pipelinesBuffer = pipelineNodes.data();
						auto pipelineId = queuerndnd::bindPipeline( commandBuffer
							, *this
							, *pipeline.pipeline
							, *buffer
							, viewport
							, scissors
							, false );
						uint32_t visibleNodesCount{};

						for ( auto const & node : nodes )
						{
							if ( node.visible )
							{
								auto instanceCount = node.node->getInstanceCount();
								queuerndnd::registerNodeCommands( *pipeline.pipeline
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
			}
		}
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Instantiated" );
			for ( auto const & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;
				auto const & buffers = pipelinesNodes.nodes;
				uint32_t * pipelinesBuffer = nullptr;

				for ( auto const & [buffer, submeshes] : buffers )
				{
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipeline.pipeline
						, *buffer
						, viewport
						, scissors
						, false );

					for ( auto const & [submesh, node] : submeshes )
					{
						if ( queuerndnd::hasVisibleInstance( node.second ) )
						{
							auto instanceCount = node.first.node->getInstanceCount();
							queuerndnd::registerNodeCommands( *pipeline.pipeline
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
			}
		}
		return result;
	}

	uint32_t QueueRenderNodes::doPrepareMeshTraditionalCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};

		auto const & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto const & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		auto origIndirectIdxBuffer = submeshIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectNIdxBuffer = submeshNIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectIdxBuffer = origIndirectIdxBuffer;
		auto indirectNIdxBuffer = origIndirectNIdxBuffer;
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Single" );
			for ( auto const & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
				{
					if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
					{
						auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
							, *buffer
							, m_nodesIds
							, nodesIdsBuffer
							, maxNodesCount );
						auto pipelinesBuffer = pipelineNodes.data();

						queuerndnd::bindPipeline( commandBuffer
							, *this
							, *pipeline.pipeline
							, *buffer
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

						queuerndnd::registerNodeCommands( *pipeline.pipeline
							, *firstVisibleNode
							, commandBuffer
							, &submeshIdxCommands
							, submeshNIdxCommands
							, visibleNodesCount
							, idxIndex
							, nidxIndex );
						++result;
					}
				}
			}
		}
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Instantiated" );
			for ( auto const & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;
				auto const & buffers = pipelinesNodes.nodes;

				for ( auto const & [buffer, submeshes] : buffers )
				{
					queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipeline.pipeline
						, *buffer
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
							queuerndnd::registerNodeCommands( *pipeline.pipeline
								, node.first
								, commandBuffer
								, &submeshIdxCommands
								, submeshNIdxCommands
								, 1u
								, idxIndex
								, nidxIndex );
							++result;
						}
					}
				}
			}
		}
		submeshIdxCommands.flush( 0u, ashes::WholeSize );
		submeshIdxCommands.unlock();
		submeshNIdxCommands.flush( 0u, ashes::WholeSize );
		submeshNIdxCommands.unlock();

		return result;
	}

#if VK_EXT_mesh_shader
	uint32_t QueueRenderNodes::doPrepareMeshModernCommandBuffersEXT( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		uint32_t mshIndex{};

		auto const & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto const & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		auto const & submeshMshCommands = *m_submeshMeshletIndirectCommandsEXT;
		auto origIndirectIdxBuffer = submeshIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectNIdxBuffer = submeshNIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectMshBuffer = submeshMshCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectIdxBuffer = origIndirectIdxBuffer;
		auto indirectNIdxBuffer = origIndirectNIdxBuffer;
		auto indirectMshBuffer = origIndirectMshBuffer;
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Single" );
			for ( auto const & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
				{
					if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
					{
						auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
							, *buffer
							, m_nodesIds
							, nodesIdsBuffer
							, maxNodesCount );
						auto pipelinesBuffer = pipelineNodes.data();

						auto pipelineId = queuerndnd::bindPipeline( commandBuffer
							, *this
							, *pipeline.pipeline
							, *buffer
							, viewport
							, scissors
							, true );

						if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
						{
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

									queuerndnd::registerNodeCommands( *pipeline.pipeline
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
						else
						{
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

							queuerndnd::registerNodeCommands( *pipeline.pipeline
								, *firstVisibleNode
								, commandBuffer
								, &submeshIdxCommands
								, submeshNIdxCommands
								, visibleNodesCount
								, idxIndex
								, nidxIndex );
							++result;
						}
					}
				}
			}
		}
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Instantiated" );
			for ( auto const & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, submeshes] : pipelinesNodes.nodes )
				{
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipelinesNodes.pipeline.pipeline
						, *buffer
						, viewport
						, scissors
						, true );

					if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
					{
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

								queuerndnd::registerNodeCommands( *pipeline.pipeline
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
					else
					{
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

								queuerndnd::registerNodeCommands( *pipeline.pipeline
									, node.first
									, commandBuffer
									, &submeshIdxCommands
									, submeshNIdxCommands
									, 1u
									, idxIndex
									, nidxIndex );
								++result;
							}
						}
					}
				}
			}
		}
		submeshMshCommands.flush( 0u, ashes::WholeSize );
		submeshMshCommands.unlock();
		submeshIdxCommands.flush( 0u, ashes::WholeSize );
		submeshIdxCommands.unlock();
		submeshNIdxCommands.flush( 0u, ashes::WholeSize );
		submeshNIdxCommands.unlock();

		return result;
	}
#endif

#if VK_NV_mesh_shader
	uint32_t QueueRenderNodes::doPrepareMeshModernCommandBuffersNV( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		uint32_t mshIndex{};

		auto const & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto const & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		auto const & submeshMshCommands = *m_submeshMeshletIndirectCommandsNV;
		auto origIndirectIdxBuffer = submeshIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectNIdxBuffer = submeshNIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectMshBuffer = submeshMshCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectIdxBuffer = origIndirectIdxBuffer;
		auto indirectNIdxBuffer = origIndirectNIdxBuffer;
		auto indirectMshBuffer = origIndirectMshBuffer;
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Single" );
			for ( auto const & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
				{
					if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
					{
						auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
							, *buffer
							, m_nodesIds
							, nodesIdsBuffer
							, maxNodesCount );
						auto pipelinesBuffer = pipelineNodes.data();

						auto pipelineId = queuerndnd::bindPipeline( commandBuffer
							, *this
							, *pipeline.pipeline
							, *buffer
							, viewport
							, scissors
							, true );

						if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
						{
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

									queuerndnd::registerNodeCommands( *pipeline.pipeline
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
						else
						{
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

							queuerndnd::registerNodeCommands( *pipeline.pipeline
								, *firstVisibleNode
								, commandBuffer
								, &submeshIdxCommands
								, submeshNIdxCommands
								, visibleNodesCount
								, idxIndex
								, nidxIndex );
							++result;
						}
					}
				}
			}
		}
		{
			C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Instantiated" );
			for ( auto const & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto const & pipeline = pipelinesNodes.pipeline;

				for ( auto const & [buffer, submeshes] : pipelinesNodes.nodes )
				{
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipelinesNodes.pipeline.pipeline
						, *buffer
						, viewport
						, scissors
						, true );

					if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
					{
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

								queuerndnd::registerNodeCommands( *pipeline.pipeline
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
					else
					{
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

								queuerndnd::registerNodeCommands( *pipeline.pipeline
									, node.first
									, commandBuffer
									, &submeshIdxCommands
									, submeshNIdxCommands
									, 1u
									, idxIndex
									, nidxIndex );
								++result;
							}
						}
					}
				}
			}
		}
		submeshMshCommands.flush( 0u, ashes::WholeSize );
		submeshMshCommands.unlock();
		submeshIdxCommands.flush( 0u, ashes::WholeSize );
		submeshIdxCommands.unlock();
		submeshNIdxCommands.flush( 0u, ashes::WholeSize );
		submeshNIdxCommands.unlock();

		return result;
	}
#endif

	uint32_t QueueRenderNodes::doPrepareBillboardNoDrawIDCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Billboards" );
		uint32_t result{};
		uint32_t nidxIndex{};

		for ( auto const & [_, pipelinesNodes] : m_billboardNodes )
		{
			auto const & pipeline = pipelinesNodes.pipeline;

			for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
			{
				if ( queuerndnd::hasVisibleNode( nodes ) )
				{
					auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
						, *buffer
						, m_nodesIds
						, nodesIdsBuffer
						, maxNodesCount );
					auto pipelinesBuffer = pipelineNodes.data();
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipeline.pipeline
						, *buffer
						, viewport
						, scissors
						, true );
					uint32_t visibleNodesCount{};

					for ( auto const & node : nodes )
					{
						if ( node.visible )
						{
							auto instanceCount = node.node->getInstanceCount();
							queuerndnd::registerNodeCommands( *pipeline.pipeline
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
		}

		return result;
	}

	uint32_t QueueRenderNodes::doPrepareBillboardCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		C3D_DebugTime( getOwner()->getOwner()->getTypeName() + " - Billboards" );
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};

		auto const & billboardCommands = *m_billboardIndirectCommands;
		auto origIndirectBuffer = billboardCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectBuffer = origIndirectBuffer;

		for ( auto const & [_, pipelinesNodes] : m_billboardNodes )
		{
			auto const & pipeline = pipelinesNodes.pipeline;

			for ( auto const & [buffer, nodes] : pipelinesNodes.nodes )
			{
				if ( auto firstVisibleNode = queuerndnd::hasVisibleNode( nodes ) )
				{
					auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
						, *buffer
						, m_nodesIds
						, nodesIdsBuffer
						, maxNodesCount );
					auto pipelinesBuffer = pipelineNodes.data();

					queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipeline.pipeline
						, *buffer
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

					queuerndnd::registerNodeCommands( *pipeline.pipeline
						, *firstVisibleNode
						, commandBuffer
						, nullptr
						, billboardCommands
						, visibleNodesCount
						, idxIndex
						, nidxIndex );
					++result;
				}
			}
		}

		billboardCommands.flush( 0u, ashes::WholeSize );
		billboardCommands.unlock();

		return result;
	}

	//*********************************************************************************************
}
