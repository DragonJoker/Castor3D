#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderQueue.hpp"

#include "RenderPass.hpp"

#include "Engine.hpp"
#include "Cache/AnimatedObjectGroupCache.hpp"
#include "Cache/BillboardCache.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/GeometryCache.hpp"
#include "Cache/LightCache.hpp"
#include "Cache/ParticleSystemCache.hpp"
#include "Cache/SamplerCache.hpp"
#include "Cache/ShaderCache.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderPass.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Technique/RenderTechnique.hpp"
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
			for ( auto l_itPipelines : p_nodes )
			{
				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeType, typename MapType >
		void DoAddRenderNode( RenderPipeline & p_pipeline
			, NodeType const & p_node
			, MapType & p_nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			auto l_itPipeline = p_nodes.insert( { &p_pipeline, ObjectRenderNodesArray() } ).first;
			l_itPipeline->second.push_back( p_node );
		}

		template< typename NodeType >
		void DoAddRenderNode( Pass & p_pass
			, RenderPipeline & p_pipeline
			, NodeType const & p_node
			, Submesh & p_object
			, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
		{
			using ObjectRenderNodesByPipelineMap = SubmeshStaticRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesByPassMap = ObjectRenderNodesByPipelineMap::mapped_type;
			using ObjectRenderNodesArray = ObjectRenderNodesByPassMap::mapped_type;

			auto l_itPipeline = p_nodes.insert( { &p_pipeline, ObjectRenderNodesByPipelineMap() } ).first;
			auto l_itPass = l_itPipeline->second.insert( { &p_pass, ObjectRenderNodesByPassMap() } ).first;
			auto l_itObject = l_itPass->second.insert( { &p_object, ObjectRenderNodesArray() } ).first;
			l_itObject->second.push_back( p_node );
		}

		AnimatedObjectSPtr DoFindAnimatedObject( Scene & p_scene, String const & p_name )
		{
			AnimatedObjectSPtr l_return;
			auto & l_cache = p_scene.GetAnimatedObjectGroupCache();
			auto l_lock = make_unique_lock( l_cache );

			for ( auto l_group : l_cache )
			{
				if ( !l_return )
				{
					auto l_it = l_group.second->GetObjects().find( p_name );

					if ( l_it != l_group.second->GetObjects().end() )
					{
						l_return = l_it->second;
					}
				}
			}

			return l_return;
		}

		template< typename CreatorFunc, typename NodesType, typename ... Params >
		void DoAddNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, Pass & p_pass
			, Scene const & p_scene
			, NodesType & p_nodes
			, CreatorFunc p_creator )
		{
			if ( p_pass.IsTwoSided()
				|| p_pass.HasAlphaBlending() )
			{
				auto l_pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, p_textureFlags
					, p_programFlags
					, p_scene.GetFlags() );

				if ( l_pipeline )
				{
					auto l_node = p_creator( *l_pipeline );
					DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_frontCulled );
				}
			}

			auto l_pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
				, p_pass.GetAlphaBlendMode()
				, p_textureFlags
				, p_programFlags
				, p_scene.GetFlags() );

			if ( l_pipeline )
			{
				auto l_node = p_creator( *l_pipeline );
				DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_backCulled );
			}
		}

		void DoAddAnimatedNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, AnimatedSkeletonSPtr p_skeleton
			, AnimatedMeshSPtr p_mesh
			, RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > & p_animated )
		{
			DoAddNode( p_renderPass
				, p_textureFlags
				, p_programFlags
				, p_pass
				, *p_primitive.GetScene()
				, p_animated
				, std::bind( &RenderPass::CreateAnimatedNode
					, &p_renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_submesh )
					, std::ref( p_primitive )
					, p_skeleton
					, p_mesh ) );
		}

		void DoAddStaticNode( RenderPass & p_renderPass
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, Pass & p_pass
			, Submesh & p_submesh
			, Geometry & p_primitive
			, RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > & p_static
			, RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap > & p_instanced )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				if ( !p_pass.HasAlphaBlending() )
				{
					if ( p_pass.IsTwoSided() )
					{
						auto l_pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
							, p_pass.GetAlphaBlendMode()
							, p_textureFlags
							, p_programFlags
							, p_primitive.GetScene()->GetFlags() );

						if ( l_pipeline )
						{
							auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
							DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_frontCulled );
						}
					}

					auto l_pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
						, p_pass.GetAlphaBlendMode()
						, p_textureFlags
						, p_programFlags
						, p_primitive.GetScene()->GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
						DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_backCulled );
					}
				}
				else
				{
					auto l_pipeline = p_renderPass.GetPipelineFront( p_pass.GetColourBlendMode()
						, p_pass.GetAlphaBlendMode()
						, p_textureFlags
						, p_programFlags
						, p_primitive.GetScene()->GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
						DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_frontCulled );
					}

					l_pipeline = p_renderPass.GetPipelineBack( p_pass.GetColourBlendMode()
						, p_pass.GetAlphaBlendMode()
						, p_textureFlags
						, p_programFlags
						, p_primitive.GetScene()->GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
						DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_backCulled );
					}
				}
			}
			else
			{
				DoAddNode( p_renderPass
					, p_textureFlags
					, p_programFlags
					, p_pass
					, *p_primitive.GetScene()
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
			, Pass & p_pass
			, BillboardBase & p_billboard
			, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & p_nodes )
		{
			DoAddNode( p_renderPass
				, p_textureFlags
				, p_programFlags
				, p_pass
				, p_billboard.GetParentScene()
				, p_nodes
				, std::bind( &RenderPass::CreateBillboardNode
					, &p_renderPass
					, std::ref( p_pass )
					, std::placeholders::_1
					, std::ref( p_billboard ) ) );
		}

		void DoSortRenderNodes( RenderPass & p_renderPass
			, bool p_opaque
			, Scene & p_scene
			, RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > & p_static
			, RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap > & p_instanced
			, RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > & p_animated )
		{
			p_static.m_frontCulled.clear();
			p_static.m_backCulled.clear();
			p_instanced.m_frontCulled.clear();
			p_instanced.m_backCulled.clear();
			p_animated.m_frontCulled.clear();
			p_animated.m_backCulled.clear();

			bool l_shadows{ p_scene.HasShadows() };

			auto l_lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto l_primitive : p_scene.GetGeometryCache() )
			{
				MeshSPtr l_mesh = l_primitive.second->GetMesh();
				REQUIRE( l_mesh );

				for ( auto l_submesh : *l_mesh )
				{
					MaterialSPtr l_material( l_primitive.second->GetMaterial( *l_submesh ) );
					REQUIRE( l_material );

					for ( auto l_pass : *l_material )
					{
						auto l_programFlags = l_submesh->GetProgramFlags();
						RemFlag( l_programFlags, ProgramFlag::eSkinning );
						RemFlag( l_programFlags, ProgramFlag::eMorphing );
						auto l_skeleton = std::static_pointer_cast< AnimatedSkeleton >( DoFindAnimatedObject( p_scene, l_primitive.first + cuT( "_Skeleton" ) ) );
						auto l_mesh = std::static_pointer_cast< AnimatedMesh >( DoFindAnimatedObject( p_scene, l_primitive.first + cuT( "_Mesh" ) ) );

						if ( l_skeleton )
						{
							AddFlag( l_programFlags, ProgramFlag::eSkinning );
						}

						if ( l_mesh )
						{
							AddFlag( l_programFlags, ProgramFlag::eMorphing );
						}

						if ( l_shadows
							&& l_primitive.second->IsShadowReceiver() )
						{
							AddFlag( l_programFlags, ProgramFlag::eShadows );
						}

						l_pass->PrepareTextures();

						if ( l_submesh->GetRefCount( l_material ) > 1
								&& !l_mesh
								&& !l_skeleton
								&& ( !l_pass->HasAlphaBlending()
									 || p_renderPass.IsMultisampling() )
								&& p_renderPass.GetEngine()->GetRenderSystem()->GetGpuInformations().HasInstancing() )
						{
							AddFlag( l_programFlags, ProgramFlag::eInstantiation );
						}
						else
						{
							RemFlag( l_programFlags, ProgramFlag::eInstantiation );
						}

						if ( l_pass->HasAlphaBlending() )
						{
							AddFlag( l_programFlags, ProgramFlag::eAlphaBlending );
						}

						auto l_textureFlags = l_pass->GetTextureFlags();
						p_renderPass.PreparePipeline( l_pass->GetColourBlendMode()
							, l_pass->GetAlphaBlendMode()
							, l_textureFlags
							, l_programFlags
							, p_scene.GetFlags(), l_pass->IsTwoSided() );

						if ( CheckFlag( l_programFlags, ProgramFlag::eAlphaBlending ) != p_opaque )
						{
							if ( !CheckFlag( l_programFlags, ProgramFlag::eShadowMap ) || l_primitive.second->IsShadowCaster() )
							{
								if ( CheckFlag( l_programFlags, ProgramFlag::eSkinning )
										|| CheckFlag( l_programFlags, ProgramFlag::eMorphing ) )
								{
									DoAddAnimatedNode( p_renderPass
										, l_textureFlags
										, l_programFlags
										, *l_pass
										, *l_submesh
										, *l_primitive.second
										, l_skeleton
										, l_mesh
										, p_animated );
								}
								else
								{
									DoAddStaticNode( p_renderPass
										, l_textureFlags
										, l_programFlags
										, *l_pass
										, *l_submesh
										, *l_primitive.second
										, p_static
										, p_instanced );
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
			auto l_addNode = [&p_opaque, &p_renderPass, &p_scene, &p_nodes]( Pass & p_pass
				, BillboardBase & p_billboard )
			{
				p_pass.PrepareTextures();
				auto l_programFlags = p_billboard.GetProgramFlags();
				AddFlag( l_programFlags, ProgramFlag::eBillboards );

				if ( p_pass.HasAlphaBlending() )
				{
					AddFlag( l_programFlags, ProgramFlag::eAlphaBlending );
				}

				auto l_textureFlags = p_pass.GetTextureFlags();
				p_renderPass.PreparePipeline( p_pass.GetColourBlendMode()
					, p_pass.GetAlphaBlendMode()
					, l_textureFlags
					, l_programFlags
					, p_scene.GetFlags()
					, p_pass.IsTwoSided() );

				if ( CheckFlag( l_programFlags, ProgramFlag::eAlphaBlending ) != p_opaque )
				{
					DoAddBillboardNode( p_renderPass
						, l_textureFlags
						, l_programFlags
						, p_pass
						, p_billboard
						, p_nodes );
				}
			};
			p_nodes.m_frontCulled.clear();
			p_nodes.m_backCulled.clear();
			{
				auto l_lock = make_unique_lock( p_scene.GetBillboardListCache() );

				for ( auto l_billboard : p_scene.GetBillboardListCache() )
				{
					MaterialSPtr l_material( l_billboard.second->GetMaterial() );
					REQUIRE( l_material );

					for ( auto l_pass : *l_material )
					{
						l_addNode( *l_pass, *l_billboard.second );
					}
				}
			}
			{
				auto l_lock = make_unique_lock( p_scene.GetParticleSystemCache() );

				for ( auto l_particleSystem : p_scene.GetParticleSystemCache() )
				{
					MaterialSPtr l_material( l_particleSystem.second->GetMaterial() );
					REQUIRE( l_material );

					for ( auto l_pass : *l_material )
					{
						l_addNode( *l_pass, *l_particleSystem.second->GetBillboards() );
					}
				}
			}
		}

		void DoAddRenderNodes( Camera const & p_camera
			, SubmeshStaticRenderNodesByPipelineMap & p_outputNodes
			, RenderPipeline & p_pipeline
			, Pass & p_pass
			, Submesh & p_submesh
			, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto const & l_node : p_renderNodes )
			{
				if ( l_node.m_sceneNode.IsDisplayable()
					 && l_node.m_sceneNode.IsVisible()
					 && p_camera.IsVisible( l_node.m_data.GetCollisionSphere(), l_node.m_sceneNode.GetDerivedTransformationMatrix() ) )
				{
					DoAddRenderNode( p_pass, p_pipeline, l_node, p_submesh, p_outputNodes );
				}
			}
		}

		template< typename MapType >
		void DoParseRenderNodes( Camera const & p_camera
			, MapType const & p_inputNodes
			, MapType & p_outputNodes )
		{
			for ( auto & l_pipelines : p_inputNodes )
			{
				for ( auto const & l_node : l_pipelines.second )
				{
					if ( l_node.m_sceneNode.IsDisplayable()
						 && l_node.m_sceneNode.IsVisible()
						 && p_camera.IsVisible( l_node.m_data.GetCollisionSphere(), l_node.m_sceneNode.GetDerivedTransformationMatrix() ) )
					{
						DoAddRenderNode( *l_pipelines.first, l_node, p_outputNodes );
					}
				}
			}
		}

		template<>
		void DoParseRenderNodes( Camera const & p_camera
			, BillboardRenderNodesByPipelineMap const & p_inputNodes
			, BillboardRenderNodesByPipelineMap & p_outputNodes )
		{
			for ( auto & l_pipelines : p_inputNodes )
			{
				for ( auto const & l_node : l_pipelines.second )
				{
					if ( l_node.m_sceneNode.IsDisplayable()
						 && l_node.m_sceneNode.IsVisible() )
					{
						DoAddRenderNode( *l_pipelines.first, l_node, p_outputNodes );
					}
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderPass & p_renderPass, bool p_opaque )
		: OwnedBy< RenderPass >{ p_renderPass }
		, m_opaque{ p_opaque }
	{
	}

	void RenderQueue::Initialise( Scene & p_scene, Camera & p_camera )
	{
		Initialise( p_scene );
		m_cameraChanged = p_camera.Connect( std::bind( &RenderQueue::OnCameraChanged, this, std::placeholders::_1 ) );
		OnCameraChanged( p_camera );
		m_camera = &p_camera;
		m_preparedRenderNodes = std::make_unique< SceneRenderNodes >( p_scene );
	}

	void RenderQueue::Initialise( Scene & p_scene )
	{
		m_sceneChanged = p_scene.Connect( std::bind( &RenderQueue::OnSceneChanged, this, std::placeholders::_1 ) );
		OnSceneChanged( p_scene );
		m_renderNodes = std::make_unique< SceneRenderNodes >( p_scene );
	}

	void RenderQueue::Update()
	{
		if ( m_changed )
		{
			DoSortRenderNodes();

			if ( m_camera )
			{
				DoPrepareRenderNodes();
			}

			m_changed = false;
		}
	}
	
	SceneRenderNodes & RenderQueue::GetRenderNodes()
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
		m_preparedRenderNodes->m_instancedGeometries.m_backCulled.clear();
		m_preparedRenderNodes->m_instancedGeometries.m_frontCulled.clear();
		m_preparedRenderNodes->m_staticGeometries.m_backCulled.clear();
		m_preparedRenderNodes->m_staticGeometries.m_frontCulled.clear();
		m_preparedRenderNodes->m_animatedGeometries.m_backCulled.clear();
		m_preparedRenderNodes->m_animatedGeometries.m_frontCulled.clear();
		m_preparedRenderNodes->m_billboards.m_backCulled.clear();
		m_preparedRenderNodes->m_billboards.m_frontCulled.clear();

		auto & l_camera = *m_camera;

		DoTraverseNodes( m_renderNodes->m_instancedGeometries.m_frontCulled
			, std::bind( DoAddRenderNodes
				, std::ref( l_camera )
				, std::ref( m_preparedRenderNodes->m_instancedGeometries.m_frontCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoTraverseNodes( m_renderNodes->m_instancedGeometries.m_backCulled
			, std::bind( DoAddRenderNodes
				, std::ref( l_camera )
				, std::ref( m_preparedRenderNodes->m_instancedGeometries.m_backCulled )
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3
				, std::placeholders::_4 ) );

		DoParseRenderNodes( l_camera
			, m_renderNodes->m_staticGeometries.m_frontCulled
			, m_preparedRenderNodes->m_staticGeometries.m_frontCulled );
		DoParseRenderNodes( l_camera
			, m_renderNodes->m_staticGeometries.m_backCulled
			, m_preparedRenderNodes->m_staticGeometries.m_backCulled );

		DoParseRenderNodes( l_camera
			, m_renderNodes->m_animatedGeometries.m_frontCulled
			, m_preparedRenderNodes->m_animatedGeometries.m_frontCulled );
		DoParseRenderNodes( l_camera
			, m_renderNodes->m_animatedGeometries.m_backCulled
			, m_preparedRenderNodes->m_animatedGeometries.m_backCulled );

		DoParseRenderNodes( l_camera
			, m_renderNodes->m_billboards.m_frontCulled
			, m_preparedRenderNodes->m_billboards.m_frontCulled );
		DoParseRenderNodes( l_camera
			, m_renderNodes->m_billboards.m_backCulled
			, m_preparedRenderNodes->m_billboards.m_backCulled );
	}

	void RenderQueue::DoSortRenderNodes()
	{
		Castor3D::DoSortRenderNodes( *GetOwner()
			, m_opaque
			, m_renderNodes->m_scene
			, m_renderNodes->m_staticGeometries
			, m_renderNodes->m_instancedGeometries
			, m_renderNodes->m_animatedGeometries );
		Castor3D::DoSortRenderNodes( *GetOwner()
			, m_opaque
			, m_renderNodes->m_scene
			, m_renderNodes->m_billboards );
	}

	void RenderQueue::OnSceneChanged( Scene const & p_scene )
	{
		m_changed = true;
	}

	void RenderQueue::OnCameraChanged( Camera const & p_camera )
	{
		m_changed = true;
		m_camera = &p_camera;
	}
}
