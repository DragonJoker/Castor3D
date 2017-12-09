#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( disable:4503 )
#endif

#include "RenderQueue.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
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
			for ( auto itPipelines : nodes )
			{
				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
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
			auto itPipeline = nodes.insert( { &pipeline, ObjectRenderNodesArray() } ).first;
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

			auto itPipeline = nodes.emplace( &pipeline, ObjectRenderNodesByPipelineMap() ).first;
			auto itPass = itPipeline->second.emplace( &pass, ObjectRenderNodesByPassMap() ).first;
			auto itObject = itPass->second.emplace( &object, ObjectRenderNodesArray() ).first;
			itObject->second.emplace_back( std::move( node ) );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene & scene
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
					auto node = creator( *pipeline );
					doAddRenderNode( *pipeline, node, nodes.m_frontCulled );
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
				auto node = creator( *pipeline );
				doAddRenderNode( *pipeline, node, nodes.m_backCulled );
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
						auto node = renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton );
						doAddRenderNode( pass, *pipeline, node, submesh, instanced.m_frontCulled );
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
					auto node = renderPass.createSkinningNode( pass, *pipeline, submesh, primitive, skeleton );
					doAddRenderNode( pass, *pipeline, node, submesh, instanced.m_backCulled );
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
						doAddRenderNode( pass, *pipeline, node, submesh, instanced.m_frontCulled );
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
					doAddRenderNode( pass, *pipeline, node, submesh, instanced.m_backCulled );
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
			, Scene & scene
			, SceneRenderNodes::StaticNodesMap & statics
			, SceneRenderNodes::InstantiatedStaticNodesMap & instanced
			, SceneRenderNodes::SkinnedNodesMap & skinning
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & instancedSkinning
			, SceneRenderNodes::MorphingNodesMap & morphing )
		{
			statics.m_frontCulled.clear();
			statics.m_backCulled.clear();
			instanced.m_frontCulled.clear();
			instanced.m_backCulled.clear();
			skinning.m_frontCulled.clear();
			skinning.m_backCulled.clear();
			instancedSkinning.m_frontCulled.clear();
			instancedSkinning.m_backCulled.clear();
			morphing.m_frontCulled.clear();
			morphing.m_backCulled.clear();

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
												, skinning
												, instancedSkinning );
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
												, morphing );
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
												, statics
												, instanced );
										}
									}
								}
							}
						}
					}
				}
			}
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool opaque
			, Scene & scene
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
			nodes.m_frontCulled.clear();
			nodes.m_backCulled.clear();
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

		template< typename MapType, typename ArrayType >
		void doAddRenderNodes( Camera const & camera
			, MapType & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, ArrayType & renderNodes )
		{
			for ( auto const & node : renderNodes )
			{
				if ( node.m_sceneNode.isDisplayable()
					&& node.m_sceneNode.isVisible()
					&& camera.isVisible( node.m_data.getBoundingSphere()
						, node.m_sceneNode.getDerivedPosition()
						, node.m_sceneNode.getDerivedScale() )
					&& camera.isVisible( node.m_data.getBoundingBox()
						, node.m_sceneNode.getDerivedTransformationMatrix() ) )
				{
					doAddRenderNode( pass, pipeline, node, submesh, outputNodes );
				}
			}
		}

		void doAddRenderNodes( Camera const & camera
			, SubmeshSkinningRenderNodesByPipelineMap & outputNodes
			, RenderPipeline & pipeline
			, Pass & pass
			, Submesh & submesh
			, SkinningRenderNodeArray & renderNodes )
		{
			for ( auto const & node : renderNodes )
			{
				if ( node.m_sceneNode.isDisplayable()
					&& node.m_sceneNode.isVisible()
					&& camera.isVisible( node.m_instance.getBoundingSphere( node.m_data )
						, node.m_sceneNode.getDerivedPosition()
						, node.m_sceneNode.getDerivedScale() )
					&& camera.isVisible( node.m_instance.getBoundingBox( node.m_data )
						, node.m_sceneNode.getDerivedTransformationMatrix() ) )
				{
					doAddRenderNode( pass, pipeline, node, submesh, outputNodes );
				}
			}
		}

		template< typename MapType >
		void doParseRenderNodes( Camera const & camera
			, MapType const & inputNodes
			, MapType & outputNodes )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.m_sceneNode.isDisplayable()
						&& node.m_sceneNode.isVisible()
						&& camera.isVisible( node.m_data.getBoundingSphere()
							, node.m_sceneNode.getDerivedPosition()
							, node.m_sceneNode.getDerivedScale() )
						&& camera.isVisible( node.m_data.getBoundingBox()
							, node.m_sceneNode.getDerivedTransformationMatrix() ) )
					{
						doAddRenderNode( *pipelines.first, node, outputNodes );
					}
				}
			}
		}

		template<>
		void doParseRenderNodes( Camera const & camera
			, BillboardRenderNodesByPipelineMap const & inputNodes
			, BillboardRenderNodesByPipelineMap & outputNodes )
		{
			for ( auto & pipelines : inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.m_sceneNode.isDisplayable()
						 && node.m_sceneNode.isVisible() )
					{
						doAddRenderNode( *pipelines.first, node, outputNodes );
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
		, m_ignored{ ignored }
	{
	}

	void RenderQueue::initialise( Scene & scene
		, Camera & camera )
	{
		initialise( scene );
		m_cameraChanged = camera.onChanged.connect( std::bind( &RenderQueue::onCameraChanged
			, this
			, std::placeholders::_1 ) );
		m_camera = &camera;
		onCameraChanged( camera );
		m_preparedRenderNodes = std::make_unique< SceneRenderNodes >( scene );
	}

	void RenderQueue::initialise( Scene & scene )
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
				doPrepareRenderNodes();
			}

			m_changed = false;
		}
	}
	
	SceneRenderNodes & RenderQueue::getRenderNodes()const
	{
		if ( m_camera )
		{
			REQUIRE( m_preparedRenderNodes );
			return *m_preparedRenderNodes;
		}

		REQUIRE( m_renderNodes );
		return *m_renderNodes;
	}

	void RenderQueue::doPrepareRenderNodes()
	{
		m_preparedRenderNodes->m_instantiatedStaticNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_instantiatedStaticNodes.m_frontCulled.clear();
		m_preparedRenderNodes->m_staticNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_staticNodes.m_frontCulled.clear();
		m_preparedRenderNodes->m_skinnedNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_skinnedNodes.m_frontCulled.clear();
		m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_frontCulled.clear();
		m_preparedRenderNodes->m_morphingNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_morphingNodes.m_frontCulled.clear();
		m_preparedRenderNodes->m_billboardNodes.m_backCulled.clear();
		m_preparedRenderNodes->m_billboardNodes.m_frontCulled.clear();

		auto & camera = *m_camera;
		camera.update();

		doTraverseNodes( m_renderNodes->m_instantiatedStaticNodes.m_frontCulled
			, std::bind( doAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedStaticNodes.m_frontCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		doTraverseNodes( m_renderNodes->m_instantiatedStaticNodes.m_backCulled
			, std::bind( doAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedStaticNodes.m_backCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		doTraverseNodes( m_renderNodes->m_instantiatedSkinnedNodes.m_frontCulled
			, std::bind( doAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_frontCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		doTraverseNodes( m_renderNodes->m_instantiatedSkinnedNodes.m_backCulled
			, std::bind( doAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_backCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		doParseRenderNodes( camera
			, m_renderNodes->m_staticNodes.m_frontCulled
			, m_preparedRenderNodes->m_staticNodes.m_frontCulled );
		doParseRenderNodes( camera
			, m_renderNodes->m_staticNodes.m_backCulled
			, m_preparedRenderNodes->m_staticNodes.m_backCulled );

		doParseRenderNodes( camera
			, m_renderNodes->m_skinnedNodes.m_frontCulled
			, m_preparedRenderNodes->m_skinnedNodes.m_frontCulled );
		doParseRenderNodes( camera
			, m_renderNodes->m_skinnedNodes.m_backCulled
			, m_preparedRenderNodes->m_skinnedNodes.m_backCulled );

		doParseRenderNodes( camera
			, m_renderNodes->m_morphingNodes.m_frontCulled
			, m_preparedRenderNodes->m_morphingNodes.m_frontCulled );
		doParseRenderNodes( camera
			, m_renderNodes->m_morphingNodes.m_backCulled
			, m_preparedRenderNodes->m_morphingNodes.m_backCulled );

		doParseRenderNodes( camera
			, m_renderNodes->m_billboardNodes.m_frontCulled
			, m_preparedRenderNodes->m_billboardNodes.m_frontCulled );
		doParseRenderNodes( camera
			, m_renderNodes->m_billboardNodes.m_backCulled
			, m_preparedRenderNodes->m_billboardNodes.m_backCulled );
	}

	void RenderQueue::doSortRenderNodes()
	{
		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, m_ignored
			, m_renderNodes->m_scene
			, m_renderNodes->m_staticNodes
			, m_renderNodes->m_instantiatedStaticNodes
			, m_renderNodes->m_skinnedNodes
			, m_renderNodes->m_instantiatedSkinnedNodes
			, m_renderNodes->m_morphingNodes );
		castor3d::doSortRenderNodes( *getOwner()
			, m_opaque
			, m_renderNodes->m_scene
			, m_renderNodes->m_billboardNodes );
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
