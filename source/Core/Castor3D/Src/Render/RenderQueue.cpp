#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderQueue.hpp"

#include "RenderPass.hpp"

#include "AnimatedObjectGroupCache.hpp"
#include "BillboardCache.hpp"
#include "CameraCache.hpp"
#include "Engine.hpp"
#include "GeometryCache.hpp"
#include "LightCache.hpp"
#include "SamplerCache.hpp"
#include "ShaderCache.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Render/Pipeline.hpp"
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
		void DoAddRenderNode( Pipeline & p_pipeline
							  , NodeType const & p_node
							  , MapType & p_nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			auto l_itPipeline = p_nodes.insert( { &p_pipeline, ObjectRenderNodesArray() } ).first;
			l_itPipeline->second.push_back( p_node );
		}

		template< typename NodeType >
		void DoAddRenderNode( Pass & p_pass
							  , Pipeline & p_pipeline
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
						, uint16_t p_programFlags
						, Pass & p_pass
						, Scene const & p_scene
						, NodesType & p_nodes
						, CreatorFunc p_creator )
		{
			if ( !p_pass.HasAlphaBlending() )
			{
				if ( p_pass.IsTwoSided() )
				{
					auto l_pipeline = p_renderPass.GetOpaquePipelineFront( p_pass.GetColourBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_scene.GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_creator( *l_pipeline );
						DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_opaqueRenderNodesFront );
					}
				}

				auto l_pipeline = p_renderPass.GetOpaquePipelineBack( p_pass.GetColourBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_scene.GetFlags() );

				if ( l_pipeline )
				{
					auto l_node = p_creator( *l_pipeline );
					DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_opaqueRenderNodesBack );
				}
			}
			else
			{
				{
					auto l_pipeline = p_renderPass.GetTransparentPipelineFront( p_pass.GetColourBlendMode(), p_pass.GetAlphaBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_scene.GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_creator( *l_pipeline );
						DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_transparentRenderNodesFront );
					}
				}
				{
					auto l_pipeline = p_renderPass.GetTransparentPipelineBack( p_pass.GetColourBlendMode(), p_pass.GetAlphaBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_scene.GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_creator( *l_pipeline );
						DoAddRenderNode( *l_pipeline, l_node, p_nodes.m_transparentRenderNodesBack );
					}
				}
			}
		}

		void DoAddAnimatedNode( RenderPass & p_renderPass
								, uint16_t p_programFlags
								, Pass & p_pass
								, Submesh & p_submesh
								, Geometry & p_primitive
								, AnimatedSkeletonSPtr p_skeleton
								, AnimatedMeshSPtr p_mesh
								, RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > & p_animated )
		{
			DoAddNode( p_renderPass
					   , p_programFlags
					   , p_pass
					   , *p_primitive.GetScene()
					   , p_animated
					   , std::bind( &RenderPass::CreateAnimatedNode, &p_renderPass, std::ref( p_pass ), std::placeholders::_1, std::ref( p_submesh ), std::ref( p_primitive ), p_skeleton, p_mesh ) );
		}

		void DoAddStaticNode( RenderPass & p_renderPass
							  , uint16_t p_programFlags
							  , Pass & p_pass
							  , Submesh & p_submesh
							  , Geometry & p_primitive
							  , RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > & p_static
							  , RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap, SubmeshStaticRenderNodesByPipelineMap > & p_instanced )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::Instantiation ) )
			{
				if ( !p_pass.HasAlphaBlending() )
				{
					if ( p_pass.IsTwoSided() )
					{
						auto l_pipeline = p_renderPass.GetOpaquePipelineFront( p_pass.GetColourBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_primitive.GetScene()->GetFlags() );

						if ( l_pipeline )
						{
							auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
							DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_opaqueRenderNodesFront );
						}
					}

					auto l_pipeline = p_renderPass.GetOpaquePipelineBack( p_pass.GetColourBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_primitive.GetScene()->GetFlags() );

					if ( l_pipeline )
					{
						auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
						DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_opaqueRenderNodesBack );
					}
				}
				else
				{
					{
						auto l_pipeline = p_renderPass.GetTransparentPipelineFront( p_pass.GetColourBlendMode(), p_pass.GetAlphaBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_primitive.GetScene()->GetFlags() );

						if ( l_pipeline )
						{
							auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
							DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_transparentRenderNodesFront );
						}
					}
					{
						auto l_pipeline = p_renderPass.GetTransparentPipelineBack( p_pass.GetColourBlendMode(), p_pass.GetAlphaBlendMode(), p_pass.GetTextureFlags(), p_programFlags, p_primitive.GetScene()->GetFlags() );

						if ( l_pipeline )
						{
							auto l_node = p_renderPass.CreateStaticNode( p_pass, *l_pipeline, p_submesh, p_primitive );
							DoAddRenderNode( p_pass, *l_pipeline, l_node, p_submesh, p_instanced.m_transparentRenderNodesBack );
						}
					}
				}
			}
			else
			{
				DoAddNode( p_renderPass
						   , p_programFlags
						   , p_pass
						   , *p_primitive.GetScene()
						   , p_static
						   , std::bind( &RenderPass::CreateStaticNode, &p_renderPass, std::ref( p_pass ), std::placeholders::_1, std::ref( p_submesh ), std::ref( p_primitive ) ) );
			}
		}

		void DoAddBillboardNode( RenderPass & p_renderPass
								 , uint16_t p_programFlags
								 , Pass & p_pass
								 , BillboardList & p_billboard
								 , RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & p_nodes )
		{
			DoAddNode( p_renderPass
					   , p_programFlags
					   , p_pass
					   , *p_billboard.GetScene()
					   , p_nodes
					   , std::bind( &RenderPass::CreateBillboardNode, &p_renderPass, std::ref( p_pass ), std::placeholders::_1, std::ref( p_billboard ) ) );
		}

		void DoSortRenderNodes( RenderPass & p_renderPass
								, Scene & p_scene
								, RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > & p_static
								, RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap, SubmeshStaticRenderNodesByPipelineMap > & p_instanced
								, RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > & p_animated )
		{
			p_static.m_opaqueRenderNodesFront.clear();
			p_static.m_opaqueRenderNodesBack.clear();
			p_static.m_transparentRenderNodesFront.clear();
			p_static.m_transparentRenderNodesBack.clear();
			p_instanced.m_opaqueRenderNodesFront.clear();
			p_instanced.m_opaqueRenderNodesBack.clear();
			p_instanced.m_transparentRenderNodesFront.clear();
			p_instanced.m_transparentRenderNodesBack.clear();
			p_animated.m_opaqueRenderNodesFront.clear();
			p_animated.m_opaqueRenderNodesBack.clear();
			p_animated.m_transparentRenderNodesFront.clear();
			p_animated.m_transparentRenderNodesBack.clear();

			bool l_shadows{ p_scene.HasShadows() };

			auto l_lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto l_primitive : p_scene.GetGeometryCache() )
			{
				MeshSPtr l_mesh = l_primitive.second->GetMesh();
				REQUIRE( l_mesh );

				for ( auto l_submesh : *l_mesh )
				{
					MaterialSPtr l_material( l_primitive.second->GetMaterial( l_submesh ) );
					REQUIRE( l_material );

					for ( auto l_pass : *l_material )
					{
						auto l_programFlags = l_submesh->GetProgramFlags();
						RemFlag( l_programFlags, ProgramFlag::Skinning );
						RemFlag( l_programFlags, ProgramFlag::Morphing );
						auto l_skeleton = std::static_pointer_cast< AnimatedSkeleton >( DoFindAnimatedObject( p_scene, l_primitive.first + cuT( "_Skeleton" ) ) );
						auto l_mesh = std::static_pointer_cast< AnimatedMesh >( DoFindAnimatedObject( p_scene, l_primitive.first + cuT( "_Mesh" ) ) );

						if ( l_skeleton )
						{
							AddFlag( l_programFlags, ProgramFlag::Skinning );
						}

						if ( l_mesh )
						{
							AddFlag( l_programFlags, ProgramFlag::Morphing );
						}

						if ( l_shadows )
						{
							AddFlag( l_programFlags, ProgramFlag::Shadows );
						}

						l_pass->PrepareTextures();

						if ( l_submesh->GetRefCount( l_material ) > 1
								&& !l_mesh
								&& !l_skeleton
								&& ( !l_pass->HasAlphaBlending()
									|| p_renderPass.IsMultisampling() )
								&& p_renderPass.GetEngine()->GetRenderSystem()->GetGpuInformations().HasInstancing() )
						{
							AddFlag( l_programFlags, ProgramFlag::Instantiation );
						}
						else
						{
							RemFlag( l_programFlags, ProgramFlag::Instantiation );
						}

						if ( l_pass->HasAlphaBlending() )
						{
							AddFlag( l_programFlags, ProgramFlag::AlphaBlending );
						}

						p_renderPass.PreparePipeline( l_pass->GetColourBlendMode(), l_pass->GetAlphaBlendMode(), l_pass->GetTextureFlags(), l_programFlags, p_scene.GetFlags(), l_pass->IsTwoSided() );

						if ( !CheckFlag( l_programFlags, ProgramFlag::ShadowMap ) || l_primitive.second->IsShadowCaster() )
						{
							if ( CheckFlag( l_programFlags, ProgramFlag::Skinning )
									|| CheckFlag( l_programFlags, ProgramFlag::Morphing ) )
							{
								DoAddAnimatedNode( p_renderPass, l_programFlags, *l_pass, *l_submesh, *l_primitive.second, l_skeleton, l_mesh, p_animated );
							}
							else
							{
								DoAddStaticNode( p_renderPass, l_programFlags, *l_pass, *l_submesh, *l_primitive.second, p_static, p_instanced );
							}
						}
					}
				}
			}
		}

		void DoSortRenderNodes( RenderPass & p_renderPass
								, Scene & p_scene
								, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > & p_nodes )
		{
			p_nodes.m_opaqueRenderNodesFront.clear();
			p_nodes.m_opaqueRenderNodesBack.clear();
			p_nodes.m_transparentRenderNodesFront.clear();
			p_nodes.m_transparentRenderNodesBack.clear();

			auto l_lock = make_unique_lock( p_scene.GetBillboardListCache() );

			for ( auto l_billboard : p_scene.GetBillboardListCache() )
			{
				MaterialSPtr l_material( l_billboard.second->GetMaterial() );

				if ( l_material )
				{
					for ( auto l_pass : *l_material )
					{
						l_pass->PrepareTextures();
						uint16_t l_programFlags = 0u;
						AddFlag( l_programFlags, ProgramFlag::Billboards );

						if ( l_pass->HasAlphaBlending() )
						{
							AddFlag( l_programFlags, ProgramFlag::AlphaBlending );
						}

						p_renderPass.PreparePipeline( l_pass->GetColourBlendMode(), l_pass->GetAlphaBlendMode(), l_pass->GetTextureFlags(), l_programFlags, p_scene.GetFlags(), l_pass->IsTwoSided() );
						DoAddBillboardNode( p_renderPass, l_programFlags, *l_pass, *l_billboard.second, p_nodes );
					}
				}
			}
		}

		void DoAddRenderNodes( Camera const & p_camera
							   , SubmeshStaticRenderNodesByPipelineMap & p_outputNodes
							   , Pipeline & p_pipeline
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

	RenderQueue::RenderQueue( RenderPass & p_renderPass )
		: OwnedBy< RenderPass >{ p_renderPass }
	{
	}

	RenderQueue::~RenderQueue()
	{
	}

	void RenderQueue::Prepare( Camera const & p_camera, Scene & p_scene )
	{
		auto l_node = p_camera.GetParent();

		if ( l_node )
		{
			auto const l_position = l_node->GetDerivedPosition();
			auto const l_orientation = l_node->GetDerivedOrientation();
			auto l_itPosition = m_cameraPositions.insert( { &p_camera
														  , { l_position + Point3r{ 1.0_r, 1.0_r, 1.0_r }
															  , l_orientation + Quaternion{ Point4r{ 1.0_r, 1.0_r, 1.0_r, 1.0_r } } } } ).first;
			auto l_continue = Prepare( p_scene );
			auto l_itNodes = m_scenesRenderNodes.find( &p_scene );

			if ( l_continue
				 || l_itPosition->second.first != l_position
				 || l_itPosition->second.second != l_orientation )
			{
				auto l_itCamera = m_preparedRenderNodes.insert( { &p_camera, SceneRenderNodesMap{} } ).first;
				auto l_itScene = l_itCamera->second.insert( { &p_scene, SceneRenderNodes{ p_scene } } ).first;

				DoPrepareRenderNodes( p_camera, l_itNodes->second, l_itScene->second );
				l_itPosition->second.first = l_position;
				l_itPosition->second.second = l_orientation;
			}
		}
	}

	bool RenderQueue::Prepare( Scene & p_scene )
	{
		auto l_itNodes = m_scenesRenderNodes.insert( { &p_scene,{ p_scene } } ).first;
		auto l_itNew = std::find( m_newScenes.begin(), m_newScenes.end(), &p_scene );
		bool l_return{ false };

		if ( l_itNew != m_newScenes.end() )
		{
			DoSortRenderNodes( l_itNodes->second );
			m_newScenes.erase( l_itNew );
			l_return = true;
		}
		else
		{
			auto l_it = m_changedScenes.find( &p_scene );

			if ( l_it != m_changedScenes.end() )
			{
				m_changedScenes.erase( l_it );
				DoSortRenderNodes( l_itNodes->second );
				l_return = true;
			}
		}

		return l_return;
	}

	void RenderQueue::AddScene( Scene & p_scene )
	{
		m_newScenes.push_back( &p_scene );
		m_scenes.insert( { &p_scene, p_scene.Connect( std::bind( &RenderQueue::OnSceneChanged, this, std::placeholders::_1 ) ) } );
	}
	
	SceneRenderNodes & RenderQueue::GetRenderNodes( Camera const & p_camera, Scene & p_scene )
	{
		static SceneRenderNodes l_dummy{ p_scene };
		auto l_itCam = m_preparedRenderNodes.find( &p_camera );

		if ( l_itCam != m_preparedRenderNodes.end() )
		{
			auto l_itScene = l_itCam->second.find( &p_scene );

			if ( l_itScene != l_itCam->second.end() )
			{
				return l_itScene->second;
			}
		}

		return l_dummy;
	}
	
	SceneRenderNodes & RenderQueue::GetRenderNodes( Scene & p_scene )
	{
		static SceneRenderNodes l_dummy{ p_scene };
		auto l_itScene = m_scenesRenderNodes.find( &p_scene );

		if ( l_itScene != m_scenesRenderNodes.end() )
		{
			return l_itScene->second;
		}

		return l_dummy;
	}

	void RenderQueue::DoPrepareRenderNodes( Camera const & p_camera, SceneRenderNodes const & p_inputNodes, SceneRenderNodes & p_outputNodes )
	{
		p_outputNodes.m_instancedGeometries.m_opaqueRenderNodesFront.clear();
		p_outputNodes.m_instancedGeometries.m_opaqueRenderNodesBack.clear();
		p_outputNodes.m_instancedGeometries.m_transparentRenderNodesFront.clear();
		p_outputNodes.m_instancedGeometries.m_transparentRenderNodesBack.clear();
		p_outputNodes.m_staticGeometries.m_opaqueRenderNodesFront.clear();
		p_outputNodes.m_staticGeometries.m_opaqueRenderNodesBack.clear();
		p_outputNodes.m_staticGeometries.m_transparentRenderNodesFront.clear();
		p_outputNodes.m_staticGeometries.m_transparentRenderNodesBack.clear();
		p_outputNodes.m_animatedGeometries.m_opaqueRenderNodesFront.clear();
		p_outputNodes.m_animatedGeometries.m_opaqueRenderNodesBack.clear();
		p_outputNodes.m_animatedGeometries.m_transparentRenderNodesFront.clear();
		p_outputNodes.m_animatedGeometries.m_transparentRenderNodesBack.clear();
		p_outputNodes.m_billboards.m_opaqueRenderNodesFront.clear();
		p_outputNodes.m_billboards.m_opaqueRenderNodesBack.clear();
		p_outputNodes.m_billboards.m_transparentRenderNodesFront.clear();
		p_outputNodes.m_billboards.m_transparentRenderNodesBack.clear();

		auto l_checkVisible = [&p_camera]( SceneNode & p_node, auto const & p_data )
		{
			return true;
		};

		DoTraverseNodes( p_inputNodes.m_instancedGeometries.m_opaqueRenderNodesFront
						 , std::bind( DoAddRenderNodes
									  , std::ref( p_camera )
									  , std::ref( p_outputNodes.m_instancedGeometries.m_opaqueRenderNodesFront )
									  , std::placeholders::_1
									  , std::placeholders::_2
									  , std::placeholders::_3
									  , std::placeholders::_4 ) );

		DoTraverseNodes( p_inputNodes.m_instancedGeometries.m_opaqueRenderNodesBack
						 , std::bind( DoAddRenderNodes
									  , std::ref( p_camera )
									  , std::ref( p_outputNodes.m_instancedGeometries.m_opaqueRenderNodesBack )
									  , std::placeholders::_1
									  , std::placeholders::_2
									  , std::placeholders::_3
									  , std::placeholders::_4 ) );

		DoTraverseNodes( p_inputNodes.m_instancedGeometries.m_transparentRenderNodesFront
						 , std::bind( DoAddRenderNodes
									  , std::ref( p_camera )
									  , std::ref( p_outputNodes.m_instancedGeometries.m_transparentRenderNodesFront )
									  , std::placeholders::_1
									  , std::placeholders::_2
									  , std::placeholders::_3
									  , std::placeholders::_4 ) );

		DoTraverseNodes( p_inputNodes.m_instancedGeometries.m_transparentRenderNodesBack
						 , std::bind( DoAddRenderNodes
									  , std::ref( p_camera )
									  , std::ref( p_outputNodes.m_instancedGeometries.m_transparentRenderNodesBack )
									  , std::placeholders::_1
									  , std::placeholders::_2
									  , std::placeholders::_3
									  , std::placeholders::_4 ) );

		DoParseRenderNodes( p_camera, p_inputNodes.m_staticGeometries.m_opaqueRenderNodesFront, p_outputNodes.m_staticGeometries.m_opaqueRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_staticGeometries.m_opaqueRenderNodesBack, p_outputNodes.m_staticGeometries.m_opaqueRenderNodesBack );
		DoParseRenderNodes( p_camera, p_inputNodes.m_staticGeometries.m_transparentRenderNodesFront, p_outputNodes.m_staticGeometries.m_transparentRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_staticGeometries.m_transparentRenderNodesBack, p_outputNodes.m_staticGeometries.m_transparentRenderNodesBack );

		DoParseRenderNodes( p_camera, p_inputNodes.m_animatedGeometries.m_opaqueRenderNodesFront, p_outputNodes.m_animatedGeometries.m_opaqueRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_animatedGeometries.m_opaqueRenderNodesBack, p_outputNodes.m_animatedGeometries.m_opaqueRenderNodesBack );
		DoParseRenderNodes( p_camera, p_inputNodes.m_animatedGeometries.m_transparentRenderNodesFront, p_outputNodes.m_animatedGeometries.m_transparentRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_animatedGeometries.m_transparentRenderNodesBack, p_outputNodes.m_animatedGeometries.m_transparentRenderNodesBack );

		DoParseRenderNodes( p_camera, p_inputNodes.m_billboards.m_opaqueRenderNodesFront, p_outputNodes.m_billboards.m_opaqueRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_billboards.m_opaqueRenderNodesBack, p_outputNodes.m_billboards.m_opaqueRenderNodesBack );
		DoParseRenderNodes( p_camera, p_inputNodes.m_billboards.m_transparentRenderNodesFront, p_outputNodes.m_billboards.m_transparentRenderNodesFront );
		DoParseRenderNodes( p_camera, p_inputNodes.m_billboards.m_transparentRenderNodesBack, p_outputNodes.m_billboards.m_transparentRenderNodesBack );
	}

	void RenderQueue::DoSortRenderNodes( SceneRenderNodes & p_nodes )
	{
		Castor3D::DoSortRenderNodes( *GetOwner(), p_nodes.m_scene, p_nodes.m_staticGeometries, p_nodes.m_instancedGeometries, p_nodes.m_animatedGeometries );
		Castor3D::DoSortRenderNodes( *GetOwner(), p_nodes.m_scene, p_nodes.m_billboards );
	}

	void RenderQueue::OnSceneChanged( Scene const & p_scene )
	{
		m_changedScenes.insert( &p_scene );
	}
}
