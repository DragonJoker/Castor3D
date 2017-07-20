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

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename MapType, typename FuncType >
		void DoTraverseNodes( MapType & p_nodes, FuncType p_function )
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
		void DoAddRenderNode( RenderPipeline & p_pipeline
			, NodeType && p_node
			, MapType & p_nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			auto itPipeline = p_nodes.insert( { &p_pipeline, ObjectRenderNodesArray() } ).first;
			itPipeline->second.emplace_back( std::move( p_node ) );
		}

		template< typename NodeType, typename MapType >
		void DoAddRenderNode( Pass & p_pass
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

		AnimatedObjectSPtr DoFindAnimatedObject( Scene & p_scene, String const & p_name )
		{
			AnimatedObjectSPtr result;
			auto & cache = p_scene.GetAnimatedObjectGroupCache();
			auto lock = make_unique_lock( cache );

			for ( auto group : cache )
			{
				if ( !result )
				{
					auto it = group.second->GetObjects().find( p_name );

					if ( it != group.second->GetObjects().end() )
					{
						result = it->second;
					}
				}
			}

			return result;
		}

		template< typename CreatorFunc, typename NodesType, typename ... Params >
		void DoAddNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, Pass & p_pass
			, NodesType & p_nodes
			, CreatorFunc p_creator )
		{
			if ( p_pass.IsTwoSided()
				|| p_pass.HasAlphaBlending() )
			{
				auto pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, p_pass.GetAlphaFunc()
					, p_textureFlags
					, p_programFlags
					, p_sceneFlags );

				if ( pipeline )
				{
					auto node = p_creator( *pipeline );
					DoAddRenderNode( *pipeline, node, p_nodes.m_frontCulled );
				}
			}

			auto pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
				, p_pass.GetAlphaBlendMode()
				, p_pass.GetAlphaFunc()
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags );

			if ( pipeline )
			{
				auto node = p_creator( *pipeline );
				DoAddRenderNode( *pipeline, node, p_nodes.m_backCulled );
			}
		}

		void DoAddSkinningNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedSkeleton & p_skeleton
			, SceneRenderNodes::SkinnedNodesMap & p_animated
			, SceneRenderNodes::InstantiatedSkinnedNodesMap & p_instanced )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( p_pass.HasAlphaBlending() 
					|| p_pass.IsTwoSided() )
				{
					auto pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
						, p_pass.GetAlphaBlendMode()
						, p_pass.GetAlphaFunc()
						, p_textureFlags
						, p_programFlags
						, p_sceneFlags );

					if ( pipeline )
					{
						auto node = p_renderPass.CreateSkinningNode( p_pass, *pipeline, p_submesh, p_primitive, p_skeleton );
						DoAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_frontCulled );
					}
				}

				auto pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, p_pass.GetAlphaFunc()
					, p_textureFlags
					, p_programFlags
					, p_sceneFlags );

				if ( pipeline )
				{
					auto node = p_renderPass.CreateSkinningNode( p_pass, *pipeline, p_submesh, p_primitive, p_skeleton );
					DoAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_backCulled );
				}
			}
			else
			{
				DoAddNode( p_renderPass
					, p_textureFlags
					, p_programFlags
					, p_sceneFlags
					, p_pass
					, p_animated
					, std::bind( &RenderPass::CreateSkinningNode
						, &p_renderPass
						, std::ref( p_pass )
						, std::placeholders::_1
						, std::ref( p_submesh )
						, std::ref( p_primitive )
						, std::ref( p_skeleton ) ) );
			}
		}

		void DoAddMorphingNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedMesh & p_mesh
			, SceneRenderNodes::MorphingNodesMap & p_animated )
		{
			DoAddNode( p_renderPass
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_pass
				, p_animated
				, std::bind( &RenderPass::CreateMorphingNode
					, &p_renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_submesh )
					, std::ref( p_primitive )
					, std::ref( p_mesh ) ) );
		}

		void DoAddStaticNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, SceneRenderNodes::StaticNodesMap & p_static
			, SceneRenderNodes::InstantiatedStaticNodesMap & p_instanced )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( p_pass.HasAlphaBlending() 
					|| p_pass.IsTwoSided() )
				{
					auto pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
						, p_pass.GetAlphaBlendMode()
						, p_pass.GetAlphaFunc()
						, p_textureFlags
						, p_programFlags
						, p_sceneFlags );

					if ( pipeline )
					{
						auto node = p_renderPass.CreateStaticNode( p_pass, *pipeline, p_submesh, p_primitive );
						DoAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_frontCulled );
					}
				}

				auto pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, p_pass.GetAlphaFunc()
					, p_textureFlags
					, p_programFlags
					, p_sceneFlags );

				if ( pipeline )
				{
					auto node = p_renderPass.CreateStaticNode( p_pass, *pipeline, p_submesh, p_primitive );
					DoAddRenderNode( p_pass, *pipeline, node, p_submesh, p_instanced.m_backCulled );
				}
			}
			else
			{
				DoAddNode( p_renderPass
					, p_textureFlags
					, p_programFlags
					, p_sceneFlags
					, p_pass
					, p_static
					, std::bind( &RenderPass::CreateStaticNode
						, &p_renderPass
						, std::ref( p_pass )
						, std::placeholders::_1
						, std::ref( p_submesh )
						, std::ref( p_primitive ) ) );
			}
		}

		void DoAddBillboardNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, Pass & p_pass
			, BillboardBase & p_billboard
			, SceneRenderNodes::BillboardNodesMap & p_nodes )
		{
			DoAddNode( p_renderPass
				, p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_pass
				, p_nodes
				, std::bind( &RenderPass::CreateBillboardNode
					, &p_renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_billboard ) ) );
		}

		void DoSortRenderNodes( RenderPass & p_renderPass
			, bool p_opaque
			, SceneNode const * p_ignored
			, Scene & p_scene
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

			bool shadows{ p_scene.HasShadows() };

			auto lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto primitive : p_scene.GetGeometryCache() )
			{
				if ( p_ignored != primitive.second->GetParent().get()
					&& primitive.second->GetParent()->IsVisible() )
				{
					MeshSPtr mesh = primitive.second->GetMesh();
					REQUIRE( mesh );

					for ( auto submesh : *mesh )
					{
						MaterialSPtr material( primitive.second->GetMaterial( *submesh ) );
						REQUIRE( material );

						for ( auto pass : *material )
						{
							auto programFlags = submesh->GetProgramFlags();
							auto sceneFlags = p_scene.GetFlags();
							RemFlag( programFlags, ProgramFlag::eSkinning );
							RemFlag( programFlags, ProgramFlag::eMorphing );
							auto skeleton = std::static_pointer_cast< AnimatedSkeleton >( DoFindAnimatedObject( p_scene, primitive.first + cuT( "_Skeleton" ) ) );
							auto mesh = std::static_pointer_cast< AnimatedMesh >( DoFindAnimatedObject( p_scene, primitive.first + cuT( "_Mesh" ) ) );

							if ( material->GetType() == MaterialType::ePbrMetallicRoughness )
							{
								AddFlag( programFlags, ProgramFlag::ePbrMetallicRoughness );
							}
							else if ( material->GetType() == MaterialType::ePbrSpecularGlossiness )
							{
								AddFlag( programFlags, ProgramFlag::ePbrSpecularGlossiness );
							}

							if ( skeleton && submesh->HasBoneData() )
							{
								AddFlag( programFlags, ProgramFlag::eSkinning );
							}

							if ( mesh )
							{
								AddFlag( programFlags, ProgramFlag::eMorphing );
							}

							if ( !shadows
								|| !primitive.second->IsShadowReceiver() )
							{
								RemFlag( sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
							}

							pass->PrepareTextures();

							if ( submesh->GetRefCount( material ) > 1
								&& !CheckFlag( programFlags, ProgramFlag::eMorphing )
								&& ( !pass->HasAlphaBlending() || p_renderPass.IsOrderIndependent() )
								&& p_renderPass.GetEngine()->GetRenderSystem()->GetGpuInformations().HasInstancing()
								&& !pass->HasEnvironmentMapping() )
							{
								AddFlag( programFlags, ProgramFlag::eInstantiation );
							}
							else
							{
								RemFlag( programFlags, ProgramFlag::eInstantiation );
							}

							AddFlags( programFlags, pass->GetProgramFlags() );

							auto textureFlags = pass->GetTextureFlags();
							p_renderPass.PreparePipeline( pass->GetColourBlendMode()
								, pass->GetAlphaBlendMode()
								, pass->GetAlphaFunc()
								, textureFlags
								, programFlags
								, sceneFlags
								, pass->IsTwoSided() );

							if ( CheckFlag( programFlags, ProgramFlag::eAlphaBlending ) != p_opaque )
							{
								if ( !IsShadowMapProgram( programFlags )
									|| primitive.second->IsShadowCaster() )
								{
									if ( CheckFlag( programFlags, ProgramFlag::eSkinning ) )
									{
										DoAddSkinningNode( p_renderPass
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
									else if ( CheckFlag( programFlags, ProgramFlag::eMorphing ) )
									{
										DoAddMorphingNode( p_renderPass
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
										DoAddStaticNode( p_renderPass
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

		void DoSortRenderNodes( RenderPass & p_renderPass
			, bool p_opaque
			, Scene & p_scene
			, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & p_nodes )
		{
			bool shadows{ p_scene.HasShadows() };
			auto addNode = [&p_opaque, &p_renderPass, &p_scene, &p_nodes, &shadows]( Pass & p_pass
				, BillboardBase & p_billboard )
			{
				p_pass.PrepareTextures();
				auto programFlags = p_billboard.GetProgramFlags();
				auto sceneFlags = p_scene.GetFlags();
				AddFlag( programFlags, ProgramFlag::eBillboards );

				if ( p_pass.HasAlphaBlending() )
				{
					AddFlag( programFlags, ProgramFlag::eAlphaBlending );
				}

				if ( !shadows
					|| !p_billboard.IsShadowReceiver() )
				{
					RemFlag( sceneFlags, SceneFlag::eShadowFilterRaw );
					RemFlag( sceneFlags, SceneFlag::eShadowFilterPoisson );
					RemFlag( sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
				}

				auto textureFlags = p_pass.GetTextureFlags();
				p_renderPass.PreparePipeline( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, p_pass.GetAlphaFunc()
					, textureFlags
					, programFlags
					, sceneFlags
					, p_pass.IsTwoSided() );

				if ( CheckFlag( programFlags, ProgramFlag::eAlphaBlending ) != p_opaque
					&& !IsShadowMapProgram( programFlags ) )
				{
					DoAddBillboardNode( p_renderPass
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
				auto lock = make_unique_lock( p_scene.GetBillboardListCache() );

				for ( auto billboard : p_scene.GetBillboardListCache() )
				{
					MaterialSPtr material( billboard.second->GetMaterial() );
					REQUIRE( material );

					for ( auto pass : *material )
					{
						addNode( *pass, *billboard.second );
					}
				}
			}
			{
				auto lock = make_unique_lock( p_scene.GetParticleSystemCache() );

				for ( auto particleSystem : p_scene.GetParticleSystemCache() )
				{
					MaterialSPtr material( particleSystem.second->GetMaterial() );
					REQUIRE( material );

					for ( auto pass : *material )
					{
						addNode( *pass, *particleSystem.second->GetBillboards() );
					}
				}
			}
		}

		template< typename MapType, typename ArrayType >
		void DoAddRenderNodes( Camera const & p_camera
			, MapType & p_outputNodes
			, RenderPipeline & p_pipeline
			, Pass & p_pass
			, Submesh & p_submesh
			, ArrayType & p_renderNodes )
		{
			for ( auto const & node : p_renderNodes )
			{
				if ( node.m_sceneNode.IsDisplayable()
					 && node.m_sceneNode.IsVisible()
					 && p_camera.IsVisible( node.m_data.GetCollisionSphere(), node.m_sceneNode.GetDerivedTransformationMatrix() ) )
				{
					DoAddRenderNode( p_pass, p_pipeline, node, p_submesh, p_outputNodes );
				}
			}
		}

		template< typename MapType >
		void DoParseRenderNodes( Camera const & p_camera
			, MapType const & p_inputNodes
			, MapType & p_outputNodes )
		{
			for ( auto & pipelines : p_inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.m_sceneNode.IsDisplayable()
						 && node.m_sceneNode.IsVisible()
						 && p_camera.IsVisible( node.m_data.GetCollisionSphere(), node.m_sceneNode.GetDerivedTransformationMatrix() ) )
					{
						DoAddRenderNode( *pipelines.first, node, p_outputNodes );
					}
				}
			}
		}

		template<>
		void DoParseRenderNodes( Camera const & p_camera
			, BillboardRenderNodesByPipelineMap const & p_inputNodes
			, BillboardRenderNodesByPipelineMap & p_outputNodes )
		{
			for ( auto & pipelines : p_inputNodes )
			{
				for ( auto const & node : pipelines.second )
				{
					if ( node.m_sceneNode.IsDisplayable()
						 && node.m_sceneNode.IsVisible() )
					{
						DoAddRenderNode( *pipelines.first, node, p_outputNodes );
					}
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderPass & p_renderPass
		, bool p_opaque
		, SceneNode const * p_ignored )
		: OwnedBy< RenderPass >{ p_renderPass }
		, m_opaque{ p_opaque }
		, m_ignored{ p_ignored }
	{
	}

	void RenderQueue::Initialise( Scene & p_scene, Camera & p_camera )
	{
		Initialise( p_scene );
		m_cameraChanged = p_camera.onChanged.connect( std::bind( &RenderQueue::OnCameraChanged, this, std::placeholders::_1 ) );
		OnCameraChanged( p_camera );
		m_preparedRenderNodes = std::make_unique< SceneRenderNodes >( p_scene );
	}

	void RenderQueue::Initialise( Scene & p_scene )
	{
		m_sceneChanged = p_scene.onChanged.connect( std::bind( &RenderQueue::OnSceneChanged, this, std::placeholders::_1 ) );
		OnSceneChanged( p_scene );
		m_renderNodes = std::make_unique< SceneRenderNodes >( p_scene );
	}

	void RenderQueue::Update()
	{
		if ( m_isSceneChanged )
		{
			DoSortRenderNodes();
			m_isSceneChanged = false;
			m_changed = true;
		}

		if ( m_changed )
		{
			if ( m_camera )
			{
				DoPrepareRenderNodes();
			}

			m_changed = false;
		}
	}
	
	SceneRenderNodes & RenderQueue::GetRenderNodes()const
	{
		if ( m_camera )
		{
			REQUIRE( m_preparedRenderNodes );
			return *m_preparedRenderNodes;
		}

		REQUIRE( m_renderNodes );
		return *m_renderNodes;
	}

	void RenderQueue::DoPrepareRenderNodes()
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

		DoTraverseNodes( m_renderNodes->m_instantiatedStaticNodes.m_frontCulled
			, std::bind( DoAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedStaticNodes.m_frontCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoTraverseNodes( m_renderNodes->m_instantiatedStaticNodes.m_backCulled
			, std::bind( DoAddRenderNodes< SubmeshStaticRenderNodesByPipelineMap, StaticRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedStaticNodes.m_backCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoTraverseNodes( m_renderNodes->m_instantiatedSkinnedNodes.m_frontCulled
			, std::bind( DoAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_frontCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoTraverseNodes( m_renderNodes->m_instantiatedSkinnedNodes.m_backCulled
			, std::bind( DoAddRenderNodes< SubmeshSkinningRenderNodesByPipelineMap, SkinningRenderNodeArray >
				, std::ref( camera )
				, std::ref( m_preparedRenderNodes->m_instantiatedSkinnedNodes.m_backCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoParseRenderNodes( camera
			, m_renderNodes->m_staticNodes.m_frontCulled
			, m_preparedRenderNodes->m_staticNodes.m_frontCulled );
		DoParseRenderNodes( camera
			, m_renderNodes->m_staticNodes.m_backCulled
			, m_preparedRenderNodes->m_staticNodes.m_backCulled );

		DoParseRenderNodes( camera
			, m_renderNodes->m_skinnedNodes.m_frontCulled
			, m_preparedRenderNodes->m_skinnedNodes.m_frontCulled );
		DoParseRenderNodes( camera
			, m_renderNodes->m_skinnedNodes.m_backCulled
			, m_preparedRenderNodes->m_skinnedNodes.m_backCulled );

		DoParseRenderNodes( camera
			, m_renderNodes->m_morphingNodes.m_frontCulled
			, m_preparedRenderNodes->m_morphingNodes.m_frontCulled );
		DoParseRenderNodes( camera
			, m_renderNodes->m_morphingNodes.m_backCulled
			, m_preparedRenderNodes->m_morphingNodes.m_backCulled );

		DoParseRenderNodes( camera
			, m_renderNodes->m_billboardNodes.m_frontCulled
			, m_preparedRenderNodes->m_billboardNodes.m_frontCulled );
		DoParseRenderNodes( camera
			, m_renderNodes->m_billboardNodes.m_backCulled
			, m_preparedRenderNodes->m_billboardNodes.m_backCulled );
	}

	void RenderQueue::DoSortRenderNodes()
	{
		Castor3D::DoSortRenderNodes( *GetOwner()
			, m_opaque
			, m_ignored
			, m_renderNodes->m_scene
			, m_renderNodes->m_staticNodes
			, m_renderNodes->m_instantiatedStaticNodes
			, m_renderNodes->m_skinnedNodes
			, m_renderNodes->m_instantiatedSkinnedNodes
			, m_renderNodes->m_morphingNodes );
		Castor3D::DoSortRenderNodes( *GetOwner()
			, m_opaque
			, m_renderNodes->m_scene
			, m_renderNodes->m_billboardNodes );
	}

	void RenderQueue::OnSceneChanged( Scene const & p_scene )
	{
		m_isSceneChanged = true;
	}

	void RenderQueue::OnCameraChanged( Camera const & p_camera )
	{
		m_changed = true;
		m_camera = &p_camera;
	}
}
