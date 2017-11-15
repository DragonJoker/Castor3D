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
		void doTraverseNodes( MapType & p_nodes, FuncType p_function )
		{
			for ( auto itPipelines : p_nodes )
			{
				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						p_function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( RenderPipeline & p_pipeline
			, NodeType && p_node
			, MapType & p_nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			auto itPipeline = p_nodes.insert( { &p_pipeline, ObjectRenderNodesArray() } ).first;
			itPipeline->second.emplace_back( std::move( p_node ) );
		}

		template< typename NodeType, typename MapType >
		void doAddRenderNode( Pass & p_pass
			, RenderPipeline & p_pipeline
			, NodeType && p_node
			, Submesh & p_object
			, MapType & p_nodes )
		{
			using ObjectRenderNodesByPipelineMap = typename MapType::mapped_type;
			using ObjectRenderNodesByPassMap = typename ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = typename ObjectRenderNodesByPassMap::mapped_type;

			auto itPipeline = p_nodes.emplace( &p_pipeline, ObjectRenderNodesByPipelineMap() ).first;
			auto itPass = itPipeline->second.emplace( &p_pass, ObjectRenderNodesByPassMap() ).first;
			auto itObject = itPass->second.emplace( &p_object, ObjectRenderNodesArray() ).first;
			itObject->second.emplace_back( std::move( p_node ) );
		}

		AnimatedObjectSPtr doFindAnimatedObject( Scene & scene, String const & p_name )
		{
			AnimatedObjectSPtr result;
			auto & cache = scene.getAnimatedObjectGroupCache();
			auto lock = makeUniqueLock( cache );

			for ( auto group : cache )
			{
				if ( !result )
				{
					auto it = group.second->getObjects().find( p_name );

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
			, Pass & p_pass
			, NodesType & p_nodes
			, CreatorFunc p_creator )
		{
			if ( p_pass.IsTwoSided()
				|| p_pass.hasAlphaBlending() )
			{
				auto pipeline = renderPass.getPipelineFront( p_pass.getColourBlendMode()
					, p_pass.getAlphaBlendMode()
					, p_pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					auto node = p_creator( *pipeline );
					doAddRenderNode( *pipeline, node, p_nodes.m_frontCulled );
				}
			}

			auto pipeline = renderPass.getPipelineBack( p_pass.getColourBlendMode()
				, p_pass.getAlphaBlendMode()
				, p_pass.getAlphaFunc()
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags );

			if ( pipeline )
			{
				auto node = p_creator( *pipeline );
				doAddRenderNode( *pipeline, node, p_nodes.m_backCulled );
			}
		}

		void doAddSkinningNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedSkeleton & p_skeleton
			, SceneRenderNodes::SkinnedNodesMap & p_animated
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & p_instanced )
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( p_pass.hasAlphaBlending() 
					|| p_pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( p_pass.getColourBlendMode()
						, p_pass.getAlphaBlendMode()
						, p_pass.getAlphaFunc()
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags );

					if ( pipeline )
					{
						auto node = renderPass.createSkinningNode( p_pass, *pipeline, p_submesh, p_primitive, p_skeleton );
						doAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( p_pass.getColourBlendMode()
					, p_pass.getAlphaBlendMode()
					, p_pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					auto node = renderPass.createSkinningNode( p_pass, *pipeline, p_submesh, p_primitive, p_skeleton );
					doAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, p_pass
					, p_animated
					, std::bind( &RenderPass::createSkinningNode
						, &renderPass
						, std::ref( p_pass )
						, std::placeholders::_1
						, std::ref( p_submesh )
						, std::ref( p_primitive )
						, std::ref( p_skeleton ) ) );
			}
		}

		void doAddMorphingNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedMesh & p_mesh
			, SceneRenderNodes::MorphingNodesMap & p_animated )
		{
			doAddNode( renderPass
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, p_pass
				, p_animated
				, std::bind( &RenderPass::createMorphingNode
					, &renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_submesh )
					, std::ref( p_primitive )
					, std::ref( p_mesh ) ) );
		}

		void doAddStaticNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, SceneRenderNodes::StaticNodesMap & p_static
			, SceneRenderNodes::InstantiatedStaticNodesMap & p_instanced )
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( p_pass.hasAlphaBlending() 
					|| p_pass.IsTwoSided() )
				{
					auto pipeline = renderPass.getPipelineFront( p_pass.getColourBlendMode()
						, p_pass.getAlphaBlendMode()
						, p_pass.getAlphaFunc()
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags );

					if ( pipeline )
					{
						auto node = renderPass.createStaticNode( p_pass, *pipeline, p_submesh, p_primitive );
						doAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_frontCulled );
					}
				}

				auto pipeline = renderPass.getPipelineBack( p_pass.getColourBlendMode()
					, p_pass.getAlphaBlendMode()
					, p_pass.getAlphaFunc()
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags );

				if ( pipeline )
				{
					auto node = renderPass.createStaticNode( p_pass, *pipeline, p_submesh, p_primitive );
					doAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_backCulled );
				}
			}
			else
			{
				doAddNode( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, p_pass
					, p_static
					, std::bind( &RenderPass::createStaticNode
						, &renderPass
						, std::ref( p_pass )
						, std::placeholders::_1
						, std::ref( p_submesh )
						, std::ref( p_primitive ) ) );
			}
		}

		void doAddBillboardNode( RenderPass & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, Pass & p_pass
			, BillboardBase & p_billboard
			, SceneRenderNodes::BillboardNodesMap & p_nodes )
		{
			doAddNode( renderPass
				, passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, p_pass
				, p_nodes
				, std::bind( &RenderPass::createBillboardNode
					, &renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_billboard ) ) );
		}

		void doSortRenderNodes( RenderPass & renderPass
			, bool p_opaque
			, SceneNode const * ignored
			, Scene & scene
			, SceneRenderNodes::StaticNodesMap & p_static
			, SceneRenderNodes::InstantiatedStaticNodesMap & p_instanced
			, SceneRenderNodes::SkinnedNodesMap & p_skinning
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & p_instancedSkinning
			, SceneRenderNodes::MorphingNodesMap & p_morphing )
		{
			p_static.m_frontCulled.clear();
			p_static.m_backCulled.clear();
			p_instanced.m_frontCulled.clear();
			p_instanced.m_backCulled.clear();
			p_skinning.m_frontCulled.clear();
			p_skinning.m_backCulled.clear();
			p_instancedSkinning.m_frontCulled.clear();
			p_instancedSkinning.m_backCulled.clear();
			p_morphing.m_frontCulled.clear();
			p_morphing.m_backCulled.clear();

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

								if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != p_opaque )
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
												, p_skinning
												, p_instancedSkinning );
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
												, p_morphing );
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
												, p_static
												, p_instanced );
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
			, bool p_opaque
			, Scene & scene
			, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & p_nodes )
		{
			bool shadows{ scene.hasShadows() };
			auto addNode = [&p_opaque, &renderPass, &scene, &p_nodes, &shadows]( Pass & p_pass
				, BillboardBase & p_billboard )
			{
				p_pass.prepareTextures();
				auto programFlags = p_billboard.getProgramFlags();
				auto sceneFlags = scene.getFlags();
				auto passFlags = p_pass.getPassFlags();
				addFlag( programFlags, ProgramFlag::eBillboards );

				if ( !shadows
					|| !p_billboard.isShadowReceiver() )
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

				if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != p_opaque
					&& !isShadowMapProgram( programFlags ) )
				{
					doAddBillboardNode( renderPass
						, passFlags
						, textureFlags
						, programFlags
						, sceneFlags
						, p_pass
						, p_billboard
						, p_nodes );
				}
			};
			p_nodes.m_frontCulled.clear();
			p_nodes.m_backCulled.clear();
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
			, MapType & p_outputNodes
			, RenderPipeline & p_pipeline
			, Pass & p_pass
			, Submesh & p_submesh
			, ArrayType & p_renderNodes )
		{
			for ( auto const & node : p_renderNodes )
			{
				if ( node.m_sceneNode.isDisplayable()
					&& node.m_sceneNode.isVisible()
					&& camera.isVisible( node.m_data.getBoundingSphere()
						, node.m_sceneNode.getDerivedPosition()
						, node.m_sceneNode.getDerivedScale() )
					&& camera.isVisible( node.m_data.getBoundingBox()
						, node.m_sceneNode.getDerivedTransformationMatrix() ) )
				{
					doAddRenderNode( p_pass, p_pipeline, node, p_submesh, p_outputNodes );
				}
			}
		}

		template< typename MapType >
		void doParseRenderNodes( Camera const & camera
			, MapType const & p_inputNodes
			, MapType & p_outputNodes )
		{
			for ( auto & pipelines : p_inputNodes )
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
						doAddRenderNode( *pipelines.first, node, p_outputNodes );
					}
				}
			}
		}

		template<>
		void doParseRenderNodes( Camera const & camera
			, BillboardRenderNodesByPipelineMap const & p_inputNodes
			, BillboardRenderNodesByPipelineMap & p_outputNodes )
		{
			for ( auto & pipelines : p_inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.m_sceneNode.isDisplayable()
						 && node.m_sceneNode.isVisible() )
					{
						doAddRenderNode( *pipelines.first, node, p_outputNodes );
					}
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderPass & renderPass
		, bool p_opaque
		, SceneNode const * ignored )
		: OwnedBy< RenderPass >{ renderPass }
		, m_opaque{ p_opaque }
		, m_ignored{ ignored }
	{
	}

	void RenderQueue::initialise( Scene & scene, Camera & camera )
	{
		initialise( scene );
		m_cameraChanged = camera.onChanged.connect( std::bind( &RenderQueue::onCameraChanged, this, std::placeholders::_1 ) );
		m_camera = &camera;
		onCameraChanged( camera );
		m_preparedRenderNodes = std::make_unique< SceneRenderNodes >( scene );
	}

	void RenderQueue::initialise( Scene & scene )
	{
		m_sceneChanged = scene.onChanged.connect( std::bind( &RenderQueue::onSceneChanged, this, std::placeholders::_1 ) );
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
