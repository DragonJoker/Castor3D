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

CU_ImplementSmartPtr( castor3d, QueueRenderNodes )

#define C3D_PrintNodesCounts 0
#define C3D_PrintNodesFullCounts 0

using ashes::operator==;
using ashes::operator!=;

#pragma warning( disable:4189 )

namespace castor3d
{
	namespace queuerndnd
	{
		template< typename NodeT, template< typename NodeU > typename PipelinesNodesContainerT >
		static bool addRenderNode( PipelineAndID pipeline
			, CulledNodeT< NodeT > const & culled
			, uint32_t drawCount
			, bool isFrontCulled
			, PipelinesNodesContainerT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & node = *culled.node;
			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions );

			if ( bufferChunk.buffer )
			{
				auto & buffer = bufferChunk.buffer->getBuffer();
				nodes.emplace( pipeline, buffer, node, culled, drawCount, isFrontCulled );
				registerPipelineNodes( pipeline.pipeline->getFlagsHash(), buffer, nodesIds );
				return true;
			}

			return false;
		}

		//*****************************************************************************************

		template< typename NodeT >
		bool hasVisibleNode( NodesViewT< NodeT > const & nodes )
		{
			return std::any_of( nodes.begin()
				, nodes.end()
				, []( CulledNodeT< NodeT > const & node )
				{
					return node.visible;
				} );
		}

		static uint32_t bindPipeline( ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, RenderPipeline const & pipeline
			, ashes::BufferBase const & buffer
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

				pipelineId = queueNodes.getPipelineNodesIndex( pipeline.getFlagsHash()
					, buffer );

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

			queueNodes.registerPipelineId( pipelineId );
			return pipelineId;
		}

		template< typename NodeT >
		static void registerNodeCommands( RenderPipeline const & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const * indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t drawCount
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			GeometryBuffers const & geometryBuffers = node.getGeometryBuffers( pipeline.getFlags() );

			for ( uint32_t i = 0u; i < geometryBuffers.buffers.size(); ++i )
			{
				commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i].get().vertexBindingDescriptions[0].binding
					, geometryBuffers.buffers[i]
					, geometryBuffers.offsets[i] );
			}

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

#if VK_EXT_mesh_shader || VK_NV_mesh_shader

		static void registerNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< DrawMeshTesksIndexedCommand > const & indirectMeshCommands
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
			commandBuffer.drawMeshTasksIndirectNV( indirectMeshCommands.getBuffer()
				, mshIndex * sizeof( DrawMeshTesksIndexedCommand )
				, drawCount
				, sizeof( DrawMeshTesksIndexedCommand ) );
			mshIndex += drawCount;
		}

#endif

		//*****************************************************************************************

		static void fillNodeIndirectCommand( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, uint32_t instanceCount )
		{
			auto & indexOffset = node.getSourceBufferOffsets().getBufferChunk( SubmeshData::eIndex );
			auto & bufferOffsets = node.getFinalBufferOffsets();
			indirectIndexedCommands->indexCount = indexOffset.getCount< uint32_t >();
			indirectIndexedCommands->instanceCount = instanceCount;
			indirectIndexedCommands->firstIndex = indexOffset.getFirst< uint32_t >();
			indirectIndexedCommands->vertexOffset = int32_t( bufferOffsets.getFirstVertex< castor::Point4f >() );
			indirectIndexedCommands->firstInstance = 0u;
			++indirectIndexedCommands;
		}

		static void fillNodeIndirectCommand( SubmeshRenderNode const & node
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			auto & bufferOffsets = node.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< castor::Point4f >( SubmeshData::ePositions );
			indirectCommands->instanceCount = instanceCount;
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< castor::Point3f >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillNodeIndirectCommand( BillboardRenderNode const & node
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t *& pipelinesBuffer )
		{
			auto & bufferOffsets = node.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< BillboardVertex >( SubmeshData::ePositions );
			indirectCommands->instanceCount = node.getInstanceCount();
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
			( *pipelinesBuffer ) = node.getId();
			++pipelinesBuffer;
		}

		static void fillNodeIndirectCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillNodeIndirectCommand( node, indirectIdxBuffer, instanceCount );
			}
			else
			{
				fillNodeIndirectCommand( node, indirectNIdxBuffer, instanceCount );
			}
		}

		static void fillNodeIndirectCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t *& pipelinesBuffer
			, uint32_t instanceCount )
		{
			fillNodeIndirectCommands( node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
			( *pipelinesBuffer ) = node.getId();
			++pipelinesBuffer;
		}

#if VK_EXT_mesh_shader || VK_NV_mesh_shader

		static void fillNodeIndirectCommand( SubmeshRenderNode const & culled
			, DrawMeshTesksIndexedCommand *& indirectMeshCommands
			, uint32_t instanceCount
			, uint32_t taskCount )
		{
#	if VK_EXT_mesh_shader
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->groupCountX = taskCount;
				indirectMeshCommands->groupCountY = 0u;
				indirectMeshCommands->groupCountZ = 0u;
				++indirectMeshCommands;
			}
#	else
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->taskCount = taskCount;
				indirectMeshCommands->firstTask = 0u;
				++indirectMeshCommands;
			}
#	endif
		}

		static void fillNodeIndirectCommands( SubmeshRenderNode const & node
			, DrawMeshTesksIndexedCommand *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillNodeIndirectCommand( node
					, indirectMeshBuffer
					, node.data.isDynamic() ? 1u : instanceCount
					, node.data.getMeshletsCount() );
			}
			else if ( node.getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
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

		static void fillNodeIndirectCommands( SubmeshRenderNode const & node
			, DrawMeshTesksIndexedCommand *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t *& pipelinesBuffer
			, uint32_t instanceCount )
		{
			fillNodeIndirectCommands( node
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
			( *pipelinesBuffer ) = node.getId();
			++pipelinesBuffer;
		}

#endif

		static size_t makeHash( SubmeshRenderNode const & node
			, bool frontCulled )
		{
			auto & submesh = node.data;
			auto & pass = *node.pass;
			size_t hash = std::hash< Submesh const * >{}( &submesh );
			hash = castor::hashCombine( hash, pass.getHash() );
			hash = castor::hashCombine( hash, node.isInstanced() );
			hash = castor::hashCombine( hash, frontCulled );
			return hash;
		}

		static size_t makeHash( BillboardRenderNode const & node
			, bool frontCulled )
		{
			auto & billboard = node.data;
			auto & pass = *node.pass;
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
			auto & engine = *renderPass.getEngine();
			auto & components = engine.getSubmeshComponentsRegister();
			auto hash = makeHash( node, frontCulled );
			auto it = pipelines.find( hash );
			Pass const & pass = *node.pass;
			auto & scene = renderPass.getCuller().getScene();
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
#if C3D_PrintNodesCounts

	template< typename NodeT >
	inline castor::String getNodesTypeName();

	template<>
	inline castor::String getNodesTypeName< SubmeshRenderNode >()
	{
		return "Submeshes";
	}

	template<>
	inline castor::String getNodesTypeName< BillboardRenderNode >()
	{
		return "Billboards";
	}

#	if C3D_PrintNodesFullCounts

	template< typename NodeT >
	static std::ostream & operator<<( std::ostream & stream, PipelinesNodesT< NodeT > const & rhs )
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
				if ( queuerndnd::hasVisibleNode( nodes.nodes ) )
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
		return stream;
	}

	template< typename NodeT >
	static std::ostream & operator<<( std::ostream & stream, InstantiatedPipelinesNodesT< NodeT > const & rhs )
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
				stream << "                Pass 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << pass;
				stream << ": " << std::dec << submeshes.size() << " submeshes\n";
					
				for ( auto & [submesh, node] : submeshes )
				{
					maxNodesCount = std::max( maxNodesCount, node->getInstanceCount() );
					totalNodesCount += node->getInstanceCount();
					stream << "                    Submesh 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << node;
					stream << ": " << std::dec << node->getInstanceCount() << "\n";
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
		return stream;
	}

#	else

	template< typename NodeT >
	static std::ostream & operator<<( std::ostream & stream, PipelinesNodesT< NodeT > const & rhs )
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
		return stream;
	}

	template< typename NodeT >
	static std::ostream & operator<<( std::ostream & stream, InstantiatedPipelinesNodesT< NodeT > const & rhs )
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
					maxNodesCount = std::max( maxNodesCount, node->getInstanceCount() );
					totalNodesCount += node->getInstanceCount();
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
		return stream;
	}

#	endif
#endif

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
		, m_onSubmeshChanged{ queue.getCuller().onSubmeshChanged.connect( [this]( SceneCuller const &
				, CulledNodeT< SubmeshRenderNode > const & node
				, bool visible )
			{
				doAddSubmesh( node );
			} ) }
		, m_onBillboardChanged{ queue.getCuller().onBillboardChanged.connect( [this]( SceneCuller const &
				, CulledNodeT< BillboardRenderNode > const & node
				, bool visible )
			{
				doAddBillboard( node );
			} ) }
	{
	}

	void QueueRenderNodes::initialise( RenderDevice const & device )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

		if ( !m_submeshIdxIndirectCommands )
		{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader

			if ( renderPass.isMeshShading() )
			{
				m_submeshMeshletIndirectCommands = makeBuffer< DrawMeshTesksIndexedCommand >( device
					, MaxSubmeshMeshletDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPass.getTypeName() + "/SubmeshMeshletIndirectBuffer" );
			}

#endif
			m_submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
				, MaxSubmeshIdxDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, renderPass.getTypeName() + "/SubmeshIndexedIndirectBuffer" );
			m_submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
				, MaxSubmeshNIdxDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, renderPass.getTypeName() + "/SubmeshIndirectBuffer" );
		}

		if ( !m_billboardIndirectCommands )
		{
			m_billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
				, MaxBillboardDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, renderPass.getTypeName() + "/BillboardIndirectBuffer" );
		}

		if ( !m_pipelinesNodes )
		{
			m_pipelinesNodes = makeBuffer< PipelineNodes >( device
				, MaxPipelines
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, renderPass.getTypeName() + "/NodesIDs" );
		}
	}

	void QueueRenderNodes::cleanup()
	{
		m_pipelinesNodes.reset();
		m_billboardIndirectCommands.reset();
		m_submeshNIdxIndirectCommands.reset();
		m_submeshIdxIndirectCommands.reset();
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		m_submeshMeshletIndirectCommands.reset();
#endif
	}

	void QueueRenderNodes::clear()
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

	bool QueueRenderNodes::sortNodes( ShadowMapLightTypeArray & shadowMaps
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

	bool QueueRenderNodes::updateNodes( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
			C3D_DebugTime( renderPass.getTypeName() );
			auto pendingSubmeshes = std::move( m_pendingSubmeshes );
			auto pendingBillboards = std::move( m_pendingBillboards );

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
		, ashes::Optional< VkRect2D > const & scissors )
	{
		uint32_t result{};
		m_visible = {};
		m_maxPipelineId = {};

		if ( !m_pipelinesNodes
			|| m_nodesIds.empty() )
		{
			return result;
		}

		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
			C3D_DebugTime( renderPass.getTypeName() + " - Overall" );
			ashes::CommandBuffer const & commandBuffer = queue.getCommandBuffer();
			commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
				, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass.getRenderPass( 0u )
					, 0u
					, VkFramebuffer( nullptr )
					, VK_FALSE
					, 0u
					, 0u ) );

			auto maxNodesCount = m_pipelinesNodes->getCount();
			auto nodesIdsBuffer = m_pipelinesNodes->lock( 0u, ashes::WholeSize, 0u );

			if ( !m_submeshNodes.empty()
				|| !m_instancedSubmeshNodes.empty() )
			{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
				if ( renderPass.isMeshShading() )
				{
					result += doPrepareMeshModernCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
				}
				else
#endif
				{
					result += doPrepareMeshTraditionalCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
				}
			}

			if ( !m_billboardNodes.empty() )
			{
				result += doPrepareBillboardCommandBuffers( commandBuffer, viewport, scissors, nodesIdsBuffer, maxNodesCount );
			}

			m_pipelinesNodes->flush( 0u, ashes::WholeSize );
			m_pipelinesNodes->unlock();

			commandBuffer.end();
		}
#if C3D_PrintNodesCounts
		log::debug << renderPass.getName() << ":\n";
		log::debug << m_submeshNodes << "\n";
		log::debug << m_instancedSubmeshNodes << "\n";
		log::debug << m_billboardNodes << "\n";
#endif
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
		auto & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, submesh, pass, isFrontCulled )
			, buffer );
	}

	uint32_t QueueRenderNodes::getPipelineNodesIndex( BillboardBase const & billboard
		, Pass const & pass
		, ashes::BufferBase const & buffer
		, bool isFrontCulled )const
	{
		auto & rp = *getOwner()->getOwner();
		return getPipelineNodesIndex( getPipelineBaseHash( rp, billboard, pass, isFrontCulled )
			, buffer );
	}

	PipelineAndID QueueRenderNodes::doGetPipeline( ShadowMapLightTypeArray & shadowMaps
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
			it = m_pipelines.emplace( hash, result ).first;
			renderPass.initialiseAdditionalDescriptor( *result.pipeline
				, shadowMaps
				, shadowBuffer );
		}

		return it->second;
	}

	PipelineAndID QueueRenderNodes::doGetPipeline( ShadowMapLightTypeArray & shadowMaps
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
			it = m_pipelines.emplace( hash, result ).first;
			renderPass.initialiseAdditionalDescriptor( *result.pipeline
				, shadowMaps
				, shadowBuffer );
		}

		return it->second;
	}

	void QueueRenderNodes::doAddSubmesh( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted )
	{
		auto & renderPass = *getOwner()->getOwner();
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

	void QueueRenderNodes::doAddBillboard( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< BillboardRenderNode > const & counted )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node );
		m_hasNodes = true;

		if ( queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, false
			, m_billboardNodes
			, m_nodesIds ) )
		{
			m_pendingBillboards.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddSingleSubmesh( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );
		m_hasNodes = true;

		if ( !queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, frontCulled
			, m_submeshNodes
			, m_nodesIds ) )
		{
			m_pendingSubmeshes.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CulledNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );
		m_hasNodes = true;

		if ( !queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, frontCulled
			, m_instancedSubmeshNodes
			, m_nodesIds ) )
		{
			m_pendingSubmeshes.insert( &counted );
		}
	}

	void QueueRenderNodes::doAddSubmesh( CulledNodeT< SubmeshRenderNode > const & node )
	{
		m_pendingSubmeshes.insert( &node );
	}

	void QueueRenderNodes::doAddBillboard( CulledNodeT< BillboardRenderNode > const & node )
	{
		m_pendingBillboards.insert( &node );
	}

	void QueueRenderNodes::doRemoveSubmesh( CulledNodeT< SubmeshRenderNode > const & node )
	{
		m_submeshNodes.erase( *node.node );
		auto it = m_pendingSubmeshes.find( &node );

		if ( it != m_pendingSubmeshes.end() )
		{
			m_pendingSubmeshes.erase( it );
		}
	}

	void QueueRenderNodes::doRemoveBillboard( CulledNodeT< BillboardRenderNode > const & node )
	{
		m_billboardNodes.erase( *node.node );
		auto it = m_pendingBillboards.find( &node );

		if ( it != m_pendingBillboards.end() )
		{
			m_pendingBillboards.erase( it );
		}
	}

	uint32_t QueueRenderNodes::doPrepareMeshTraditionalCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		auto & renderPass = *getOwner()->getOwner();
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};

		auto & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		auto origIndirectIdxBuffer = submeshIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectNIdxBuffer = submeshNIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectIdxBuffer = origIndirectIdxBuffer;
		auto indirectNIdxBuffer = origIndirectNIdxBuffer;
		{
			C3D_DebugTime( renderPass.getTypeName() + " - Single" );
			for ( auto & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;

				for ( auto & [buffer, nodes] : pipelinesNodes.nodes )
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
							, scissors );
						uint32_t visibleNodesCount{};
						SubmeshRenderNode const * firstVisibleNode{};

						for ( auto & node : nodes )
						{
							if ( node.visible )
							{
								auto instanceCount = node.node->getInstanceCount();
								queuerndnd::fillNodeIndirectCommands( *node.node
									, indirectIdxBuffer
									, indirectNIdxBuffer
									, pipelinesBuffer
									, instanceCount );
								m_visible.objectCount += instanceCount;
								m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
								m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
								CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
								CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
								CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
								++visibleNodesCount;

								if ( !firstVisibleNode )
								{
									firstVisibleNode = node.node;
								}
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
			C3D_DebugTime( renderPass.getTypeName() + " - Instantiated" );
			for ( auto & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;
				auto & buffers = pipelinesNodes.nodes;

				for ( auto & [buffer, submeshes] : buffers )
				{
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipeline.pipeline
						, *buffer
						, viewport
						, scissors );

					for ( auto & [submesh, node] : submeshes )
					{
						auto instanceCount = node->getInstanceCount();
						queuerndnd::fillNodeIndirectCommands( *node
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, instanceCount );
						m_visible.objectCount += instanceCount;
						m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
						m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
						CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
						CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
						queuerndnd::registerNodeCommands( *pipeline.pipeline
							, *node
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
		submeshIdxCommands.flush( 0u, ashes::WholeSize );
		submeshIdxCommands.unlock();
		submeshNIdxCommands.flush( 0u, ashes::WholeSize );
		submeshNIdxCommands.unlock();

		return result;
	}

	uint32_t QueueRenderNodes::doPrepareMeshModernCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		auto & renderPass = *getOwner()->getOwner();
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		uint32_t mshIndex{};

		auto & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		auto & submeshMshCommands = *m_submeshMeshletIndirectCommands;
		auto origIndirectIdxBuffer = submeshIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectNIdxBuffer = submeshNIdxCommands.lock( 0u, ashes::WholeSize, 0u );
		auto origIndirectMshBuffer = submeshMshCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectIdxBuffer = origIndirectIdxBuffer;
		auto indirectNIdxBuffer = origIndirectNIdxBuffer;
		auto indirectMshBuffer = origIndirectMshBuffer;
		{
			C3D_DebugTime( renderPass.getTypeName() + " - Single" );
			for ( auto & [_, pipelinesNodes] : m_submeshNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;

				for ( auto & [buffer, nodes] : pipelinesNodes.nodes )
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
							, scissors );

						if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
						{
							uint32_t drawOffset{};

							for ( auto & node : nodes )
							{
								if ( node.visible )
								{
									auto instanceCount = node.node->getInstanceCount();
									queuerndnd::fillNodeIndirectCommands( *node.node
										, indirectMshBuffer
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer
										, instanceCount );
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
										, node.instanceCount
										, mshIndex );
									drawOffset += node.instanceCount;
									++result;
								}
							}
						}
						else
						{
							uint32_t visibleNodesCount{};
							SubmeshRenderNode const * firstVisibleNode{};

							for ( auto & node : nodes )
							{
								if ( node.visible )
								{
									auto instanceCount = node.node->getInstanceCount();
									queuerndnd::fillNodeIndirectCommands( *node.node
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer
										, instanceCount );
									m_visible.objectCount += instanceCount;
									m_visible.faceCount += node.node->data.getFaceCount() * instanceCount;
									m_visible.vertexCount += node.node->data.getPointsCount() * instanceCount;
									CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
									CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );
									CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
									++visibleNodesCount;

									if ( !firstVisibleNode )
									{
										firstVisibleNode = node.node;
									}
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
			C3D_DebugTime( renderPass.getTypeName() + " - Instantiated" );
			for ( auto & [_, pipelinesNodes] : m_instancedSubmeshNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;

				for ( auto & [buffer, submeshes] : pipelinesNodes.nodes )
				{
					auto pipelineId = queuerndnd::bindPipeline( commandBuffer
						, *this
						, *pipelinesNodes.pipeline.pipeline
						, *buffer
						, viewport
						, scissors );

					if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
					{
						for ( auto & [submesh, node] : submeshes )
						{
							auto instanceCount = node->getInstanceCount();
							queuerndnd::fillNodeIndirectCommands( *node
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
								, *node
								, commandBuffer
								, submeshMshCommands
								, pipelineId
								, 0u
								, node->getInstanceCount()
								, mshIndex );
							++result;
						}
					}
					else
					{
						for ( auto & [submesh, node] : submeshes )
						{
							auto instanceCount = node->getInstanceCount();
							queuerndnd::fillNodeIndirectCommands( *node
								, indirectIdxBuffer
								, indirectNIdxBuffer
								, instanceCount );
							m_visible.objectCount += instanceCount;
							m_visible.faceCount += uint32_t( submesh->getFaceCount() * instanceCount );
							m_visible.vertexCount += uint32_t( submesh->getPointsCount() * instanceCount );
							CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= submeshIdxCommands.getCount() );
							CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= submeshNIdxCommands.getCount() );

							queuerndnd::registerNodeCommands( *pipeline.pipeline
								, *node
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
		submeshMshCommands.flush( 0u, ashes::WholeSize );
		submeshMshCommands.unlock();
		submeshIdxCommands.flush( 0u, ashes::WholeSize );
		submeshIdxCommands.unlock();
		submeshNIdxCommands.flush( 0u, ashes::WholeSize );
		submeshNIdxCommands.unlock();

		return result;
	}

	uint32_t QueueRenderNodes::doPrepareBillboardCommandBuffers( ashes::CommandBuffer const & commandBuffer
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors
		, PipelineNodes * nodesIdsBuffer
		, VkDeviceSize maxNodesCount )
	{
		auto & renderPass = *getOwner()->getOwner();
		C3D_DebugTime( renderPass.getTypeName() + " - Billboards" );
		uint32_t result{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};

		auto & billboardCommands = *m_billboardIndirectCommands;
		auto origIndirectBuffer = billboardCommands.lock( 0u, ashes::WholeSize, 0u );
		auto indirectBuffer = origIndirectBuffer;

		for ( auto & [_, pipelinesNodes] : m_billboardNodes )
		{
			auto & pipeline = pipelinesNodes.pipeline;

			for ( auto & [buffer, nodes] : pipelinesNodes.nodes )
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
						, scissors );
					uint32_t visibleNodesCount{};
					BillboardRenderNode const * firstVisibleNode{};

					for ( auto & node : nodes )
					{
						if ( node.visible )
						{
							queuerndnd::fillNodeIndirectCommand( *node.node
								, indirectBuffer
								, pipelinesBuffer );
							m_visible.billboardCount += node.node->data.getCount();
							CU_Require( size_t( std::distance( origIndirectBuffer, indirectBuffer ) ) <= billboardCommands.getCount() );
							CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
							++visibleNodesCount;

							if ( !firstVisibleNode )
							{
								firstVisibleNode = node.node;
							}
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
