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
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <CastorUtils/Miscellaneous/BlockTimer.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, QueueRenderNodes )

using ashes::operator==;
using ashes::operator!=;

#pragma warning( disable:4189 )

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
		static void addRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, uint32_t drawCount
			, bool isFrontCulled
			, NodePtrByPipelineMapT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			NodePtrByBufferMapT< NodeT > & pipelineMap = nodes.emplace( &pipeline, NodePtrByBufferMapT< NodeT >{} ).first->second;
			NodeArrayT< NodeT > & bufferMap = pipelineMap.emplace( buffer, NodeArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( bufferMap.begin()
				, bufferMap.end()
				, [&node]( CountedNodeT< NodeT > const & lookup )
				{
					return compareOffsets( *lookup.node, node );
				} );
			CU_Require( buffer );
			bufferMap.emplace( it, CountedNodeT< NodeT >{ &node, drawCount, isFrontCulled } );
			registerPipelineNodes( pipeline.getFlagsHash(), *buffer, nodesIds );
		}

		template< typename NodeT >
		static void addRenderNode( RenderPipeline & pipeline
			, NodeT const & node
			, uint32_t drawCount
			, bool isFrontCulled
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes
			, PipelineBufferArray & nodesIds )
		{
			auto & bufferChunk = node.getFinalBufferOffsets().getBufferChunk( SubmeshFlag::ePositions );
			auto buffer = &bufferChunk.buffer->getBuffer().getBuffer();
			ObjectNodesPtrByBufferMapT< NodeT > & pipelineMap = nodes.emplace( &pipeline, ObjectNodesPtrByBufferMapT< NodeT >{} ).first->second;
			ObjectNodesPtrByPassT< NodeT > & bufferMap = pipelineMap.emplace( buffer, ObjectNodesPtrByPassT< NodeT >{} ).first->second;
			ObjectNodesPtrMapT< NodeT > & passMap = bufferMap.emplace( node.pass, ObjectNodesPtrMapT< NodeT >{} ).first->second;
			NodeArrayT< NodeT > & objectMap = passMap.emplace( &node.data, NodeArrayT< NodeT >{} ).first->second;
			auto it = std::find_if( objectMap.begin()
				, objectMap.end()
				, [&node]( CountedNodeT< NodeT > const & lookup )
				{
					return compareOffsets( *lookup.node, node );
				} );
			CU_Require( buffer );
			objectMap.emplace( it, CountedNodeT< NodeT >{ &node, drawCount, isFrontCulled } );
			registerPipelineNodes( pipeline.getFlagsHash(), *buffer, nodesIds );
		}

		//*****************************************************************************************

		template< typename NodeT >
		static uint32_t doBindPipeline( ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes const & queueNodes
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
		static uint32_t doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
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

			for ( auto & pipelines : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelines.first;

				for ( auto & buffers : pipelines.second )
				{
					auto pipelineId = doBindPipeline( commandBuffer
						, queueNodes
						, pipeline
						, *buffers.first
						, *buffers.second.front().node
						, viewport
						, scissor );
					doAddGeometryNodeCommands( pipeline
						, *buffers.second.front().node
						, commandBuffer
						, indirectIndexedCommands
						, indirectCommands
						, uint32_t( buffers.second.size() )
						, idxIndex
						, nidxIndex );
					++result;

					if constexpr ( VisibilityResolvePass::useCompute )
					{
						for ( auto & nodeIt : buffers.second )
						{
							queueNodes.registerNodePipeline( nodeIt.node->getId(), pipelineId );
						}
					}
				}
			}

			return result;
		}

#if VK_NV_mesh_shader

		static void doAddGeometryNodeCommands( RenderPipeline const & pipeline
			, SubmeshRenderNode const & node
			, ashes::CommandBuffer const & commandBuffer
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
			commandBuffer.drawMeshTasksIndirectNV( indirectMeshCommands.getBuffer()
				, mshIndex * sizeof( VkDrawMeshTasksIndirectCommandNV )
				, drawCount
				, sizeof( VkDrawMeshTasksIndirectCommandNV ) );
			mshIndex += drawCount;
		}

		template< typename NodeT >
		static uint32_t doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};

			for ( auto & pipelineIt : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelineIt.first;
				NodePtrByBufferMapT< NodeT > & buffers = pipelineIt.second;

				for ( auto & bufferIt : buffers )
				{
					NodeArrayT< NodeT > & nodes = bufferIt.second;
					auto pipelineId = doBindPipeline( commandBuffer
						, queueNodes
						, pipeline
						, *bufferIt.first
						, *nodes.front().node
						, viewport
						, scissor );

					if ( queueNodes.getOwner()->getOwner()->isMeshShading()
						&& pipeline.hasMeshletDescriptorSetLayout() )
					{
						uint32_t drawOffset{};

						for ( auto & nodeIt : nodes )
						{
							doAddGeometryNodeCommands( pipeline
								, *nodeIt.node
								, commandBuffer
								, indirectMeshCommands
								, pipelineId
								, drawOffset
								, nodeIt.instanceCount
								, mshIndex );
							drawOffset += nodeIt.instanceCount;
							++result;

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								queueNodes.registerNodePipeline( nodeIt.node->getId(), pipelineId );
							}
						}
					}
					else
					{
						doAddGeometryNodeCommands( pipeline
							, *nodes.front().node
							, commandBuffer
							, indirectIndexedCommands
							, indirectCommands
							, uint32_t( nodes.size() )
							, idxIndex
							, nidxIndex );
						++result;

						if constexpr ( VisibilityResolvePass::useCompute )
						{
							for ( auto & nodeIt : nodes )
							{
								queueNodes.registerNodePipeline( nodeIt.node->getId(), pipelineId );
							}
						}
					}
				}
			}

			return result;
		}

		static uint32_t doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, QueueRenderNodes & queueNodes
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & indirectMeshCommands
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & mshIndex
			, uint32_t & idxIndex
			, uint32_t & nidxIndex )
		{
			uint32_t result{};

			for ( auto & pipelineIt : inputNodes )
			{
				RenderPipeline const & pipeline = *pipelineIt.first;
				ObjectNodesPtrByBufferMapT< SubmeshRenderNode > & buffers = pipelineIt.second;

				for ( auto & bufferIt : buffers )
				{
					ObjectNodesPtrByPassT< SubmeshRenderNode > & passes = bufferIt.second;

					for ( auto & passIt : passes )
					{
						ObjectNodesPtrMapT< SubmeshRenderNode > & submeshes = passIt.second;

						for ( auto & submeshIt : submeshes )
						{
							auto pipelineId = doBindPipeline( commandBuffer
								, queueNodes
								, pipeline
								, *bufferIt.first
								, *submeshIt.second.front().node
								, viewport
								, scissor );

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								for ( auto & nodeIt : submeshIt.second )
								{
									queueNodes.registerNodePipeline( nodeIt.node->getId(), pipelineId );
								}
							}

							auto & node = *submeshIt.second.front().node;

							if ( queueNodes.getOwner()->getOwner()->isMeshShading()
								&& pipeline.hasMeshletDescriptorSetLayout() )
							{
								doAddGeometryNodeCommands( pipeline
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
								doAddGeometryNodeCommands( pipeline
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
					}
				}
			}

			return result;
		}

#else

		template< typename NodeT >
		static uint32_t doParseRenderNodesCommands( ObjectNodesPtrByPipelineMapT< NodeT > & inputNodes
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
								, queueNodes
								, pipeline
								, *bufferIt.first
								, *submeshIt.second.front()
								, viewport
								, scissor );
							doAddGeometryNodeCommands( pipeline
								, *submeshIt.second.front()
								, commandBuffer
								, indirectIndexedCommands
								, indirectCommands
								, 1u
								, idxIndex
								, nidxIndex );
							++result;

							if constexpr ( VisibilityResolvePass::useCompute )
							{
								for ( auto & nodeIt : submeshIt.second )
								{
									queueNodes.registerNodePipeline( nodeIt.node->getId(), pipelineId );
								}
							}
						}
					}
				}
			}

			return result;
		}

#endif

		//*****************************************************************************************

		static uint32_t getInstanceCount( SubmeshRenderNode const & culled )
		{
			auto & instantiation = culled.data.getInstantiation();
			return instantiation.getRefCount( culled.pass->getOwner() );
		}

		static uint32_t getInstanceCount( BillboardRenderNode const & culled )
		{
			return culled.data.getCount();
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, uint32_t instanceCount )
		{
			auto & indexOffset = culled.getSourceBufferOffsets().getBufferChunk( SubmeshFlag::eIndex );
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
			indirectCommands->vertexCount = bufferOffsets.getCount< castor::Point4f >( SubmeshFlag::ePositions );
			indirectCommands->instanceCount = instanceCount;
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< castor::Point3f >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillIndirectCommand( BillboardRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< BillboardVertex >( SubmeshFlag::ePositions );
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

#ifndef NDEBUG

		static void checkBuffers( SubmeshRenderNode const & firstNode
			, SubmeshRenderNode const & currentNode )
		{
			auto idxBuffer = firstNode.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex )
				? &firstNode.getSourceBufferOffsets().getBuffer( SubmeshFlag::eIndex )
				: nullptr;
			auto posBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePositions )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions )
				: nullptr;
			auto nmlBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eNormals )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eNormals )
				: nullptr;
			auto tanBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTangents )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTangents )
				: nullptr;
			auto tex0Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords0 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords0 )
				: nullptr;
			auto tex1Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords1 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords1 )
				: nullptr;
			auto tex2Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords2 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords2 )
				: nullptr;
			auto tex3Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords3 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords3 )
				: nullptr;
			auto colBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eColours )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eColours )
				: nullptr;
			auto bonBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eSkin )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eSkin )
				: nullptr;
			auto pmkBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePassMasks )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePassMasks )
				: nullptr;

			CU_Require( idxBuffer == ( currentNode.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex )
				? &currentNode.getSourceBufferOffsets().getBuffer( SubmeshFlag::eIndex )
				: nullptr ) );
			CU_Require( posBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePositions )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions )
				: nullptr ) );
			CU_Require( nmlBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eNormals )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eNormals )
				: nullptr ) );
			CU_Require( tanBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTangents )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTangents )
				: nullptr ) );
			CU_Require( tex0Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords0 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords0 )
				: nullptr ) );
			CU_Require( tex1Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords1 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords1 )
				: nullptr ) );
			CU_Require( tex2Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords2 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords2 )
				: nullptr ) );
			CU_Require( tex3Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords3 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords3 )
				: nullptr ) );
			CU_Require( colBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eColours )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eColours )
				: nullptr ) );
			CU_Require( bonBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eSkin )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eSkin )
				: nullptr ) );
			CU_Require( pmkBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePassMasks )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePassMasks )
				: nullptr ) );
		}

#endif

#if VK_NV_mesh_shader

		static void fillIndirectCommand( SubmeshRenderNode const & culled
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

		static void fillNodeCommands( SubmeshRenderNode const & node
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( meshShading
				&& node.data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillIndirectCommand( node
					, indirectMeshBuffer
					, node.data.isDynamic() ? 1u : instanceCount
					, node.data.getMeshletsCount() );
			}
			else if ( node.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex ) )
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

		static void fillNodeCommands( NodeArrayT< SubmeshRenderNode > const & nodes
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node.node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#	ifndef NDEBUG
					checkBuffers( *nodes.front().node, *node.node );
#	endif
				}
			}

			fillNodeCommands( *nodes.front().node
				, meshShading
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, meshShading
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, getInstanceCount( node ) );
			( *pipelinesBuffer ) = node.instance.getId( *node.pass, node.data );
			++pipelinesBuffer;
		}

#else

		static void fillNodeCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex ) )
			{
				fillIndirectCommand( node, indirectIdxBuffer, instanceCount );
			}
			else
			{
				fillIndirectCommand( node, indirectNIdxBuffer, instanceCount );
			}
		}

		static void fillNodeCommands( NodeArrayT< SubmeshRenderNode > const & nodes
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node.node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#ifndef NDEBUG
					checkBuffers( *nodes.front().node, *node.node );
#endif
				}
			}

			fillNodeCommands( *nodes.front().node
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

#endif
	}

	//*************************************************************************************************

	QueueRenderNodes::QueueRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
	{
	}

	void QueueRenderNodes::initialise( RenderDevice const & device )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

		if ( !m_submeshIdxIndirectCommands )
		{
#if VK_NV_mesh_shader
			m_submeshMeshletIndirectCommands = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
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
#if VK_NV_mesh_shader
		m_submeshMeshletIndirectCommands.reset();
#endif
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

	void QueueRenderNodes::sortNodes( ShadowMapLightTypeArray & shadowMaps )
	{
		auto & queue = *getOwner();
		auto & renderPass = *queue.getOwner();

#if C3D_DebugTimers
		CU_TimeEx( renderPass.getTypeName() );
#endif

		auto & culler = queue.getCuller();
		m_hasNodes = false;
		m_nodesIds.clear();
		m_submeshNodes.clear();
		m_instancedSubmeshNodes.clear();
		m_billboardNodes.clear();

		for ( auto & culled : culler.getSubmeshes() )
		{
			auto & node = *culled.node;

			if ( renderPass.isValidPass( *node.pass )
				&& renderPass.isValidRenderable( node.instance )
				&& renderPass.isValidNode( *node.instance.getParent() ) )
			{
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
							, *culled.node
							, false );

						if ( needsFront )
						{
							doAddInstancedSubmesh( shadowMaps
								, *culled.node
								, true );
						}
					}
				}
				else
				{
					doAddSubmesh( shadowMaps
						, *culled.node
						, false );

					if ( needsFront )
					{
						doAddSubmesh( shadowMaps
							, *culled.node
							, true );
					}
				}
			}
		}

		for ( auto & culled : culler.getBillboards() )
		{
			auto & node = *culled.node;

			if ( renderPass.isValidPass( *node.pass )
				&& renderPass.isValidRenderable( node.instance )
				&& renderPass.isValidNode( *node.instance.getNode() ) )
			{
				doAddBillboard( shadowMaps, node );
			}
		}

		fillIndirectBuffers();
		renderPass.onSortNodes( renderPass );
	}

	void QueueRenderNodes::fillIndirectBuffers()
	{
		if ( !m_pipelinesNodes
			|| m_nodesIds.empty() )
		{
			return;
		}

		auto nodesIdsBuffer = m_pipelinesNodes->lock( 0u, ashes::WholeSize, 0u );
		auto maxNodesCount = m_pipelinesNodes->getCount();

		if ( !m_submeshNodes.empty()
			|| !m_instancedSubmeshNodes.empty() )
		{
			auto & queue = *getOwner();
			auto & scene = queue.getCuller().getScene();
#if VK_NV_mesh_shader
			auto renderPass = queue.getOwner();
			auto origIndirectMshBuffer = m_submeshMeshletIndirectCommands
				? m_submeshMeshletIndirectCommands->lock( 0u, ashes::WholeSize, 0u )
				: nullptr;
			auto indirectMshBuffer = origIndirectMshBuffer;
#endif
			auto origIndirectIdxBuffer = m_submeshIdxIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectIdxBuffer = origIndirectIdxBuffer;

			auto origIndirectNIdxBuffer = m_submeshNIdxIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectNIdxBuffer = origIndirectNIdxBuffer;

			for ( auto & pipelineIt : m_submeshNodes )
			{
				for ( auto & bufferIt : pipelineIt.second )
				{
					auto & pipelineNodes = getPipelineNodes( pipelineIt.first->getFlagsHash()
						, *bufferIt.first
						, m_nodesIds
						, nodesIdsBuffer
						, maxNodesCount );
					auto pipelinesBuffer = pipelineNodes.data();

					for ( auto & culled : bufferIt.second )
					{
#if VK_NV_mesh_shader
						queuerndnd::fillNodeCommands( *culled.node
							, scene
							, renderPass->isMeshShading()
							, indirectMshBuffer
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, pipelinesBuffer );
						CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= m_submeshMeshletIndirectCommands->getCount() );
#else
						cullscn::fillNodeCommands( *culled.node
							, scene
							, indirectIdxBuffer
							, indirectNIdxBuffer
							, pipelinesBuffer );
#endif
						CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
						CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
					}
				}
			}

			for ( auto & pipelineIt : m_instancedSubmeshNodes )
			{
				for ( auto & bufferIt : pipelineIt.second )
				{
					for ( auto & passIt : bufferIt.second )
					{
						for ( auto & submeshIt : passIt.second )
						{
#if VK_NV_mesh_shader
							queuerndnd::fillNodeCommands( submeshIt.second
								, renderPass->isMeshShading()
								, indirectMshBuffer
								, indirectIdxBuffer
								, indirectNIdxBuffer );
							CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= m_submeshMeshletIndirectCommands->getCount() );
#else
							cullscn::fillNodeCommands( submeshIt.second
								, indirectIdxBuffer
								, indirectNIdxBuffer );
#endif
							CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= m_submeshIdxIndirectCommands->getCount() );
							CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= m_submeshNIdxIndirectCommands->getCount() );
						}
					}
				}
			}

#if VK_NV_mesh_shader

			if ( m_submeshMeshletIndirectCommands )
			{
				m_submeshMeshletIndirectCommands->flush( 0u, ashes::WholeSize );
				m_submeshMeshletIndirectCommands->unlock();
			}

#endif
			m_submeshIdxIndirectCommands->flush( 0u, ashes::WholeSize );
			m_submeshIdxIndirectCommands->unlock();
			m_submeshNIdxIndirectCommands->flush( 0u, ashes::WholeSize );
			m_submeshNIdxIndirectCommands->unlock();
		}

		if ( !m_billboardNodes.empty() )
		{
			auto origIndirectBuffer = m_billboardIndirectCommands->lock( 0u, ashes::WholeSize, 0u );
			auto indirectBuffer = origIndirectBuffer;

			for ( auto & perPipeline : m_billboardNodes )
			{
				for ( auto & perBuffer : perPipeline.second )
				{
					auto & pipelineNodes = getPipelineNodes( perPipeline.first->getFlagsHash()
						, *perBuffer.first
						, m_nodesIds
						, nodesIdsBuffer
						, maxNodesCount );
					auto pipelinesBuffer = pipelineNodes.data();

					for ( auto & culled : perBuffer.second )
					{
						queuerndnd::fillIndirectCommand( *culled.node, indirectBuffer );
						( *pipelinesBuffer ) = culled.node->instance.getId( *culled.node->pass );
						++pipelinesBuffer;
						CU_Require( size_t( std::distance( origIndirectBuffer, indirectBuffer ) ) <= m_billboardIndirectCommands->getCount() );
						CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
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
		auto & rp = *queue.getOwner();
		uint32_t result{};

		ashes::CommandBuffer const & cb = queue.getCommandBuffer();
		cb.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( rp.getRenderPass( 0u )
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );

#if VK_NV_mesh_shader

		auto & submeshMshCommands = *m_submeshMeshletIndirectCommands;
		auto & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		uint32_t mshIndex{};
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		result += queuerndnd::doParseRenderNodesCommands( m_submeshNodes
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

#else

		auto & submeshIdxCommands = *m_submeshIdxIndirectCommands;
		auto & submeshNIdxCommands = *m_submeshNIdxIndirectCommands;
		uint32_t idxIndex{};
		uint32_t nidxIndex{};
		result += queuerndnd::doParseRenderNodesCommands( m_submeshNodes
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

#endif

		auto & billboardCommands = *m_billboardIndirectCommands;
		idxIndex = 0u;
		nidxIndex = 0u;
		result += queuerndnd::doParseRenderNodesCommands( m_billboardNodes
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

	std::pair< uint32_t, uint32_t > QueueRenderNodes::fillPipelinesIds( castor::ArrayView< uint32_t > nodesPipelinesIds )const
	{
		uint32_t maxId{};
		uint32_t count{};

		for ( auto & nodeIt : m_nodesPipelinesIds )
		{
			nodesPipelinesIds[nodeIt.first] = nodeIt.second;
			maxId = std::max( nodeIt.second, maxId );
			++count;
		}

		return { count, maxId };
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

	RenderPipeline & QueueRenderNodes::doGetPipeline( SubmeshRenderNode const & node
		, bool frontCulled )
	{
		auto & submesh = node.data;
		auto & pass = *node.pass;
		size_t hash = std::hash< Submesh const * >{}( &submesh );
		hash = castor::hashCombinePtr( hash, pass );
		hash = castor::hashCombine( hash, node.getInstanceCount() );
		hash = castor::hashCombine( hash, frontCulled );
		auto it = m_pipelines.find( hash );

		if ( it == m_pipelines.end() )
		{
			auto & renderPass = *getOwner()->getOwner();
			auto & scene = getOwner()->getCuller().getScene();
			auto material = pass.getOwner();
			auto pipelineFlags = renderPass.createPipelineFlags( pass
				, pass.getTexturesMask()
				, submesh.getSubmeshFlags( &pass )
				, submesh.getProgramFlags( *material )
				, scene.getFlags()
				, submesh.getTopology()
				, frontCulled
				, submesh.getMorphTargets() );

			if ( pipelineFlags.usesMesh() )
			{
				node.createMeshletDescriptorSet();
			}

			auto & result = frontCulled
				? renderPass.prepareFrontPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
					, node.getMeshletDescriptorLayout() )
				: renderPass.prepareBackPipeline( pipelineFlags
					, node.getGeometryBuffers( pipelineFlags ).layouts
					, node.getMeshletDescriptorLayout() );
			it = m_pipelines.emplace( hash, &result ).first;
		}

		return *it->second;
	}

	RenderPipeline & QueueRenderNodes::doGetPipeline( BillboardRenderNode const & node )
	{
		auto & billboard = node.data;
		auto & pass = *node.pass;
		size_t hash = std::hash< BillboardBase const * >{}( &billboard );
		hash = castor::hashCombinePtr( hash, pass );
		hash = castor::hashCombine( hash, node.getInstanceCount() );
		auto it = m_pipelines.find( hash );

		if ( it == m_pipelines.end() )
		{
			auto & renderPass = *getOwner()->getOwner();
			auto & scene = getOwner()->getCuller().getScene();
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
				, false
				, {} );
			auto & result = renderPass.prepareBackPipeline( pipelineFlags
				, billboard.getGeometryBuffers().layouts
				, nullptr );
			it = m_pipelines.emplace( hash, &result ).first;
		}

		return *it->second;
	}

	void QueueRenderNodes::doAddSubmesh( ShadowMapLightTypeArray & shadowMaps
		, SubmeshRenderNode const & node
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & pipeline = doGetPipeline( node, frontCulled );
		queuerndnd::addRenderNode( pipeline
			, node
			, node.getInstanceCount()
			, frontCulled
			, m_submeshNodes
			, m_nodesIds );
		renderPass.initialiseAdditionalDescriptor( pipeline
			, shadowMaps
			, node.data.getMorphTargets() );
		m_hasNodes = true;
	}

	void QueueRenderNodes::doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
		, SubmeshRenderNode const & node
		, bool frontCulled )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & pipeline = doGetPipeline( node, frontCulled );
		queuerndnd::addRenderNode( pipeline
			, node
			, node.getInstanceCount()
			, frontCulled
			, m_instancedSubmeshNodes
			, m_nodesIds );
		renderPass.initialiseAdditionalDescriptor( pipeline
			, shadowMaps
			, node.data.getMorphTargets() );
		m_hasNodes = true;
	}

	void QueueRenderNodes::doAddBillboard( ShadowMapLightTypeArray & shadowMaps
		, BillboardRenderNode const & node )
	{
		auto & renderPass = *getOwner()->getOwner();
		auto & pipeline = doGetPipeline( node );
		queuerndnd::addRenderNode( pipeline
			, node
			, node.getInstanceCount()
			, false
			, m_billboardNodes
			, m_nodesIds );
		renderPass.initialiseAdditionalDescriptor( pipeline
			, shadowMaps
			, {} );
		m_hasNodes = true;
	}

	//*********************************************************************************************
}
