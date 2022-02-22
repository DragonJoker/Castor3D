#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
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
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderWriter/Source.hpp>

#pragma warning( disable:4189 )
#pragma warning( disable:5245 )

CU_ImplementCUSmartPtr( castor3d, QueueCulledRenderNodes )

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	namespace
	{
		//*****************************************************************************************

		template< typename NodeMapT
			, typename OnPipelineFuncT
			, typename OnSubmeshFuncT >
			void doTraverseNodes( NodeMapT & nodes
				, OnPipelineFuncT onPipeline
				, OnSubmeshFuncT onSubmesh )
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
			, typename OnSubmeshFuncT >
			void doTraverseNodes( NodeMapT & nodes
				, OnSubmeshFuncT onSubmesh )
		{
			doTraverseNodes( nodes
				, []( RenderPipeline const & pipeline )
				{
				}
				, onSubmesh );
		}

		void registerPipelineNodes( RenderPipeline const & pipeline
			, std::vector< RenderPipeline const * > & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [&pipeline]( RenderPipeline const * lookup )
				{
					return lookup == &pipeline;
				} );

			if ( it == cont.end() )
			{
				cont.emplace_back( &pipeline );
			}
		}

		uint32_t getPipelineNodesIndex( RenderPipeline const & pipeline
			, std::vector< RenderPipeline const * > const & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [&pipeline]( RenderPipeline const * lookup )
				{
					return lookup == &pipeline;
				} );
			CU_Require( it != cont.end() );
			return uint32_t( std::distance( cont.begin(), it ) );
		}

		PipelineNodes & getPipelineNodes( RenderPipeline const & pipeline
			, std::vector< RenderPipeline const * > const & cont
			, PipelineNodes * nodes )
		{
			auto index = getPipelineNodesIndex( pipeline, cont );
			return nodes[index];
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeT * node
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			auto & pipelineMap = nodes.emplace( &pipeline, NodePtrArrayT< NodeT >{} ).first->second;
			pipelineMap.emplace_back( node );
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
			auto & objectMap = passMap.emplace( &object, NodePtrArrayT< NodeT >{} ).first->second;
			objectMap.emplace_back( node );
		}

		template< typename NodeT >
		void doParseInstantiatedRenderNodes( NodePtrByPipelineMapT< NodeT > & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, NodeMapT< NodeT > & renderNodes
			, SceneCuller::CulledInstancesPtrT< NodeCulledT< NodeT > > const & culledNodes )
		{
			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.objects.begin()
					, culledNodes.objects.end()
					, node.first );

				if ( it != culledNodes.objects.end() )
				{
					doAddRenderNode( pipeline
						, node.second
						, outputNodes );
				}
			}
		}

		template< typename NodeT >
		void doParseInstantiatedRenderNodes( ObjectNodesPtrByPipelineMapT< NodeT > & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, NodeMapT< NodeT > & renderNodes
			, SceneCuller::CulledInstancesPtrT< NodeCulledT< NodeT > > const & culledNodes )
		{
			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.objects.begin()
					, culledNodes.objects.end()
					, node.first );

				if ( it != culledNodes.objects.end() )
				{
					doAddInstantiatedRenderNode( pass
						, pipeline
						, node.second
						, ( *it )->data
						, outputNodes );
				}
			}
		}

		template< typename NodeT >
		void doParseRenderNodes( NodeByPipelineMapT< NodeT > & inputNodes
			, NodePtrByPipelineMapT< NodeT > & outputNodes
			, SceneCuller::CulledInstancesPtrT< NodeCulledT< NodeT > > const & culledNodes
			, std::vector< RenderPipeline const * > & registeredPipelines )
		{
			for ( auto & pipelines : inputNodes )
			{
				registerPipelineNodes( *pipelines.first, registeredPipelines );

				for ( auto & node : pipelines.second )
				{
					auto it = std::find( culledNodes.objects.begin()
						, culledNodes.objects.end()
						, node.first );

					if ( it != culledNodes.objects.end() )
					{
						doAddRenderNode( *pipelines.first
							, node.second
							, outputNodes );
					}
				}
			}
		}

		//*****************************************************************************************

		template< typename NodeT >
		using NodeVertexT = typename NodeT::VertexType;

		template< typename NodeT >
		void fillIndirectCommand( NodeT const & node
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			using VertexT = NodeVertexT< NodeT >;

			if ( node.buffers.bufferOffset.idxBuffer )
			{
				indirectIndexedCommands->indexCount = node.buffers.bufferOffset.getIndexCount();
				indirectIndexedCommands->instanceCount = node.getInstanceMult( instanceCount );
				indirectIndexedCommands->firstIndex = node.buffers.bufferOffset.getFirstIndex();
				indirectIndexedCommands->vertexOffset = int32_t( node.buffers.bufferOffset.getFirstVertex< VertexT >() );
				indirectIndexedCommands->firstInstance = 0u;
				++indirectIndexedCommands;
			}
			else
			{
				indirectCommands->vertexCount = node.buffers.bufferOffset.getVertexCount< VertexT >();
				indirectCommands->instanceCount = node.getInstanceMult( instanceCount );
				indirectCommands->firstVertex = node.buffers.bufferOffset.getFirstVertex< VertexT >();
				indirectCommands->firstInstance = 0u;
				++indirectCommands;
			}
		}

		template< typename NodeT = SubmeshRenderNode >
		void doFillRenderNodes( NodePtrByPipelineMapT< NodeT > & renderNodes
			, std::vector< RenderPipeline const * > const & registeredPipelines
			, PipelineNodes *& pipelinesNodes
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			for ( auto & pipelines : renderNodes )
			{
				RenderPipeline const & pipeline = *pipelines.first;
				auto & pipelineNodes = getPipelineNodes( pipeline, registeredPipelines, pipelinesNodes );
				auto pipelinesBuffer = pipelineNodes.data();

				for ( auto & node : pipelines.second )
				{
					fillIndirectCommand( *node
						, indirectIndexedCommands
						, indirectCommands
						, instanceCount );
					( *pipelinesBuffer )->x = node->getId() - 1u;

					if constexpr ( std::is_same_v< NodeT, SubmeshRenderNode > )
					{
						if ( node->mesh )
						{
							( *pipelinesBuffer )->y = node->mesh->getId() - 1u;
						}

						if ( node->skeleton )
						{
							( *pipelinesBuffer )->z = node->skeleton->getId() - 1u;
						}
					}

					++pipelinesBuffer;
				}
			}
		}

		void doFillInstantiatedRenderNodes( NodePtrArrayT< SubmeshRenderNode > & renderNodes
			, std::vector< RenderPipeline const * > const & registeredPipelines
			, PipelineNodes *& pipelinesNodes
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			instanceCount *= uint32_t( renderNodes.size() );
			fillIndirectCommand( **renderNodes.begin()
				, indirectIndexedCommands
				, indirectCommands
				, instanceCount );
		}

		//*****************************************************************************************

		template< typename NodeT >
		void doAddGeometryNodeCommands( RenderPipeline & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & index
			, uint32_t & drawId )
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
				commandBuffer.bindDescriptorSet( pipeline.getAdditionalDescriptorSet(), pipeline.getPipelineLayout() );
			}

			commandBuffer.pushConstants( pipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 4u, 4u, &drawId );
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
			++drawId;
		}

		void doBindPipeline( ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, std::vector< RenderPipeline const * > const & registeredPipelines
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
				commandBuffer.bindDescriptorSet( *pipeline.getOwner()->getCuller().getScene().getBindlessTexDescriptorSet()
					, pipeline.getPipelineLayout() );

				auto pipelineId = getPipelineNodesIndex( pipeline, registeredPipelines );
				commandBuffer.pushConstants( pipeline.getPipelineLayout()
					, VK_SHADER_STAGE_VERTEX_BIT
					, 0u
					, 4u
					, &pipelineId );
			}
		}

		template< typename NodeT >
		void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, RenderNodesPass const & nodesPass
			, std::vector< RenderPipeline const * > const & registeredPipelines
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & indirectIndexedCommands
			, ashes::Buffer< VkDrawIndirectCommand > const & indirectCommands
			, uint32_t & index )
		{

			for ( auto & pipelines : inputNodes )
			{
				RenderPipeline & pipeline = *pipelines.first;
				doBindPipeline( commandBuffer
					, nodesPass
					, registeredPipelines
					, pipeline
					, viewport
					, scissor );
				uint32_t drawId = 0u;

				for ( auto & node : pipelines.second )
				{
					doAddGeometryNodeCommands( pipeline
						, *node
						, commandBuffer
						, indirectIndexedCommands
						, indirectCommands
						, index
						, drawId );
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
			uint32_t drawId = 0u;

			for ( auto & node : renderNodes )
			{
				doAddGeometryNodeCommands( pipeline
					, *node
					, commandBuffer
					, indirectIndexedCommands
					, indirectCommands
					, index
					, drawId );
			}
		}

		//*****************************************************************************************
	}

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
		, m_nodesIds{ makeBuffer< PipelineNodes >( queue.getOwner()->getEngine()->getRenderSystem()->getRenderDevice()
			, 1'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, queue.getOwner()->getName() + "PipelinesNodesIDs" ) }
	{
	}

	void QueueCulledRenderNodes::parse()
	{
		auto & queue = *getOwner();
		auto & culler = queue.getOwner()->getCuller();

		auto & allNodes = queue.getAllRenderNodes();
		staticNodes.backCulled.clear();
		staticNodes.frontCulled.clear();
		skinnedNodes.backCulled.clear();
		skinnedNodes.frontCulled.clear();
		morphingNodes.backCulled.clear();
		morphingNodes.frontCulled.clear();
		billboardNodes.backCulled.clear();
		billboardNodes.frontCulled.clear();
		m_pipelinesNodes.clear();
		auto & rp = *queue.getOwner();

		doTraverseNodes( allNodes.instancedStaticNodes.frontCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				registerPipelineNodes( pipeline, m_pipelinesNodes );
				doParseInstantiatedRenderNodes( staticNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedStaticNodes.backCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				registerPipelineNodes( pipeline, m_pipelinesNodes );
				doParseInstantiatedRenderNodes( staticNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.frontCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				registerPipelineNodes( pipeline, m_pipelinesNodes );
				doParseInstantiatedRenderNodes( skinnedNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.backCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				registerPipelineNodes( pipeline, m_pipelinesNodes );
				doParseInstantiatedRenderNodes( skinnedNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );

		doParseRenderNodes( allNodes.staticNodes.frontCulled
			, staticNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );
		doParseRenderNodes( allNodes.staticNodes.backCulled
			, staticNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );

		doParseRenderNodes( allNodes.skinnedNodes.frontCulled
			, skinnedNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );
		doParseRenderNodes( allNodes.skinnedNodes.backCulled
			, skinnedNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );

		doParseRenderNodes( allNodes.morphingNodes.frontCulled
			, morphingNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );
		doParseRenderNodes( allNodes.morphingNodes.backCulled
			, morphingNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() )
			, m_pipelinesNodes );

		doParseRenderNodes( allNodes.billboardNodes.frontCulled
			, billboardNodes.frontCulled
			, culler.getCulledBillboards( queue.getMode() )
			, m_pipelinesNodes );
		doParseRenderNodes( allNodes.billboardNodes.backCulled
			, billboardNodes.backCulled
			, culler.getCulledBillboards( queue.getMode() )
			, m_pipelinesNodes );
	}

	void QueueCulledRenderNodes::fillIndirect()
	{
		auto & queue = *getOwner();
		auto & rp = *queue.getOwner();
		auto instanceCount = rp.getInstanceMult();
		auto indIndexedBuffer = m_indirectIndexedCommands->lock( 0u, ashes::WholeSize, 0u );
		auto indBuffer = m_indirectCommands->lock( 0u, ashes::WholeSize, 0u );
		auto nodesIdsBuffer = m_nodesIds->lock( 0u, ashes::WholeSize, 0u );

		doTraverseNodes( instancedStaticNodes.frontCulled
			, [this, &nodesIdsBuffer, &indIndexedBuffer, &indBuffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doFillInstantiatedRenderNodes( nodes
					, m_pipelinesNodes
					, nodesIdsBuffer
					, indIndexedBuffer
					, indBuffer
					, instanceCount );
			} );
		doTraverseNodes( instancedStaticNodes.backCulled
			, [this, &nodesIdsBuffer, &indIndexedBuffer, &indBuffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doFillInstantiatedRenderNodes( nodes
					, m_pipelinesNodes
					, nodesIdsBuffer
					, indIndexedBuffer
					, indBuffer
					, instanceCount );
			} );
		doTraverseNodes( instancedSkinnedNodes.frontCulled
			, [this, &nodesIdsBuffer, &indIndexedBuffer, &indBuffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doFillInstantiatedRenderNodes( nodes
					, m_pipelinesNodes
					, nodesIdsBuffer
					, indIndexedBuffer
					, indBuffer
					, instanceCount );
			} );
		doTraverseNodes( instancedSkinnedNodes.backCulled
			, [this, &nodesIdsBuffer, &indIndexedBuffer, &indBuffer, instanceCount]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doFillInstantiatedRenderNodes( nodes
					, m_pipelinesNodes
					, nodesIdsBuffer
					, indIndexedBuffer
					, indBuffer
					, instanceCount );
			} );

		doFillRenderNodes( staticNodes.frontCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );
		doFillRenderNodes( staticNodes.backCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );

		doFillRenderNodes( skinnedNodes.frontCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );
		doFillRenderNodes( skinnedNodes.backCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );

		doFillRenderNodes( morphingNodes.frontCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );
		doFillRenderNodes( morphingNodes.backCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );

		doFillRenderNodes( billboardNodes.frontCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );
		doFillRenderNodes( billboardNodes.backCulled
			, m_pipelinesNodes
			, nodesIdsBuffer
			, indIndexedBuffer
			, indBuffer
			, instanceCount );

		m_nodesIds->flush( 0u, ashes::WholeSize );
		m_nodesIds->unlock();
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
			, [this, &cb, &rp, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, rp, m_pipelinesNodes, pipeline, viewport, scissors );
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
			, [this, &cb, &rp, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, rp, m_pipelinesNodes, pipeline, viewport, scissors );
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
			, [this, &cb, &rp, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, rp, m_pipelinesNodes, pipeline, viewport, scissors );
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
			, [this, &cb, &rp, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, rp, m_pipelinesNodes, pipeline, viewport, scissors );
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
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( staticNodes.backCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( skinnedNodes.frontCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( skinnedNodes.backCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( morphingNodes.frontCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( morphingNodes.backCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );

		doParseRenderNodesCommands( billboardNodes.frontCulled
			, cb
			, rp
			, m_pipelinesNodes
			, viewport
			, scissors
			, *m_indirectIndexedCommands
			, *m_indirectCommands
			, index );
		doParseRenderNodesCommands( billboardNodes.backCulled
			, cb
			, rp
			, m_pipelinesNodes
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
