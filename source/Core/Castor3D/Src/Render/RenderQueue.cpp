#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( disable:4503 )
#endif

#include "RenderQueue.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Render/RenderPass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
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

		template< typename NodeType, typename MapType >
		void doAddRenderNode( RenderPipeline & pipeline
			, NodeType && node
			, MapType & nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			ObjectRenderNodesArray tmp;
			auto itPipeline = nodes.emplace( &pipeline, std::move( tmp ) ).first;
			itPipeline->second.emplace_back( std::move( node ) );
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( Pass & pass
			, RenderPipeline & pipeline
			, NodeType && node
			, Submesh & object
			, MapType & nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap{} ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap{} ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray{} ).first;
			itObject->second.emplace_back( std::move( node ) );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene const & scene
			, String const & name )
		{
			AnimatedObjectSPtr result;
			auto & cache = scene.getAnimatedObjectGroupCache();
			auto lock = makeUniqueLock( cache );

			for ( auto group : cache )
			{
				if ( !result )
				{
					auto it = group.second->getObjects().find( name );

					if ( it != group.second->getObjects().end() )
					{
						result = it->second;
					}
				}
			}

			return result;
		}

		template< typename CreatorFunc, typename NodesType, typename ... Params >
		void doAddNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & pass
			, NodesType & nodes
			, CreatorFunc creator )
		{
			if ( pass.IsTwoSided()
				|| pass.hasAlphaBlending() )
			{
				auto pipeline = renderPass.getPipelineFront( pass.getColourBlendMode()
					, pass.getAlphaBlendMode()
					, pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					doAddRenderNode( *pipeline, creator( *pipeline ), nodes.frontCulled );
				}
			}

			auto pipeline = renderPass.getPipelineBack( pass.getColourBlendMode()
				, pass.getAlphaBlendMode()
				, pass.getAlphaFunc()
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags );

			if ( pipeline )
			{
				doAddRenderNode( *pipeline, creator( *pipeline ), nodes.backCulled );
			}
		}

		void doAddSkinningNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton
			, SceneRenderNodes::SkinnedNodesMap & animated
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & instanced )
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( pass.getColourBlendMode()
						, pass.getAlphaBlendMode()
						, pass.getAlphaFunc()
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags );

					if ( pipeline )
					{
						doAddRenderNode( pass
							, *pipeline
							, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
							, submesh
							, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( pass.getColourBlendMode()
					, pass.getAlphaBlendMode()
					, pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					doAddRenderNode( pass
						, *pipeline
						, renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton )
						, submesh
						, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, pass
					, animated
					, std::bind( &RenderPass::createSkinningNode
						, &renderPass
						, std::ref( pass )
						, std::placeholders::_1
						, std::ref( submesh )
						, std::ref( primitive )
						, std::ref( skeleton ) ) );
			}
		}

		void doAddMorphingNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh
			, SceneRenderNodes::MorphingNodesMap & animated )
		{
			doAddNode( renderPass
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, pass
				, animated
				, std::bind( &RenderPass::createMorphingNode
					, &renderPass
					, std::ref( pass )
					, std::placeholders::_1
					, std::ref( submesh )
					, std::ref( primitive )
					, std::ref( mesh ) ) );
		}

		void doAddStaticNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & pass
			, Submesh & submesh
			, Geometry & primitive
			, SceneRenderNodes::StaticNodesMap & statics
			, SceneRenderNodes::InstantiatedStaticNodesMap & instanced )
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( pass.hasAlphaBlending()
					|| pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( pass.getColourBlendMode()
						, pass.getAlphaBlendMode()
						, pass.getAlphaFunc()
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags );

					if ( pipeline )
					{
						auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
						doAddRenderNode( pass, *pipeline, node, submesh, instanced.frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( pass.getColourBlendMode()
					, pass.getAlphaBlendMode()
					, pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					auto node = renderPass.createStaticNode( pass, *pipeline, submesh, primitive );
					doAddRenderNode( pass, *pipeline, node, submesh, instanced.backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, pass
					, statics
					, std::bind( &RenderPass::createStaticNode
						, &renderPass
						, std::ref( pass )
						, std::placeholders::_1
						, std::ref( submesh )
						, std::ref( primitive ) ) );
			}
		}

		void doAddBillboardNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & pass
			, BillboardBase & billboard
			, SceneRenderNodes::BillboardNodesMap & nodes )
		{
			doAddNode( renderPass
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, pass
				, nodes
				, std::bind( &RenderPass::createBillboardNode
					, &renderPass
					, std::ref( pass )
					, std::placeholders::_1
					, std::ref( billboard ) ) );
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool opaque
			, SceneNode const * ignored
			, SceneRenderNodes & nodes )
		{
			nodes.staticNodes.frontCulled.clear();
			nodes.staticNodes.backCulled.clear();
			nodes.instancedStaticNodes.frontCulled.clear();
			nodes.instancedStaticNodes.backCulled.clear();
			nodes.skinnedNodes.frontCulled.clear();
			nodes.skinnedNodes.backCulled.clear();
			nodes.instancedSkinnedNodes.frontCulled.clear();
			nodes.instancedSkinnedNodes.backCulled.clear();
			nodes.morphingNodes.frontCulled.clear();
			nodes.morphingNodes.backCulled.clear();
			auto & scene = nodes.scene;

			bool shadows{ scene.hasShadows() };

			auto lock = makeUniqueLock( scene.getGeometryCache() );

			for ( auto primitive : scene.getGeometryCache() )
			{
				if ( ignored != primitive.second->getParent().get()
					&& primitive.second->getParent()->isVisible()
					&& primitive.second->getMesh() )
				{
					MeshSPtr mesh = primitive.second->getMesh();

					for ( auto submesh : *mesh )
					{
						MaterialSPtr material( primitive.second->getMaterial( *submesh ) );

						if ( material )
						{
							for ( auto pass : *material )
							{
								auto programFlags = submesh->getProgramFlags();
								auto sceneFlags = scene.getFlags();
								auto passFlags = pass->getPassFlags();
								auto submeshFlags = submesh->getProgramFlags();
								remFlag( programFlags, ProgramFlag::eSkinning );
								remFlag( programFlags, ProgramFlag::eMorphing );
								auto skeleton = std::static_pointer_cast< AnimatedSkeleton >( doFindAnimatedObject( scene, primitive.first + cuT( "_Skeleton" ) ) );
								auto mesh = std::static_pointer_cast< AnimatedMesh >( doFindAnimatedObject( scene, primitive.first + cuT( "_Mesh" ) ) );

								if ( skeleton && checkFlag( submeshFlags, ProgramFlag::eSkinning ) )
								{
									addFlag( programFlags, ProgramFlag::eSkinning );
								}

								if ( mesh )
								{
									addFlag( programFlags, ProgramFlag::eMorphing );
								}

								if ( !shadows
									|| !primitive.second->isShadowReceiver() )
								{
									remFlag( sceneFlags, SceneFlag::eShadowFilterPcf );
								}

								pass->prepareTextures();

								if ( checkFlag( submeshFlags, ProgramFlag::eInstantiation )
									&& !checkFlag( programFlags, ProgramFlag::eMorphing )
									&& ( !pass->hasAlphaBlending() || renderPass.isOrderIndependent() )
									&& renderPass.getEngine()->getRenderSystem()->getGpuInformations().hasInstancing()
									&& !pass->hasEnvironmentMapping() )
								{
									addFlag( programFlags, ProgramFlag::eInstantiation );
								}
								else
								{
									remFlag( programFlags, ProgramFlag::eInstantiation );
								}

								auto textureFlags = pass->getTextureFlags();
								renderPass.preparePipeline( pass->getColourBlendMode()
									, pass->getAlphaBlendMode()
									, pass->getAlphaFunc()
									, passFlags
									, textureFlags
									, programFlags
									, sceneFlags
									, pass->IsTwoSided() );

								if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != opaque )
								{
									if ( !isShadowMapProgram( programFlags )
										|| primitive.second->isShadowCaster() )
									{
										if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
										{
											doAddSkinningNode( renderPass
												, passFlags
												, textureFlags
												, programFlags
												, sceneFlags
												, *pass
												, *submesh
												, *primitive.second
												, *skeleton
												, nodes.skinnedNodes
												, nodes.instancedSkinnedNodes );
										}
										else if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
										{
											doAddMorphingNode( renderPass
												, passFlags
												, textureFlags
												, programFlags
												, sceneFlags
												, *pass
												, *submesh
												, *primitive.second
												, *mesh
												, nodes.morphingNodes );
										}
										else
										{
											doAddStaticNode( renderPass
												, passFlags
												, textureFlags
												, programFlags
												, sceneFlags
												, *pass
												, *submesh
												, *primitive.second
												, nodes.staticNodes
												, nodes.instancedStaticNodes );
										}
									}
								}
							}
						}
					}
				}
			}

			auto initialiseNodes = [&renderPass]( auto & pipelineNodes )
			{
				for ( auto & pipelineNode : pipelineNodes )
				{
					pipelineNode.first->createDescriptorPools( { uint32_t( pipelineNode.second.size() ) } );

					for ( auto & node : pipelineNode.second )
					{
						renderPass.initialiseDescriptor( pipelineNode.first->getDescriptorPool( 0u ), node );
					}
				}
			};

			initialiseNodes( nodes.staticNodes.frontCulled );
			initialiseNodes( nodes.staticNodes.backCulled );
			initialiseNodes( nodes.skinnedNodes.frontCulled );
			initialiseNodes( nodes.skinnedNodes.backCulled );
			initialiseNodes( nodes.morphingNodes.frontCulled );
			initialiseNodes( nodes.morphingNodes.backCulled );

			auto initialiseInstancedNodes = [&renderPass]( auto & pipelineNodes )
			{
				for ( auto & pipelineNode : pipelineNodes )
				{
					uint32_t size = 0u;

					for ( auto & passNodes : pipelineNode.second )
					{
						size += uint32_t( passNodes.second.size() );
					}

					pipelineNode.first->createDescriptorPools( { size } );

					for ( auto & passNodes : pipelineNode.second )
					{
						for ( auto & submeshNodes : passNodes.second )
						{
							renderPass.initialiseDescriptor( pipelineNode.first->getDescriptorPool( 0u ), submeshNodes.second[0] );
						}
					}
				}
			};

			initialiseInstancedNodes( nodes.instancedStaticNodes.frontCulled );
			initialiseInstancedNodes( nodes.instancedStaticNodes.backCulled );
			initialiseInstancedNodes( nodes.instancedSkinnedNodes.frontCulled );
			initialiseInstancedNodes( nodes.instancedSkinnedNodes.backCulled );
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool opaque
			, Scene const & scene
			, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & nodes )
		{
			bool shadows{ scene.hasShadows() };
			auto addNode = [&opaque, &renderPass, &scene, &nodes, &shadows]( Pass & p_pass
				, BillboardBase & billboard )
				{
					p_pass.prepareTextures();
					auto programFlags = billboard.getProgramFlags();
					auto sceneFlags = scene.getFlags();
					auto passFlags = p_pass.getPassFlags();
					addFlag( programFlags, ProgramFlag::eBillboards );

					if ( !shadows
						|| !billboard.isShadowReceiver() )
					{
						remFlag( sceneFlags, SceneFlag::eShadowFilterPcf );
					}

					auto textureFlags = p_pass.getTextureFlags();
					renderPass.preparePipeline( p_pass.getColourBlendMode()
						, p_pass.getAlphaBlendMode()
						, p_pass.getAlphaFunc()
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags
						, p_pass.IsTwoSided() );

					if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != opaque
						&& !isShadowMapProgram( programFlags ) )
					{
						doAddBillboardNode( renderPass
							, passFlags
							, textureFlags
							, programFlags
							, sceneFlags
							, p_pass
							, billboard
							, nodes );
					}
				};
			nodes.frontCulled.clear();
			nodes.backCulled.clear();
			{
				auto lock = makeUniqueLock( scene.getBillboardListCache() );

				for ( auto billboard : scene.getBillboardListCache() )
				{
					MaterialSPtr material( billboard.second->getMaterial() );
					REQUIRE( material );

					for ( auto pass : *material )
					{
						addNode( *pass, *billboard.second );
					}
				}
			}
			{
				auto lock = makeUniqueLock( scene.getParticleSystemCache() );

				for ( auto particleSystem : scene.getParticleSystemCache() )
				{
					MaterialSPtr material( particleSystem.second->getMaterial() );
					REQUIRE( material );

					for ( auto pass : *material )
					{
						addNode( *pass, *particleSystem.second->getBillboards() );
					}
				}
			}
		}

		template< typename NodeType >
		void doAddRenderNodeCommand( RenderPipeline & pipeline
			, NodeType const & node
			, renderer::CommandBuffer const & commandBuffer )
		{
			GeometryBuffers const & geometryBuffers = node.data.getGeometryBuffers();

			commandBuffer.bindPipeline( pipeline.getPipeline() );
			commandBuffer.bindDescriptorSet( *node.descriptorSet, pipeline.getPipelineLayout() );
			commandBuffer.bindVertexBuffers( 0u, geometryBuffers.vbo, geometryBuffers.vboOffsets );

			if ( geometryBuffers.ibo )
			{
				commandBuffer.bindIndexBuffer( *geometryBuffers.ibo, geometryBuffers.iboOffset, renderer::IndexType::eUInt32 );
				commandBuffer.drawIndexed( geometryBuffers.idxCount );
			}
			else
			{
				commandBuffer.draw( geometryBuffers.vtxCount );
			}
		}

		template< typename NodeType >
		void doAddRenderNodeCommand( Pass & pass
			, RenderPipeline & pipeline
			, NodeType const & node
			, Submesh & object
			, renderer::CommandBuffer const & commandBuffer )
		{
			GeometryBuffers const & geometryBuffers = object.getGeometryBuffers();

			commandBuffer.bindPipeline( pipeline.getPipeline() );
			commandBuffer.bindDescriptorSet( *node.descriptorSet, pipeline.getPipelineLayout() );
			commandBuffer.bindVertexBuffers( 0u, geometryBuffers.vbo, geometryBuffers.vboOffsets );

			if ( geometryBuffers.ibo )
			{
				commandBuffer.bindIndexBuffer( *geometryBuffers.ibo, geometryBuffers.iboOffset, renderer::IndexType::eUInt32 );
				commandBuffer.drawIndexed( geometryBuffers.idxCount );
			}
			else
			{
				commandBuffer.draw( geometryBuffers.vtxCount );
			}
		}

		template< typename MapType, typename ArrayType >
		void doAddRenderNodes( Camera const & camera
			, renderer::CommandBuffer const & commandBuffer
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, ArrayType & renderNodes )
		{
			for ( auto const & node : renderNodes )
			{
				if ( node.sceneNode.isDisplayable()
					&& node.sceneNode.isVisible()
					&& camera.isVisible( node.instance, node.data ) )
				{
					doAddRenderNodeCommand( pass, pipeline, node, submesh, commandBuffer );
				}
			}
		}

		template< typename MapType >
		void doParseRenderNodes( Camera const & camera
			, MapType const & inputNodes
			, renderer::CommandBuffer const & commandBuffer )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.sceneNode.isDisplayable()
						&& node.sceneNode.isVisible()
						&& camera.isVisible( node.instance, node.data ) )
					{
						doAddRenderNodeCommand( *pipelines.first, node, commandBuffer );
					}
				}
			}
		}

		template<>
		void doParseRenderNodes( Camera const & camera
			, BillboardRenderNodesByPipelineMap const & inputNodes
			, renderer::CommandBuffer const & commandBuffer )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.sceneNode.isDisplayable()
						 && node.sceneNode.isVisible() )
					{
						doAddRenderNodeCommand( *pipelines.first, node, commandBuffer );
					}
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderPass & renderPass
		, bool opaque
		, SceneNode const * ignored )
		: OwnedBy< RenderPass >{ renderPass }
		, m_opaque{ opaque }
		, m_ignoredNode{ ignored }
	{
	}

	void RenderQueue::initialise( Scene const & scene
		, Camera & camera )
	{
		m_commandBuffer = getOwner()->getEngine()->getRenderSystem()->getMainDevice().getGraphicsCommandPool().createCommandBuffer( false );
		initialise( scene );
		m_cameraChanged = camera.onChanged.connect( std::bind( &RenderQueue::onCameraChanged
			, this
			, std::placeholders::_1 ) );
		m_camera = &camera;
		onCameraChanged( camera );
	}

	void RenderQueue::initialise( Scene const & scene )
	{
		m_sceneChanged = scene.onChanged.connect( std::bind( &RenderQueue::onSceneChanged
			, this
			, std::placeholders::_1 ) );
		onSceneChanged( scene );
		m_renderNodes = std::make_unique< SceneRenderNodes >( scene );
	}

	void RenderQueue::update()
	{
		if ( m_isSceneChanged )
		{
			doSortRenderNodes();
			m_isSceneChanged = false;
			m_changed = true;
		}

		if ( m_changed )
		{
			if ( m_camera )
			{
				doPrepareCulledNodesCommandBuffer();
			}
			else
			{
				doPrepareAllNodesCommandBuffer();
			}

			m_changed = false;
		}
	}
	
	void RenderQueue::doPrepareCulledNodesCommandBuffer()
	{
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eRenderPassContinue ) )
		{
			auto & camera = *m_camera;
			camera.update();

			doTraverseNodes( m_renderNodes->instancedStaticNodes.frontCulled
				, std::bind( doAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
					, std::ref( camera )
					, std::ref( *m_commandBuffer )
					, std::placeholders::_1
					, std::placeholders::_2
					, std::placeholders::_3
					, std::placeholders::_4 ) );

			doTraverseNodes( m_renderNodes->instancedStaticNodes.backCulled
				, std::bind( doAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
					, std::ref( camera )
					, std::ref( *m_commandBuffer )
					, std::placeholders::_1
					, std::placeholders::_2
					, std::placeholders::_3
					, std::placeholders::_4 ) );

			doTraverseNodes( m_renderNodes->instancedSkinnedNodes.frontCulled
				, std::bind( doAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
					, std::ref( camera )
					, std::ref( *m_commandBuffer )
					, std::placeholders::_1
					, std::placeholders::_2
					, std::placeholders::_3
					, std::placeholders::_4 ) );

			doTraverseNodes( m_renderNodes->instancedSkinnedNodes.backCulled
				, std::bind( doAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
					, std::ref( camera )
					, std::ref( *m_commandBuffer )
					, std::placeholders::_1
					, std::placeholders::_2
					, std::placeholders::_3
					, std::placeholders::_4 ) );

			doParseRenderNodes( camera
				, m_renderNodes->staticNodes.frontCulled
				, *m_commandBuffer );
			doParseRenderNodes( camera
				, m_renderNodes->staticNodes.backCulled
				, *m_commandBuffer );

			doParseRenderNodes( camera
				, m_renderNodes->skinnedNodes.frontCulled
				, *m_commandBuffer );
			doParseRenderNodes( camera
				, m_renderNodes->skinnedNodes.backCulled
				, *m_commandBuffer );

			doParseRenderNodes( camera
				, m_renderNodes->morphingNodes.frontCulled
				, *m_commandBuffer );
			doParseRenderNodes( camera
				, m_renderNodes->morphingNodes.backCulled
				, *m_commandBuffer );

			doParseRenderNodes( camera
				, m_renderNodes->billboardNodes.frontCulled
				, *m_commandBuffer );
			doParseRenderNodes( camera
				, m_renderNodes->billboardNodes.backCulled
				, *m_commandBuffer );
		}
	}

	void RenderQueue::doSortRenderNodes()
	{
		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, m_ignoredNode
			, *m_renderNodes );
		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, m_renderNodes->scene
			, m_renderNodes->billboardNodes );
	}

	void RenderQueue::onSceneChanged( Scene const & scene )
	{
		m_isSceneChanged = true;
	}

	void RenderQueue::onCameraChanged( Camera const & camera )
	{
		m_changed = true;
	}
}
