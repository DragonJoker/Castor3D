#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"

#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
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

#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>

using namespace castor;

using ashes::operator==;
using ashes::operator!=;

//*************************************************************************************************

namespace castor
{
	void Deleter< castor3d::SceneCulledRenderNodes >::operator()( castor3d::SceneCulledRenderNodes * ptr )noexcept
	{
		delete ptr;
	}
}

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		template< typename NodeType, typename MapType >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeType * node
			, MapType & nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			ObjectRenderNodesArray tmp;
			auto itPipeline = nodes.emplace( &pipeline, std::move( tmp ) ).first;
			itPipeline->second.emplace_back( node );
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeType * node
			, Submesh & object
			, MapType & nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap{} ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap{} ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray{} ).first;
			itObject->second.emplace_back( node );
		}

		template< typename MapType
			, typename CulledMapType
			, typename CulledT >
		void doParseRenderNodes( MapType & inputNodes
			, CulledMapType & outputNodes
			, SceneCuller::CulledInstancesPtrT< CulledT > const & culledNodes )
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
						doAddRenderNode( *pipelines.first, &node.second, outputNodes );
					}
				}
			}
		}

		template< typename CulledMapType
			, typename AllMapType >
		void doParseRenderNodes( CulledMapType & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, AllMapType & renderNodes
			, SceneCuller::CulledInstancesPtrT< CulledSubmesh > const & culledNodes )
		{
			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.objects.begin()
					, culledNodes.objects.end()
					, node.first );

				if ( it != culledNodes.objects.end() )
				{
					doAddRenderNode( pass, pipeline, &node.second, ( *it )->data, outputNodes );
				}
			}
		}

		//*****************************************************************************************

		GeometryBuffers const & getGeometryBuffers( Submesh const & submesh
			, MaterialSPtr material
			, uint32_t instanceCount )
		{
			return submesh.getGeometryBuffers( material, instanceCount );
		}

		GeometryBuffers const & getGeometryBuffers( BillboardBase const & billboard
			, MaterialSPtr material
			, uint32_t instanceCount )
		{
			return billboard.getGeometryBuffers();
		}

		template< typename NodeType >
		void doAddRenderNodeCommands( RenderPipeline & pipeline
			, NodeType const & node
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceCount )
		{
			if ( instanceCount )
			{
				GeometryBuffers const & geometryBuffers = getGeometryBuffers( node.data
					, node.passNode.pass.getOwner()->shared_from_this()
					, instanceCount );

				commandBuffer.bindPipeline( pipeline.getPipeline() );

				if ( viewport )
				{
					commandBuffer.setViewport( *viewport );
				}

				if ( scissor )
				{
					commandBuffer.setScissor( *scissor );
				}

				commandBuffer.bindDescriptorSet( *node.uboDescriptorSet, pipeline.getPipelineLayout() );

				if ( node.texDescriptorSet )
				{
					commandBuffer.bindDescriptorSet( *node.texDescriptorSet, pipeline.getPipelineLayout() );
				}

				for ( uint32_t i = 0; i < geometryBuffers.vbo.size(); ++i )
				{
					commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i].get().vertexBindingDescriptions[0].binding
						, geometryBuffers.vbo[i]
						, geometryBuffers.vboOffsets[i] );
				}

				if ( geometryBuffers.ibo )
				{
					commandBuffer.bindIndexBuffer( *geometryBuffers.ibo
						, geometryBuffers.iboOffset
						, VK_INDEX_TYPE_UINT32 );
					commandBuffer.drawIndexed( geometryBuffers.idxCount
						, instanceCount );
				}
				else
				{
					commandBuffer.draw( geometryBuffers.vtxCount
						, instanceCount );
				}
			}
		}

		template< typename NodeType >
		void doAddRenderNodeCommands( Pass & pass
			, RenderPipeline & pipeline
			, NodeType const & node
			, Submesh & object
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceCount )
		{
			if ( instanceCount )
			{
				GeometryBuffers const & geometryBuffers = object.getGeometryBuffers( pass.getOwner()->shared_from_this()
					, instanceCount );

				commandBuffer.bindPipeline( pipeline.getPipeline() );

				if ( viewport )
				{
					commandBuffer.setViewport( *viewport );
				}

				if ( scissor )
				{
					commandBuffer.setScissor( *scissor );
				}

				commandBuffer.bindDescriptorSet( *node.uboDescriptorSet, pipeline.getPipelineLayout() );

				if ( node.texDescriptorSet )
				{
					commandBuffer.bindDescriptorSet( *node.texDescriptorSet, pipeline.getPipelineLayout() );
				}

				for ( uint32_t i = 0; i < geometryBuffers.vbo.size(); ++i )
				{
					commandBuffer.bindVertexBuffer( geometryBuffers.layouts[i].get().vertexBindingDescriptions[0].binding
						, geometryBuffers.vbo[i]
						, geometryBuffers.vboOffsets[i] );
				}

				if ( geometryBuffers.ibo )
				{
					commandBuffer.bindIndexBuffer( *geometryBuffers.ibo
						, geometryBuffers.iboOffset
						, VK_INDEX_TYPE_UINT32 );
					commandBuffer.drawIndexed( geometryBuffers.idxCount
						, instanceCount );
				}
				else
				{
					commandBuffer.draw( geometryBuffers.vtxCount
						, instanceCount );
				}
			}
		}

		template< typename CulledMapType >
		void doParseRenderNodesCommands( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, CulledMapType & renderNodes
			, uint32_t instanceMult )
		{
			doAddRenderNodeCommands( pass
				, pipeline
				, *renderNodes[0]
				, submesh
				, commandBuffer
				, viewport
				, scissor
				, uint32_t( renderNodes.size() * instanceMult ) );
		}

		template< typename MapType >
		void doParseRenderNodesCommands( MapType & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceMult )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					doAddRenderNodeCommands( *pipelines.first
						, *node
						, commandBuffer
						, viewport
						, scissor
						, instanceMult );
				}
			}
		}

		template<>
		void doParseRenderNodesCommands( BillboardRenderNodesPtrByPipelineMap & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, uint32_t instanceMult )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto & node : pipelines.second )
				{
					doAddRenderNodeCommands( *pipelines.first
						, *node
						, commandBuffer
						, viewport
						, scissor
						, node->instance.getCount() );
				}
			}
		}

		template< typename MapType, typename FuncType >
		void doTraverseNodes( MapType & nodes
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );
					}
				}
			}
		}
	}

	//*************************************************************************************************

	void SceneCulledRenderNodes::parse( RenderQueue const & queue )
	{
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

		doTraverseNodes( allNodes.instancedStaticNodes.frontCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodes( instancedStaticNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedStaticNodes.backCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodes( instancedStaticNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.frontCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodes( instancedSkinnedNodes.frontCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );
		doTraverseNodes( allNodes.instancedSkinnedNodes.backCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodes( instancedSkinnedNodes.backCulled
					, pipeline
					, pass
					, nodes
					, culler.getCulledSubmeshes( queue.getMode() ) );
			} );

		doParseRenderNodes( allNodes.staticNodes.frontCulled
			, staticNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );
		doParseRenderNodes( allNodes.staticNodes.backCulled
			, staticNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );

		doParseRenderNodes( allNodes.skinnedNodes.frontCulled
			, skinnedNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );
		doParseRenderNodes( allNodes.skinnedNodes.backCulled
			, skinnedNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );

		doParseRenderNodes( allNodes.morphingNodes.frontCulled
			, morphingNodes.frontCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );
		doParseRenderNodes( allNodes.morphingNodes.backCulled
			, morphingNodes.backCulled
			, culler.getCulledSubmeshes( queue.getMode() ) );

		doParseRenderNodes( allNodes.billboardNodes.frontCulled
			, billboardNodes.frontCulled
			, culler.getCulledBillboards( queue.getMode() ) );
		doParseRenderNodes( allNodes.billboardNodes.backCulled
			, billboardNodes.backCulled
			, culler.getCulledBillboards( queue.getMode() ) );
	}

	void SceneCulledRenderNodes::prepareCommandBuffers( RenderQueue const & queue
		, ashes::Optional< VkViewport > const & viewport
		, ashes::Optional< VkRect2D > const & scissors )
	{
		queue.getCommandBuffer().begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( VkRenderPass( queue.getOwner()->getRenderPass() )
				, 0u
				, VkFramebuffer( VK_NULL_HANDLE )
				, VkBool32( VK_FALSE )
				, 0u
				, 0u ) );

		doTraverseNodes( instancedStaticNodes.frontCulled
			, [&queue, &viewport, &scissors]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodesCommands( queue.getCommandBuffer()
					, viewport
					, scissors
					, pipeline
					, pass
					, submesh
					, nodes
					, queue.getOwner()->getInstanceMult() );
			} );
		doTraverseNodes( instancedStaticNodes.backCulled
			, [&queue, &viewport, &scissors]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodesCommands< StaticRenderNodePtrArray >( queue.getCommandBuffer()
					, viewport
					, scissors
					, pipeline
					, pass
					, submesh
					, nodes
					, queue.getOwner()->getInstanceMult() );
			} );
		doTraverseNodes( instancedSkinnedNodes.frontCulled
			, [&queue, &viewport, &scissors]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodesCommands< SkinningRenderNodePtrArray >( queue.getCommandBuffer()
					, viewport
					, scissors
					, pipeline
					, pass
					, submesh
					, nodes
					, queue.getOwner()->getInstanceMult() );
			} );
		doTraverseNodes( instancedSkinnedNodes.backCulled
			, [&queue, &viewport, &scissors]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, auto & nodes )
			{
				doParseRenderNodesCommands< SkinningRenderNodePtrArray >( queue.getCommandBuffer()
					, viewport
					, scissors
					, pipeline
					, pass
					, submesh
					, nodes
					, queue.getOwner()->getInstanceMult() );
			} );

		doParseRenderNodesCommands( staticNodes.frontCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );
		doParseRenderNodesCommands( staticNodes.backCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );

		doParseRenderNodesCommands( skinnedNodes.frontCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );
		doParseRenderNodesCommands( skinnedNodes.backCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );

		doParseRenderNodesCommands( morphingNodes.frontCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );
		doParseRenderNodesCommands( morphingNodes.backCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );

		doParseRenderNodesCommands( billboardNodes.frontCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );
		doParseRenderNodesCommands( billboardNodes.backCulled
			, queue.getCommandBuffer()
			, viewport
			, scissors
			, queue.getOwner()->getInstanceMult() );

		queue.getCommandBuffer().end();
	}

	//*************************************************************************************************
}
