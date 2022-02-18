#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, QueueCulledRenderNodes )

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	namespace
	{
		uint32_t getInstanceMult( Submesh const & data
			, uint32_t instanceMult )
		{
			return instanceMult;
		}

		uint32_t getInstanceMult( BillboardBase const & data
			, uint32_t instanceMult )
		{
			return data.getCount();
		}

		void fillIndirectCommand( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			if ( node.buffers.bufferOffset.idxBuffer )
			{
				indirectIndexedCommands->indexCount = node.buffers.bufferOffset.getIndexCount();
				indirectIndexedCommands->instanceCount = getInstanceMult( node.data, instanceCount );
				indirectIndexedCommands->firstIndex = node.buffers.bufferOffset.getFirstIndex();
				indirectIndexedCommands->vertexOffset = int32_t( node.buffers.bufferOffset.getFirstVertex() );
				indirectIndexedCommands->firstInstance = 0u;
				++indirectIndexedCommands;
			}
			else
			{
				indirectCommands->vertexCount = node.buffers.bufferOffset.getVertexCount();
				indirectCommands->instanceCount = getInstanceMult( node.data, instanceCount );
				indirectCommands->firstVertex = node.buffers.bufferOffset.getFirstVertex();
				indirectCommands->firstInstance = 0u;
				++indirectCommands;
			}
		}

		void fillIndirectCommand( BillboardRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			indirectCommands->vertexCount = uint32_t( node.buffers.bufferOffset.vtxChunk.size / sizeof( BillboardBase::Vertex ) );
			indirectCommands->instanceCount = getInstanceMult( node.data, instanceCount );
			indirectCommands->firstVertex = node.buffers.bufferOffset.getFirstVertex();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT * node
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, NodePtrArrayT< NodeT >{} ).first->second;
			pipelineMap.emplace_back( node );
		}

		template< typename NodeT >
		void doParseRenderNodes( NodeByPipelineMapT< NodeT > & inputNodes
			, NodePtrByPipelineMapT< NodeT > & outputNodes
			, SceneCuller::CulledInstancesPtrT< NodeCulledT< NodeT > > const & culledNodes
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					auto it = std::find( culledNodes.objects.begin()
						, culledNodes.objects.end()
						, node.first );

					if ( it != culledNodes.objects.end() )
					{
						fillIndirectCommand( *node.second
							, indirectIndexedCommands
							, indirectCommands
							, instanceCount );
						doAddRenderNode( *pipelines.first
							, node.second
							, outputNodes );
					}
				}
			}
		}

		template< typename NodeT >
		void doAddInstantiatedRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeT * node
			, Submesh & object
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, ObjectNodesPtrByPassT< NodeT >{} ).first->second;
			auto & passMap = pipelineMap.emplace( &pass, ObjectNodesPtrMapT< NodeT >{} ).first->second;
			auto & obbjectMap = passMap.emplace( &object, NodePtrArrayT< NodeT >{} ).first->second;
			obbjectMap.emplace_back( node );
		}

		template< typename NodeT >
		void doParseInstantiatedRenderNodes( ObjectNodesPtrByPipelineMapT< NodeT > & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, NodeMapT< NodeT > & renderNodes
			, SceneCuller::CulledInstancesPtrT< CulledSubmesh > const & culledNodes
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			instanceCount *= uint32_t( renderNodes.size() );
			fillIndirectCommand( *renderNodes.begin()->second
				, indirectIndexedCommands
				, indirectCommands
				, instanceCount );

			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.objects.begin()
					, culledNodes.objects.end()
					, node.first );

				if ( it != culledNodes.objects.end() )
				{
					doAddInstantiatedRenderNode( pass
						, pipeline
						, node.second, ( *it )->data
						, outputNodes );
				}
			}
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doAddGeometryNodeCommands( RenderPipeline & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & index )
		{
			GeometryBuffers const & geometryBuffers = node.buffers;

			if ( node.uboDescriptorSet )
			{
				commandBuffer.bindDescriptorSet( *node.uboDescriptorSet, pipeline.getPipelineLayout() );
			}

			if ( node.texDescriptorSet )
			{
				commandBuffer.bindDescriptorSet( *node.texDescriptorSet, pipeline.getPipelineLayout() );
			}

			if ( pipeline.hasDescriptorSetLayout() )
			{
				commandBuffer.bindDescriptorSet( pipeline.getAdditionalDescriptorSet( node ), pipeline.getPipelineLayout() );
			}

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
					, index * sizeof( VkDrawIndexedIndirectCommand )
					, 1u
					, sizeof( VkDrawIndexedIndirectCommand ) );
			}
			else
			{
				commandBuffer.drawIndirect( indirectCommands.getBuffer()
					, index * sizeof( VkDrawIndirectCommand )
					, 1u
					, sizeof( VkDrawIndirectCommand ) );
			}

			++index;
		}

		void doBindPipeline( ashes::CommandBuffer const & commandBuffer
			, RenderPipeline const & pipeline
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

			if ( pipeline.getRenderSystem().hasFeature( GpuFeature::eBindless ) )
			{
				auto ds = pipeline.getRenderSystem().getEngine()->getTextureUnitCache().getDescriptorSet();
				commandBuffer.bindDescriptorSet( *ds, pipeline.getPipelineLayout() );
			}
		}

		template< typename NodeT >
		void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & index )
		{
			for ( auto & pipelines : inputNodes )
			{
				RenderPipeline & pipeline = *pipelines.first;
				doBindPipeline( commandBuffer, pipeline, viewport, scissor );

				for ( auto & node : pipelines.second )
				{
					doAddGeometryNodeCommands( pipeline
						, *node
						, commandBuffer
						, indirectIndexedCommands
						, indirectCommands
						, index );
				}
			}
		}

		template< typename NodeT >
		void doParseInstantiatedRenderNodesCommands( ashes::CommandBuffer const & commandBuffer
			, RenderPipeline & pipeline
			, NodePtrArrayT< NodeT > & renderNodes
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & index )
		{
			doAddGeometryNodeCommands( pipeline
				, *renderNodes[0]
				, commandBuffer
				, indirectIndexedCommands
				, indirectCommands
				, index );
		}

		template< typename NodeMapT
			, typename OnPipelineFuncT
			, typename OnSumbeshFuncT >
		void doTraverseNodes( NodeMapT & nodes
			, OnPipelineFuncT onPipeline
			, OnSumbeshFuncT onSubmesh )
		{
			for ( auto & itPipelines : nodes )
			{
				onPipeline( *itPipelines.first );

				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						onSubmesh( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeMapT
			, typename OnSumbeshFuncT >
		void doTraverseNodes( NodeMapT & nodes
			, OnSumbeshFuncT onSubmesh )
		{
			doTraverseNodes( nodes
				, []( RenderPipeline const & pipeline )
				{
				}
			, onSubmesh );
		}
	}

	//*************************************************************************************************

	QueueCulledRenderNodes::QueueCulledRenderNodes( RenderQueue const & queue )
		: castor::OwnedBy< RenderQueue const >{ queue }
		, m_indirectIndexedCommands{ makeBuffer< VkDrawIndexedIndirectCommand >( queue.getOwner()->getEngine()->getRenderSystem()->getRenderDevice()
			, 250'000ull
			, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, queue.getOwner()->getName() + "IndirectIndexedBuffer" ) }
		, m_indirectCommands{ makeBuffer< VkDrawIndirectCommand >( queue.getOwner()->getEngine()->getRenderSystem()->getRenderDevice()
			, 250'000ull
			, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, queue.getOwner()->getName() + "IndirectBuffer" ) }
	{
	}

	void QueueCulledRenderNodes::parse()
	{
		auto & queue = *getOwner();
		auto & culler = queue.getOwner()->getCuller();

		auto & allNodes = queue.getAllRenderNodes();
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
		auto indexedBuffer = m_indirectIndexedCommands->lock( 0u, ashes::WholeSize, 0u );
		auto buffer = m_indirectCommands->lock( 0u, ashes::WholeSize, 0u );
		auto & rp = *queue.getOwner();
		auto instanceCount = rp.getInstanceMult();

		doTraverseNodes( allNodes.instancedStaticNodes.frontCulled
			, [this, &queue, &culler, &indexedBuffer, &buffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				doParseInstantiatedRenderNodes( instancedStaticNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() )
					, indexedBuffer
					, buffer
					, instanceCount );
			} );
		doTraverseNodes( allNodes.instancedStaticNodes.backCulled
			, [this, &queue, &culler, &indexedBuffer, &buffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				doParseInstantiatedRenderNodes( instancedStaticNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() )
					, indexedBuffer
					, buffer
					, instanceCount );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.frontCulled
			, [this, &queue, &culler, &indexedBuffer, &buffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				doParseInstantiatedRenderNodes( instancedSkinnedNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() )
					, indexedBuffer
					, buffer
					, instanceCount );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.backCulled
			, [this, &queue, &culler, &indexedBuffer, &buffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				doParseInstantiatedRenderNodes( instancedSkinnedNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() )
					, indexedBuffer
					, buffer
					, instanceCount );
			} );

		doParseRenderNodes( allNodes.staticNodes.frontCulled
			, staticNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );
		doParseRenderNodes( allNodes.staticNodes.backCulled
			, staticNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );

		doParseRenderNodes( allNodes.skinnedNodes.frontCulled
			, skinnedNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );
		doParseRenderNodes( allNodes.skinnedNodes.backCulled
			, skinnedNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );

		doParseRenderNodes( allNodes.morphingNodes.frontCulled
			, morphingNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );
		doParseRenderNodes( allNodes.morphingNodes.backCulled
			, morphingNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );

		doParseRenderNodes( allNodes.billboardNodes.frontCulled
			, billboardNodes.frontCulled
			, culler.getCulledBillboards( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );
		doParseRenderNodes( allNodes.billboardNodes.backCulled
			, billboardNodes.backCulled
			, culler.getCulledBillboards( queue.getMode() )
			, indexedBuffer
			, buffer
			, instanceCount );

		m_indirectCommands->flush( 0u, ashes::WholeSize );
		m_indirectCommands->unlock();
		m_indirectIndexedCommands->flush( 0u, ashes::WholeSize );
		m_indirectIndexedCommands->unlock();
	}

	void QueueCulledRenderNodes::prepareCommandBuffers( RenderQueue const & queue
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors )
	{
		ashes::CommandBuffer const & cb = queue.getCommandBuffer();
		auto & rp = *queue.getOwner();
		cb.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( rp.getRenderPass()
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );
		uint32_t index{};

		doTraverseNodes( instancedStaticNodes.frontCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [this, &cb, &index]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, nodes
					, *m_indirectIndexedCommands
					, *m_indirectCommands
					, index );
			} );
		doTraverseNodes( instancedStaticNodes.backCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [this, &cb, &index]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, nodes
					, *m_indirectIndexedCommands
					, *m_indirectCommands
					, index );
			} );
		doTraverseNodes( instancedSkinnedNodes.frontCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [this, &cb, &index]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, nodes
					, *m_indirectIndexedCommands
					, *m_indirectCommands
					, index );
			} );
		doTraverseNodes( instancedSkinnedNodes.backCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [this, &cb, &index]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, nodes
					, *m_indirectIndexedCommands
					, *m_indirectCommands
					, index );
			} );

		doParseRenderNodesCommands( staticNodes.frontCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( staticNodes.backCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( skinnedNodes.frontCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( skinnedNodes.backCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( morphingNodes.frontCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( morphingNodes.backCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( billboardNodes.frontCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( billboardNodes.backCulled
			, cb
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		cb.end();
	}

	bool QueueCulledRenderNodes::hasNodes()const
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
	//*************************************************************************************************
}
