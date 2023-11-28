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

#define C3D_PrintNodesCounts 1

using ashes::operator==;
using ashes::operator!=;

#pragma warning( disable:4189 )

namespace castor3d
{
	namespace queuerndnd
	{
		template< typename NodeT
			, typename OnSubmeshFuncT >
		static void doTraverseNodes( InstantiatedPipelinesNodesT< NodeT > & nodes
			, OnSubmeshFuncT onSubmesh )
		{
			for ( auto & [pipeline, buffers] : nodes )
			{
				for ( auto & [buffer, passes] : buffers )
				{
					for ( auto & [pass, submeshes] : passes )
					{
						for ( auto & [submesh, nodes] : submeshes )
						{
							onSubmesh( pipeline
								, *pass
								, *submesh
								, nodes );
						}
					}
				}
			}
		}

		template< typename NodeT >
		static bool addRenderNode( PipelineAndID pipeline
			, CountedNodeT< NodeT > const & culled
			, uint32_t drawCount
			, bool isFrontCulled
			, PipelinesNodesT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & node = *culled.node;
			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions );

			if ( bufferChunk.buffer )
			{
				auto & buffer = bufferChunk.buffer->getBuffer();
				nodes.emplace( pipeline, buffer, culled, drawCount, isFrontCulled );
				registerPipelineNodes( pipeline.pipeline->getFlagsHash(), buffer, nodesIds );
				return true;
			}

			return false;
		}

		template< typename NodeT >
		static bool addRenderNode( PipelineAndID & pipeline
			, CountedNodeT< NodeT > const & culled
			, uint32_t drawCount
			, bool isFrontCulled
			, InstantiatedPipelinesNodesT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & node = *culled.node;
			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshData::ePositions );

			if ( bufferChunk.buffer )
			{
				auto & buffer = bufferChunk.buffer->getBuffer();
				nodes.emplace( pipeline, buffer, *node.pass, node.data, culled, drawCount, isFrontCulled );
				registerPipelineNodes( pipeline.pipeline->getFlagsHash(), buffer, nodesIds );
				return true;
			}

			return false;
		}

		//*****************************************************************************************

		template< typename NodeT, uint64_t CountT >
		bool hasVisibleNode( NodesViewT< NodeT, CountT > const & nodes )
		{
			return std::any_of( nodes.begin()
				, nodes.end()
				, []( CountedNodeT< NodeT > const * node )
				{
					return node->visible;
				} );
		}

		static uint32_t doBindPipeline( ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes const & queueNodes
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

			return pipelineId;
		}

		static bool isBufferEnabled( uint32_t submeshFlagIndex
			, PipelineFlags const & pipelineFlags )
		{
			if ( submeshFlagIndex == uint32_t( SubmeshData::ePositions ) )
			{
				return pipelineFlags.enablePosition();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eNormals ) )
			{
				return pipelineFlags.enableNormal();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eTangents ) )
			{
				return pipelineFlags.enableTangentSpace();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eBitangents ) )
			{
				return pipelineFlags.enableBitangent();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eTexcoords0 ) )
			{
				return pipelineFlags.enableTexcoord0();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eTexcoords1 ) )
			{
				return pipelineFlags.enableTexcoord1();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eTexcoords2 ) )
			{
				return pipelineFlags.enableTexcoord2();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eTexcoords3 ) )
			{
				return pipelineFlags.enableTexcoord3();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eColours ) )
			{
				return pipelineFlags.enableColours();
			}

			if ( submeshFlagIndex == uint32_t( SubmeshData::eVelocity ) )
			{
				return pipelineFlags.enableVelocity();
			}

			return true;
		}

		template< typename NodeT >
		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
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
		static uint32_t doParsePipelineCommands( PipelineAndID const & pipeline
			, BuffersNodesViewT< NodeT > const & buffers
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};
			
			for ( auto & [buffer, nodes] : buffers )
			{
				auto pipelineId = doBindPipeline( commandBuffer
					, queueNodes
					, *pipeline.pipeline
					, *buffer
					, viewport
					, scissor );
				doAddGeometryNodeCommands( *pipeline.pipeline
					, *nodes.front()->node
					, commandBuffer
					, indirectIndexedCommands
					, indirectCommands
					, uint32_t( nodes.size() )
					, idxIndex
					, nidxIndex );
				++result;

				for ( auto & node : nodes )
				{
					queueNodes.registerNodePipeline( node->node->getId(), pipelineId );
				}
			}

			return result;
		}

		template< typename NodeT >
		static uint32_t doParseRenderNodesCommands( PipelinesDrawnNodesT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};
#if C3D_DebugTimers
			CU_TimeEx( queueNodes.getOwner()->getOwner()->getTypeName() );
#endif


			for ( auto & [id, pipelineNodes] : inputNodes )
			{
				result += doParsePipelineCommands( pipelineNodes.pipeline
					, pipelineNodes.buffers
					, commandBuffer
					, queueNodes
					, viewport
					, scissor
					, indirectIndexedCommands
					, indirectCommands
					, idxIndex
					, nidxIndex );
			}

			return result;
		}

		template< typename NodeT >
		static uint32_t doParseRenderNodesCommands( InstantiatedPipelinesNodesT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};

			for ( auto & [_, pipelinesNodes] : inputNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;
				auto & buffers = pipelinesNodes.buffers;

				for ( auto & [buffer, passes] : buffers )
				{
					//for ( auto & [pass, submeshes] : passes )
					//{
						for ( auto & [submesh, nodes] : passes )
						{
							auto pipelineId = doBindPipeline( commandBuffer
								, queueNodes
								, *pipeline.pipeline
								, *buffer
								, viewport
								, scissor );
							doAddGeometryNodeCommands( *pipeline.pipeline
								, *nodes.front()->node
								, commandBuffer
								, indirectIndexedCommands
								, indirectCommands
								, 1u
								, idxIndex
								, nidxIndex );
							++result;

							for ( auto & node : nodes )
							{
								queueNodes.registerNodePipeline( node->node->getId(), pipelineId );
							}
						}
					//}
				}
			}

			return result;
		}

#if VK_EXT_mesh_shader || VK_NV_mesh_shader

		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
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

		template< typename NodeT >
		static uint32_t doParseRenderNodesCommands( PipelinesDrawnNodesT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< DrawMeshTesksIndexedCommand > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};

			for ( auto & [id, pipelineNodes] : inputNodes )
			{
				auto & pipeline = pipelineNodes.pipeline;
				auto & buffers = pipelineNodes.buffers;

				if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
				{
					for ( auto & [buffer, nodes] : buffers )
					{
						auto pipelineId = doBindPipeline( commandBuffer
							, queueNodes
							, *pipeline.pipeline
							, *buffer
							, viewport
							, scissor );
						uint32_t drawOffset{};

						for ( auto & node : nodes )
						{
							doAddGeometryNodeCommands( *pipeline.pipeline
								, *node->node
								, commandBuffer
								, indirectMeshCommands
								, pipelineId
								, drawOffset
								, node->instanceCount
								, mshIndex );
							drawOffset += node->instanceCount;
							++result;
							queueNodes.registerNodePipeline( node->node->getId(), pipelineId );
						}
					}
				}
				else
				{
					result += doParsePipelineCommands( pipeline
						, buffers
						, commandBuffer
						, queueNodes
						, viewport
						, scissor
						, indirectIndexedCommands
						, indirectCommands
						, idxIndex
						, nidxIndex );
				}
			}

			return result;
		}

		static uint32_t doParseRenderNodesCommands( InstantiatedPipelinesNodesT< SubmeshRenderNode > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< DrawMeshTesksIndexedCommand > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};

			for ( auto & [_, pipelinesNodes] : inputNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;
				auto & buffers = pipelinesNodes.buffers;

				for ( auto & [buffer, passes] : buffers )
				{
					//for ( auto & [pass, submeshes] : passes )
					//{
						for ( auto & [submesh, nodes] : passes )
						{
							auto pipelineId = doBindPipeline( commandBuffer
								, queueNodes
								, *pipeline.pipeline
								, *buffer
								, viewport
								, scissor );

							for ( auto & node : nodes )
							{
								queueNodes.registerNodePipeline( node->node->getId(), pipelineId );
							}

							auto & node = *nodes.front()->node;

							if ( pipeline.pipeline->hasMeshletDescriptorSetLayout() )
							{
								doAddGeometryNodeCommands( *pipeline.pipeline
									, node
									, commandBuffer
									, indirectMeshCommands
									, pipelineId
									, 0u
									, node.getInstanceCount()
									, mshIndex );
								++result;
							}
							else
							{
								doAddGeometryNodeCommands( *pipeline.pipeline
									, node
									, commandBuffer
									, indirectIndexedCommands
									, indirectCommands
									, 1u
									, idxIndex
									, nidxIndex );
								++result;
							}
						}
					//}
				}
			}

			return result;
		}

#endif

		//*****************************************************************************************

		static uint32_t getInstanceCount( SubmeshRenderNode const & culled )
		{
			auto & instantiation = culled.data.getInstantiation();
			return instantiation.getData().getRefCount( culled.pass->getOwner() );
		}

		static uint32_t getInstanceCount( BillboardRenderNode const & culled )
		{
			return culled.data.getCount();
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, uint32_t instanceCount )
		{
			auto & indexOffset = culled.getSourceBufferOffsets().getBufferChunk( SubmeshData::eIndex );
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectIndexedCommands->indexCount = indexOffset.getCount< uint32_t >();
			indirectIndexedCommands->instanceCount = instanceCount;
			indirectIndexedCommands->firstIndex = indexOffset.getFirst< uint32_t >();
			indirectIndexedCommands->vertexOffset = int32_t( bufferOffsets.getFirstVertex< castor::Point4f >() );
			indirectIndexedCommands->firstInstance = 0u;
			++indirectIndexedCommands;
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< castor::Point4f >( SubmeshData::ePositions );
			indirectCommands->instanceCount = instanceCount;
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< castor::Point3f >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillIndirectCommand( BillboardRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< BillboardVertex >( SubmeshData::ePositions );
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

#ifndef NDEBUG

		static void checkBuffers( SubmeshRenderNode const & firstNode
			, SubmeshRenderNode const & currentNode )
		{
			auto idxBuffer = firstNode.getSourceBufferOffsets().hasData( SubmeshData::eIndex )
				? &firstNode.getSourceBufferOffsets().getBuffer( SubmeshData::eIndex )
				: nullptr;
			auto posBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::ePositions )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::ePositions )
				: nullptr;
			auto nmlBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eNormals )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eNormals )
				: nullptr;
			auto tanBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eTangents )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTangents )
				: nullptr;
			auto bitBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eBitangents )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eBitangents )
				: nullptr;
			auto tex0Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords0 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords0 )
				: nullptr;
			auto tex1Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords1 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords1 )
				: nullptr;
			auto tex2Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords2 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords2 )
				: nullptr;
			auto tex3Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords3 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords3 )
				: nullptr;
			auto colBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eColours )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eColours )
				: nullptr;
			auto bonBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::eSkin )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::eSkin )
				: nullptr;
			auto pmkBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshData::ePassMasks )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshData::ePassMasks )
				: nullptr;

			CU_Require( idxBuffer == ( currentNode.getSourceBufferOffsets().hasData( SubmeshData::eIndex )
				? &currentNode.getSourceBufferOffsets().getBuffer( SubmeshData::eIndex )
				: nullptr ) );
			CU_Require( posBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::ePositions )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::ePositions )
				: nullptr ) );
			CU_Require( nmlBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eNormals )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eNormals )
				: nullptr ) );
			CU_Require( tanBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eTangents )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTangents )
				: nullptr ) );
			CU_Require( bitBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eBitangents )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eBitangents )
				: nullptr ) );
			CU_Require( tex0Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords0 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords0 )
				: nullptr ) );
			CU_Require( tex1Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords1 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords1 )
				: nullptr ) );
			CU_Require( tex2Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords2 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords2 )
				: nullptr ) );
			CU_Require( tex3Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eTexcoords3 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eTexcoords3 )
				: nullptr ) );
			CU_Require( colBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eColours )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eColours )
				: nullptr ) );
			CU_Require( bonBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::eSkin )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::eSkin )
				: nullptr ) );
			CU_Require( pmkBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshData::ePassMasks )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshData::ePassMasks )
				: nullptr ) );
		}

#endif

		static void fillNodeCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillIndirectCommand( node, indirectIdxBuffer, instanceCount );
			}
			else
			{
				fillIndirectCommand( node, indirectNIdxBuffer, instanceCount );
			}
		}

		template< uint64_t CountT >
		static void fillNodeCommands( NodesViewT< SubmeshRenderNode, CountT > const & nodes
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node->node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#ifndef NDEBUG
					checkBuffers( *nodes.front().node, *node.node );
#endif
				}
			}

			fillNodeCommands( *nodes.front()->node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, getInstanceCount( node ) );
			( *pipelinesBuffer ) = node.instance.getId( *node.pass, node.data );
			++pipelinesBuffer;
		}

#if VK_EXT_mesh_shader || VK_NV_mesh_shader

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, DrawMeshTesksIndexedCommand *& indirectMeshCommands
			, uint32_t instanceCount
			, uint32_t taskCount )
		{
#if VK_EXT_mesh_shader
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->groupCountX = taskCount;
				indirectMeshCommands->groupCountY = 0u;
				indirectMeshCommands->groupCountZ = 0u;
				++indirectMeshCommands;
			}
#else
			for ( uint32_t i = 0u; i < instanceCount; ++i )
			{
				indirectMeshCommands->taskCount = taskCount;
				indirectMeshCommands->firstTask = 0u;
				++indirectMeshCommands;
			}
#endif
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, DrawMeshTesksIndexedCommand *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillIndirectCommand( node
					, indirectMeshBuffer
					, node.data.isDynamic() ? 1u : instanceCount
					, node.data.getMeshletsCount() );
			}
			else if ( node.getSourceBufferOffsets().hasData( SubmeshData::eIndex ) )
			{
				fillIndirectCommand( node
					, indirectIdxBuffer
					, instanceCount );
			}
			else
			{
				fillIndirectCommand( node
					, indirectNIdxBuffer
					, instanceCount );
			}
		}

		template< uint64_t CountT >
		static void fillNodeCommands( NodesViewT< SubmeshRenderNode, CountT > const & nodes
			, DrawMeshTesksIndexedCommand *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node->node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#	ifndef NDEBUG
					checkBuffers( *nodes.front().node, *node.node );
#	endif
				}
			}

			fillNodeCommands( *nodes.front()->node
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, DrawMeshTesksIndexedCommand *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, getInstanceCount( node ) );
			( *pipelinesBuffer ) = node.instance.getId( *node.pass, node.data );
			++pipelinesBuffer;
		}

#endif

		static size_t makeHash( SubmeshRenderNode const & node
			, bool frontCulled )
		{
			auto & submesh = node.data;
			auto & pass = *node.pass;
			size_t hash = std::hash< Submesh const * >{}( &submesh );
			hash = castor::hashCombinePtr( hash, pass );
			hash = castor::hashCombine( hash, node.getInstanceCount() );
			hash = castor::hashCombine( hash, frontCulled );
			return hash;
		}

		static size_t makeHash( BillboardRenderNode const & node
			, bool frontCulled )
		{
			auto & billboard = node.data;
			auto & pass = *node.pass;
			size_t hash = std::hash< BillboardBase const * >{}( &billboard );
			hash = castor::hashCombinePtr( hash, pass );
			hash = castor::hashCombine( hash, node.getInstanceCount() );
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
	static std::ostream & operator<<( std::ostream & stream, PipelinesDrawnNodesT< NodeT > const & rhs )
	{
		stream << "    Submeshes " << rhs.size() << " pipelines\n";
		for ( auto & [id, pipelineNodes] : rhs )
		{
			auto & pipeline = pipelineNodes.pipeline;
			auto & buffers = pipelineNodes.buffers;
			stream << "        Pipeline 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << pipeline.pipeline;
			stream << ": " << std::dec << buffers.size() << " buffers\n";

			for ( auto & [buffer, nodes] : buffers )
			{
				if ( buffer )
				{
					stream << "            Buffer 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << buffer;
					stream << ": " << std::dec << nodes.size() << " nodes\n";
				}
			}
		}

		return stream;
	}

	template< typename NodeT >
	static std::ostream & operator<<( std::ostream & stream, InstantiatedPipelinesNodesT< NodeT > const & rhs )
	{
		stream << "    Instantiated Submeshes " << rhs.size() << " pipelines\n";
		for ( auto & [id, pipelineNodes] : rhs )
		{
			auto & pipeline = pipelineNodes.pipeline;

			if ( pipeline.pipeline )
			{
				auto & buffers = pipelineNodes.buffers;
				stream << "        Pipeline 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << pipeline.pipeline;
				stream << ": " << std::dec << buffers.size() << " buffers\n";

				for ( auto & [buffer, submeshes] : buffers )
				{
					if ( buffer )
					{
						stream << "            Buffer 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << buffer;
						stream << ": " << std::dec << submeshes.size() << " submeshes\n";

						for ( auto & [submesh, nodes] : submeshes )
						{
							if ( buffer )
							{
								stream << "                Submesh 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << submesh;
								stream << ": " << std::dec << nodes.size() << " nodes\n";
							}
						}
					}
				}
			}
		}

		return stream;
	}

#endif
	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
		, m_onSubmeshChanged{ queue.getCuller().onSubmeshChanged.connect( [this]( SceneCuller const &
				, CountedNodeT< SubmeshRenderNode > const & node
				, bool visible )
			{
				if ( visible )
				{
					doAddSubmesh( node );
				}
				else
				{
					doRemoveSubmesh( node );
				}
			} ) }
		, m_onBillboardChanged{ queue.getCuller().onBillboardChanged.connect( [this]( SceneCuller const &
				, CountedNodeT< BillboardRenderNode > const & node
				, bool visible )
			{
				if ( visible )
				{
					doAddBillboard( node );
				}
				else
				{
					doRemoveBillboard( node );
				}
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
			m_submeshMeshletIndirectCommands = makeBuffer< DrawMeshTesksIndexedCommand >( device
				, MaxSubmeshMeshletDrawIndirectCommand
				, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, renderPass.getTypeName() + "/SubmeshMeshletIndirectBuffer" );
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
			, [&renderPass]( CountedNodeT< SubmeshRenderNode > const & lookup )
			{
				auto & node = *lookup.node;
				return renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() )
					&& ( !node.data.getInstantiation().isInstanced( node.instance.getMaterial( node.data ) )
						|| node.instance.getParent()->isVisible() );
			} );
		auto billboardsIt = std::find_if( culler.getBillboards().begin()
			, culler.getBillboards().end()
			, [&renderPass]( CountedNodeT< BillboardRenderNode > const & lookup )
			{
				auto & node = *lookup.node;
				return renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() );
			} );

		m_hasNodes = submeshesIt != culler.getSubmeshes().end()
			|| billboardsIt != culler.getBillboards().end();
	}

	void QueueRenderNodes::sortNodes( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
#if C3D_DebugTimers
			CU_TimeEx( renderPass.getTypeName() );
#endif

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
				auto & node = *culled.node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() ) )
				{
					doAddSubmesh( shadowMaps, shadowBuffer, culled );
				}
			}

			for ( auto & culled : culler.getBillboards() )
			{
				auto & node = *culled.node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() ) )
				{
					doAddBillboard( shadowMaps, shadowBuffer, culled );
				}
			}
		}
		fillIndirectBuffers();
		renderPass.onSortNodes( renderPass );
#if C3D_PrintNodesCounts
		log::debug << renderPass.getName() << ":\n";
		log::debug << m_drawnSubmeshNodes << "\n";
		log::debug << m_instancedSubmeshNodes << "\n";
#endif
	}

	void QueueRenderNodes::updateNodes( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		{
#if C3D_DebugTimers
			CU_TimeEx( renderPass.getTypeName() );
#endif

			for ( auto culled : m_pendingSubmeshes )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getParent() ) )
				{
					doAddSubmesh( shadowMaps, shadowBuffer, *culled );
				}
			}

			for ( auto culled : m_pendingBillboards )
			{
				auto & node = *culled->node;

				if ( renderPass.isValidPass( *node.pass )
					&& renderPass.isValidRenderable( node.instance )
					&& renderPass.isValidNode( *node.instance.getNode() ) )
				{
					doAddBillboard( shadowMaps, shadowBuffer, *culled );
				}
			}

			m_pendingSubmeshes.clear();
			m_pendingBillboards.clear();
		}
		fillIndirectBuffers();
		renderPass.onSortNodes( renderPass );
#if C3D_PrintNodesCounts
		log::debug << renderPass.getName() << ":\n";
		log::debug << m_drawnSubmeshNodes << "\n";
		log::debug << m_instancedSubmeshNodes << "\n";
#endif
	}

	void QueueRenderNodes::fillIndirectBuffers()
	{
		m_visible = {};

		if ( !m_pipelinesNodes
			|| m_nodesIds.empty() )
		{
			return;
		}

		m_drawnSubmeshNodes.clear();
		m_drawnBillboardNodes.clear();
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

#if C3D_DebugTimers
		CU_TimeEx( renderPass.getTypeName() );
#endif

		auto nodesIdsBuffer = m_pipelinesNodes->lock( 0u, ashes::WholeSize, 0u );
		auto maxNodesCount = m_pipelinesNodes->getCount();

		if ( !m_submeshNodes.empty()
			|| !m_instancedSubmeshNodes.empty() )
		{
			auto & scene = queue.getCuller().getScene();
			auto origIndirectIdxBuffer = m_submeshIdxIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectIdxBuffer = origIndirectIdxBuffer;

			auto origIndirectNIdxBuffer = m_submeshNIdxIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectNIdxBuffer = origIndirectNIdxBuffer;

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
			if ( renderPass.isMeshShading() )
			{
				auto origIndirectMshBuffer = m_submeshMeshletIndirectCommands
					? m_submeshMeshletIndirectCommands->lock( 0u, ashes::WholeSize, 0u )
					: nullptr;
				auto indirectMshBuffer = origIndirectMshBuffer;

				for ( auto & [_, pipelinesNodes] : m_submeshNodes )
				{
					auto & pipeline = pipelinesNodes.pipeline;

					if ( !pipeline.pipeline )
					{
						continue;
					}

					auto & buffers = pipelinesNodes.buffers;
					auto dstPipelineIt = m_drawnSubmeshNodes.emplace( pipelinesNodes.pipeline
						, pipelinesNodes.isFrontCulled );

					for ( auto & [buffer, nodes] : buffers )
					{
						if ( queuerndnd::hasVisibleNode( nodes ) )
						{
							auto dstBufferIt = dstPipelineIt->buffers.emplace( *buffer );
							auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
								, *buffer
								, m_nodesIds
								, nodesIdsBuffer
								, maxNodesCount );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & culled : nodes )
							{
								if ( culled->visible )
								{
									dstBufferIt->nodes.emplace( *culled );
									queuerndnd::fillNodeCommands( *culled->node
										, scene
										, indirectMshBuffer
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer );
									++m_visible.objectCount;
									m_visible.faceCount += culled->node->data.getFaceCount();
									m_visible.vertexCount += culled->node->data.getPointsCount();
									CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= m_submeshMeshletIndirectCommands->getCount() );
									CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
									CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
									CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
								}
							}
						}
					}
				}

				for ( auto & [_, pipelinesNodes] : m_instancedSubmeshNodes )
				{
					auto & pipeline = pipelinesNodes.pipeline;
					auto & buffers = pipelinesNodes.buffers;

					for ( auto & [buffer, passes] : buffers )
					{
						//for ( auto & [pass, submeshes] : passes )
						//{
							for ( auto & [submesh, nodes] : passes )
							{
								queuerndnd::fillNodeCommands( nodes
									, indirectMshBuffer
									, indirectIdxBuffer
									, indirectNIdxBuffer );
								++m_visible.objectCount;
								m_visible.faceCount += uint32_t( nodes.front()->node->data.getFaceCount() * nodes.size() );
								m_visible.vertexCount += uint32_t( nodes.front()->node->data.getPointsCount() * nodes.size() );
								CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= m_submeshMeshletIndirectCommands->getCount() );
								CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
								CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
							}
						//}
					}
				}

				m_submeshMeshletIndirectCommands->flush( 0u, ashes::WholeSize );
				m_submeshMeshletIndirectCommands->unlock();
			}
			else
#endif
			{
				for ( auto & [_, pipelinesNodes] : m_submeshNodes )
				{
					auto & pipeline = pipelinesNodes.pipeline;

					if ( !pipeline.pipeline )
					{
						continue;
					}

					auto & buffers = pipelinesNodes.buffers;
					auto dstPipelineIt = m_drawnSubmeshNodes.emplace( pipelinesNodes.pipeline
						, pipelinesNodes.isFrontCulled );

					for ( auto & [buffer, nodes] : buffers )
					{
						if ( queuerndnd::hasVisibleNode( nodes ) )
						{
							auto dstBufferIt = dstPipelineIt->buffers.emplace( *buffer );
							auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
								, *buffer
								, m_nodesIds
								, nodesIdsBuffer
								, maxNodesCount );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & culled : nodes )
							{
								if ( culled->visible )
								{
									dstBufferIt->nodes.emplace( *culled );
									queuerndnd::fillNodeCommands( *culled->node
										, scene
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer );
									++m_visible.objectCount;
									m_visible.faceCount += culled->node->data.getFaceCount();
									m_visible.vertexCount += culled->node->data.getPointsCount();
									CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
									CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
									CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
								}
							}
						}
					}
				}

				for ( auto & [_, pipelinesNodes] : m_instancedSubmeshNodes )
				{
					auto & pipeline = pipelinesNodes.pipeline;
					auto & buffers = pipelinesNodes.buffers;

					for ( auto & [buffer, passes] : buffers )
					{
						//for ( auto & [pass, submeshes] : passes )
						//{
							for ( auto & [submesh, nodes] : passes )
							{
								queuerndnd::fillNodeCommands( nodes
									, indirectIdxBuffer
									, indirectNIdxBuffer );
								++m_visible.objectCount;
								m_visible.faceCount += uint32_t( nodes.front()->node->data.getFaceCount() * nodes.size() );
								m_visible.vertexCount += uint32_t( nodes.front()->node->data.getPointsCount() * nodes.size() );
								CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
								CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
							}
						//}
					}
				}
			}

			m_submeshIdxIndirectCommands->flush( 0u, ashes::WholeSize );
			m_submeshIdxIndirectCommands->unlock();
			m_submeshNIdxIndirectCommands->flush( 0u, ashes::WholeSize );
			m_submeshNIdxIndirectCommands->unlock();
		}

		if ( !m_billboardNodes.empty() )
		{
			auto origIndirectBuffer = m_billboardIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectBuffer = origIndirectBuffer;

			for ( auto & [_, pipelinesNodes] : m_billboardNodes )
			{
				auto & pipeline = pipelinesNodes.pipeline;

				if ( !pipeline.pipeline )
				{
					continue;
				}

				auto & buffers = pipelinesNodes.buffers;
				auto dstPipelineIt = m_drawnBillboardNodes.emplace( pipelinesNodes.pipeline
					, pipelinesNodes.isFrontCulled );

				for ( auto & [buffer, nodes] : buffers )
				{
					if ( queuerndnd::hasVisibleNode( nodes ) )
					{
						auto dstBufferIt = dstPipelineIt->buffers.emplace( *buffer );
						auto & pipelineNodes = getPipelineNodes( pipeline.pipeline->getFlagsHash()
							, *buffer
							, m_nodesIds
							, nodesIdsBuffer
							, maxNodesCount );
						auto pipelinesBuffer = pipelineNodes.data();

						for ( auto & culled : nodes )
						{
							if ( culled->visible )
							{
								dstBufferIt->nodes.emplace( *culled );
								queuerndnd::fillIndirectCommand( *culled->node, indirectBuffer );
								( *pipelinesBuffer ) = culled->node->instance.getId( *culled->node->pass );
								++pipelinesBuffer;
								m_visible.billboardCount += culled->node->data.getCount();
								CU_Require( size_t( std::distance( origIndirectBuffer, indirectBuffer ) ) <= m_billboardIndirectCommands->getCount() );
								CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
							}
						}
					}
				}
			}

			m_billboardIndirectCommands->flush( 0u, ashes::WholeSize );
			m_billboardIndirectCommands->unlock();
		}

		m_pipelinesNodes->flush( 0u, ashes::WholeSize );
		m_pipelinesNodes->unlock();
	}

	uint32_t QueueRenderNodes::prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();
		uint32_t result{};

#if C3D_DebugTimers
		CU_TimeEx( renderPass.getTypeName() );
#endif

		ashes::CommandBuffer const & cb = queue.getCommandBuffer();
		cb.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass.getRenderPass( 0u )
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );

		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		auto & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		if ( renderPass.isMeshShading() )
		{
			auto & submeshMshCommands = *m_submeshMeshletIndirectCommands;
			uint32_t mshIndex{};
			result += queuerndnd::doParseRenderNodesCommands( m_drawnSubmeshNodes
				, cb
				, *this
				, viewport
				, scissors
				, submeshMshCommands
				, submeshIdxCommands
				, submeshNIdxCommands
				, mshIndex
				, idxIndex
				, nidxIndex );
			result += queuerndnd::doParseRenderNodesCommands( m_instancedSubmeshNodes
				, cb
				, *this
				, viewport
				, scissors
				, submeshMshCommands
				, submeshIdxCommands
				, submeshNIdxCommands
				, mshIndex
				, idxIndex
				, nidxIndex );
		}
		else
#endif
		{
			result += queuerndnd::doParseRenderNodesCommands( m_drawnSubmeshNodes
				, cb
				, *this
				, viewport
				, scissors
				, submeshIdxCommands
				, submeshNIdxCommands
				, idxIndex
				, nidxIndex );
			result += queuerndnd::doParseRenderNodesCommands( m_instancedSubmeshNodes
				, cb
				, *this
				, viewport
				, scissors
				, submeshIdxCommands
				, submeshNIdxCommands
				, idxIndex
				, nidxIndex );
		}

		auto & billboardCommands = *m_billboardIndirectCommands;
		idxIndex = 0u;
		nidxIndex = 0u;
		result += queuerndnd::doParseRenderNodesCommands( m_drawnBillboardNodes
			, cb
			, *this
			, viewport
			, scissors
			, submeshIdxCommands
			, billboardCommands
			, idxIndex
			, nidxIndex );

		cb.end();

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
		uint32_t maxId{};

		for ( auto & nodeIt : m_nodesPipelinesIds )
		{
			maxId = std::max( nodeIt.second, maxId );
		}

		return maxId;
	}

	void QueueRenderNodes::registerNodePipeline( uint32_t nodeId
		, uint32_t pipelineId )
	{
		m_nodesPipelinesIds.emplace( nodeId, pipelineId );
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
		, CountedNodeT< SubmeshRenderNode > const & counted )
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
		auto & instantiation = node.data.getInstantiation();

		if ( instantiation.isInstanced( node.instance.getMaterial( node.data ) ) )
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
		, CountedNodeT< BillboardRenderNode > const & counted )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node );

		if ( queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, false
			, m_billboardNodes
			, m_nodesIds ) )
		{
			m_hasNodes = true;
		}
	}

	void QueueRenderNodes::doAddSingleSubmesh( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CountedNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );

		if ( queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, frontCulled
			, m_submeshNodes
			, m_nodesIds ) )
		{
			m_hasNodes = true;
		}
	}

	void QueueRenderNodes::doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, CountedNodeT< SubmeshRenderNode > const & counted
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & node = *counted.node;
		auto pipelineId = doGetPipeline( shadowMaps
			, shadowBuffer
			, node
			, frontCulled );

		if ( queuerndnd::addRenderNode( pipelineId
			, counted
			, node.getInstanceCount()
			, frontCulled
			, m_instancedSubmeshNodes
			, m_nodesIds ) )
		{
			m_hasNodes = true;
		}
	}

	void QueueRenderNodes::doAddSubmesh( CountedNodeT< SubmeshRenderNode > const & node )
	{
		m_pendingSubmeshes.insert( &node );
	}

	void QueueRenderNodes::doAddBillboard( CountedNodeT< BillboardRenderNode > const & node )
	{
		m_pendingBillboards.insert( &node );
	}

	void QueueRenderNodes::doRemoveSubmesh( CountedNodeT< SubmeshRenderNode > const & node )
	{
		m_submeshNodes.erase( *node.node );
	}

	void QueueRenderNodes::doRemoveBillboard( CountedNodeT< BillboardRenderNode > const & node )
	{
		m_billboardNodes.erase( *node.node );
	}

	//*********************************************************************************************
}
