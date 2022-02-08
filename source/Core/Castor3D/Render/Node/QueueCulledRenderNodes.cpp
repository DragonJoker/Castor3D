#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"

#include "Castor3D/Engine.hpp"
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
			, SceneCuller::CulledInstancesPtrT< NodeCulledT< NodeT > > const & culledNodes )
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
						doAddRenderNode( *pipelines.first, node.second, outputNodes );
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
			, SceneCuller::CulledInstancesPtrT< CulledSubmesh > const & culledNodes )
		{
			for ( auto & node : renderNodes )
			{
				auto it = std::find( culledNodes.objects.begin()
					, culledNodes.objects.end()
					, node.first );

				if ( it != culledNodes.objects.end() )
				{
					doAddInstantiatedRenderNode( pass, pipeline, node.second, ( *it )->data, outputNodes );
				}
			}
		}

		//*****************************************************************************************

		GeometryBuffers const & getGeometryBuffers( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, Submesh const & submesh
			, Pass const & pass
			, uint32_t instanceCount
			, bool forceTexcoords )
		{
			return submesh.getGeometryBuffers( shaderFlags
				, programFlags
				, pass.getOwner()
				, instanceCount
				, pass.getTexturesMask()
				, forceTexcoords );
		}

		GeometryBuffers const & getGeometryBuffers( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, BillboardBase const & billboard
			, Pass const & pass
			, uint32_t instanceCount
			, bool forceTexcoords )
		{
			return billboard.getGeometryBuffers();
		}

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

		template< typename NodeT >
		void doAddGeometryNodeCommands( RenderPipeline & pipeline
			, NodeT const & node
			, ashes::CommandBuffer const & commandBuffer
			, GeometryBuffers const & geometryBuffers
			, uint32_t instanceCount )
		{
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

		template< typename NodeT, typename InstanceT >
		void doAddGeometryNodeCommands( RenderPipeline & pipeline
			, ShaderFlags const & shaderFlags
			, NodeT const & node
			, Pass & pass
			, InstanceT & instance
			, ashes::CommandBuffer const & commandBuffer
			, uint32_t instanceCount )
		{
			GeometryBuffers const & geometryBuffers = getGeometryBuffers( shaderFlags
				, pipeline.getFlags().programFlags
				, instance
				, pass
				, instanceCount
				, checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eForceTexCoords ) );
			doAddGeometryNodeCommands( pipeline
				, node
				, commandBuffer
				, geometryBuffers
				, instanceCount );
		}

		template< typename NodeT >
		void doParseRenderNodesCommands( NodePtrByPipelineMapT< NodeT > & inputNodes
			, ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissor
			, ShaderFlags const & shaderFlags
			, uint32_t instanceMult )
		{
			if ( instanceMult )
			{
				for ( auto & pipelines : inputNodes )
				{
					RenderPipeline & pipeline = *pipelines.first;
					doBindPipeline( commandBuffer, pipeline, viewport, scissor );

					for ( auto & node : pipelines.second )
					{
						doAddGeometryNodeCommands( pipeline
							, shaderFlags
							, *node
							, node->passNode.pass
							, node->data
							, commandBuffer
							, getInstanceMult( node->data, instanceMult ) );
					}
				}
			}
		}

		template< typename NodeT >
		void doParseInstantiatedRenderNodesCommands( ashes::CommandBuffer const & commandBuffer
			, RenderPipeline & pipeline
			, ShaderFlags const & shaderFlags
			, Pass & pass
			, Submesh & submesh
			, NodePtrArrayT< NodeT > & renderNodes
			, uint32_t instanceMult )
		{
			auto instanceCount = uint32_t( renderNodes.size() * instanceMult );

			if ( instanceCount )
			{
				doAddGeometryNodeCommands( pipeline
					, shaderFlags
					, *renderNodes[0]
					, pass
					, submesh
					, commandBuffer
					, instanceCount );
			}
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

		doTraverseNodes( allNodes.instancedStaticNodes.frontCulled
			, [this, &queue, &culler]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodeMap & nodes )
			{
				doParseInstantiatedRenderNodes( instancedStaticNodes.frontCulled
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
				doParseInstantiatedRenderNodes( instancedStaticNodes.backCulled
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
				doParseInstantiatedRenderNodes( instancedSkinnedNodes.frontCulled
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
				doParseInstantiatedRenderNodes( instancedSkinnedNodes.backCulled
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

		doTraverseNodes( instancedStaticNodes.frontCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [&cb, &rp]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, rp.getShaderFlags()
					, pass
					, submesh
					, nodes
					, rp.getInstanceMult() );
			} );
		doTraverseNodes( instancedStaticNodes.backCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [&cb, &rp]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, rp.getShaderFlags()
					, pass
					, submesh
					, nodes
					, rp.getInstanceMult() );
			} );
		doTraverseNodes( instancedSkinnedNodes.frontCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [&cb, &rp]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, rp.getShaderFlags()
					, pass
					, submesh
					, nodes
					, rp.getInstanceMult() );
			} );
		doTraverseNodes( instancedSkinnedNodes.backCulled
			, [&cb, &viewport, &scissors]( RenderPipeline const & pipeline )
			{
				doBindPipeline( cb, pipeline, viewport, scissors );
			}
			, [&cb, &rp]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SubmeshRenderNodePtrArray & nodes )
			{
				doParseInstantiatedRenderNodesCommands( cb
					, pipeline
					, rp.getShaderFlags()
					, pass
					, submesh
					, nodes
					, rp.getInstanceMult() );
			} );

		doParseRenderNodesCommands( staticNodes.frontCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );
		doParseRenderNodesCommands( staticNodes.backCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );

		doParseRenderNodesCommands( skinnedNodes.frontCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );
		doParseRenderNodesCommands( skinnedNodes.backCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );

		doParseRenderNodesCommands( morphingNodes.frontCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );
		doParseRenderNodesCommands( morphingNodes.backCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );

		doParseRenderNodesCommands( billboardNodes.frontCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );
		doParseRenderNodesCommands( billboardNodes.backCulled
			, cb
			, viewport
			, scissors
			, rp.getShaderFlags()
			, rp.getInstanceMult() );

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
