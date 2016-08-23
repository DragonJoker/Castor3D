#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderQueue.hpp"

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
			for ( auto l_itPrograms : p_nodes )
			{
				for ( auto l_itPass : l_itPrograms.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( l_itPrograms.first, l_itPass.first, l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeType, typename MapType >
		void DoAddRenderNode( ShaderProgramSPtr p_program
							  , NodeType const & p_node
							  , MapType & p_nodes )
		{
			using ObjectRenderNodesArray = typename MapType::mapped_type;
			auto l_itProgram = p_nodes.insert( { p_program, ObjectRenderNodesArray() } ).first;
			l_itProgram->second.push_back( p_node );
		}

		template< typename NodeType, typename OpaqueMapType, typename TransparentMapType = OpaqueMapType >
		void DoAddRenderNode( PassSPtr p_pass
							  , ShaderProgramSPtr p_program
							  , NodeType const & p_node
							  , RenderNodesT< NodeType, OpaqueMapType, TransparentMapType > & p_nodes )
		{
			if ( p_pass->HasAlphaBlending() )
			{
				DoAddRenderNode( p_program, p_node, p_nodes.m_transparentRenderNodes[size_t( p_pass->GetAlphaBlendMode() )] );
			}
			else
			{
				DoAddRenderNode( p_program, p_node, p_nodes.m_opaqueRenderNodes );
			}
		}

		template< typename NodeType >
		void DoAddRenderNode( PassSPtr p_pass
							  , ShaderProgramSPtr p_program
							  , NodeType const & p_node
							  , SubmeshSPtr p_object
							  , RenderNodesT< NodeType, SubmeshStaticRenderNodesByProgramMap, SubmeshStaticRenderNodesByProgramMap > & p_nodes )
		{
			using ObjectRenderNodesByProgramMap = SubmeshStaticRenderNodesByProgramMap::mapped_type;
			using ObjectRenderNodesByPassMap = ObjectRenderNodesByProgramMap::mapped_type;
			using ObjectRenderNodesArray = ObjectRenderNodesByPassMap::mapped_type;

			if ( p_pass->HasAlphaBlending() )
			{
				auto l_itProgram = p_nodes.m_transparentRenderNodes[size_t( p_pass->GetAlphaBlendMode() )].insert( { p_program, ObjectRenderNodesByProgramMap() } ).first;
				auto l_itPass = l_itProgram->second.insert( { p_pass, ObjectRenderNodesByPassMap() } ).first;
				auto l_itObject = l_itPass->second.insert( { p_object, ObjectRenderNodesArray() } ).first;
				l_itObject->second.push_back( p_node );
			}
			else
			{
				auto l_itProgram = p_nodes.m_opaqueRenderNodes.insert( { p_program, ObjectRenderNodesByProgramMap() } ).first;
				auto l_itPass = l_itProgram->second.insert( { p_pass, ObjectRenderNodesByPassMap() } ).first;
				auto l_itObject = l_itPass->second.insert( { p_object, ObjectRenderNodesArray() } ).first;
				l_itObject->second.push_back( p_node );
			}
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

		void DoSortRenderNodes( RenderTechnique const & p_technique
								, Scene & p_scene
								, RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByProgramMap > & p_static
								, RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByProgramMap, SubmeshStaticRenderNodesByProgramMap > & p_instanced
								, RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByProgramMap > & p_animated )
		{
			p_static.m_opaqueRenderNodes.clear();
			p_instanced.m_opaqueRenderNodes.clear();
			p_animated.m_opaqueRenderNodes.clear();

			for ( size_t i = 0; i < p_static.m_transparentRenderNodes.size(); ++i )
			{
				p_static.m_transparentRenderNodes[i].clear();
				p_instanced.m_transparentRenderNodes[i].clear();
				p_animated.m_transparentRenderNodes[i].clear();
			}

			auto l_lock = make_unique_lock( p_scene.GetGeometryCache() );

			for ( auto l_primitive : p_scene.GetGeometryCache() )
			{
				MeshSPtr l_mesh = l_primitive.second->GetMesh();
				SceneNodeSPtr l_sceneNode = l_primitive.second->GetParent();

				if ( l_mesh && l_sceneNode )
				{
					for ( auto l_submesh : *l_mesh )
					{
						MaterialSPtr l_material( l_primitive.second->GetMaterial( l_submesh ) );

						if ( l_material )
						{
							for ( auto l_pass : *l_material )
							{
								ShaderProgramSPtr l_program;
								uint32_t l_programFlags = l_submesh->GetProgramFlags();
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

								if ( l_submesh->GetRefCount( l_material ) > 1
									 && !l_mesh
									 && !l_skeleton
									 && ( !l_material->HasAlphaBlending()
										  || p_technique.IsMultisampling() ) )
								{
									AddFlag( l_programFlags, ProgramFlag::Instantiation );
								}

								l_pass->PrepareTextures();
								l_program = p_scene.GetEngine()->GetShaderProgramCache().GetAutomaticProgram( p_technique, l_pass->GetTextureFlags(), l_programFlags );

								auto l_sceneBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
								auto l_passBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferPass );
								auto l_animationBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
								Point4rFrameVariableSPtr l_pt4r;
								Point3rFrameVariableSPtr l_pt3r;
								OneFloatFrameVariableSPtr l_1f;

								if ( CheckFlag( l_programFlags, ProgramFlag::Skinning )
									 || CheckFlag( l_programFlags, ProgramFlag::Morphing ) )
								{
									AnimatedGeometryRenderNode l_renderNode
									{	// AnimatedGeometryRenderNode
										{	// SceneRenderNode
											{	// RenderNode
												*l_pass,
												*l_program,
												*l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
												*l_passBuffer,
												*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
												*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
											},
											*l_sceneBuffer,
											*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
										},
										l_submesh->GetGeometryBuffers( *l_program ),
										*l_sceneNode,
										*l_submesh,
										*l_primitive.second,
										l_skeleton.get(),
										l_mesh.get(),
										*l_animationBuffer
									};

									l_pass->BindToNode( l_renderNode.m_scene );
									DoAddRenderNode( l_pass, l_program, l_renderNode, p_animated );
								}
								else
								{
									StaticGeometryRenderNode l_renderNode
									{	// StaticGeometryRenderNode
										{	// SceneRenderNode
											{	// RenderNode
												*l_pass,
												*l_program,
												*l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
												*l_passBuffer,
												*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
												*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
												*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
											},
											*l_sceneBuffer,
											*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
										},
										l_submesh->GetGeometryBuffers( *l_program ),
										*l_sceneNode,
										*l_submesh,
										*l_primitive.second,
									};

									l_pass->BindToNode( l_renderNode.m_scene );

									if ( CheckFlag( l_programFlags, ProgramFlag::Instantiation ) )
									{
										DoAddRenderNode( l_pass, l_program, l_renderNode, l_submesh, p_instanced );
									}
									else
									{
										DoAddRenderNode( l_pass, l_program, l_renderNode, p_static );
									}
								}
							}
						}
					}
				}
			}
		}

		void DoSortRenderNodes( RenderTechnique const & p_technique
								, Scene & p_scene
								, RenderNodesT< BillboardRenderNode, BillboardRenderNodesByProgramMap > & p_nodes )
		{
			p_nodes.m_opaqueRenderNodes.clear();

			for ( size_t i = 0; i < p_nodes.m_transparentRenderNodes.size(); ++i )
			{
				p_nodes.m_transparentRenderNodes[i].clear();
			}

			auto l_lock = make_unique_lock( p_scene.GetBillboardListCache() );

			for ( auto l_billboard : p_scene.GetBillboardListCache() )
			{
				SceneNodeSPtr l_sceneNode = l_billboard.second->GetParent();

				if ( l_sceneNode )
				{
					MaterialSPtr l_material( l_billboard.second->GetMaterial() );

					if ( l_material )
					{
						for ( auto l_pass : *l_material )
						{
							l_pass->PrepareTextures();
							ShaderProgramSPtr l_program = p_scene.GetEngine()->GetShaderProgramCache().GetBillboardProgram( l_pass->GetTextureFlags(), uint32_t( ProgramFlag::Billboards ) );

							if ( !l_program )
							{
								l_program = p_scene.GetEngine()->GetRenderSystem()->CreateBillboardsProgram( p_technique, l_pass->GetTextureFlags() );
								p_scene.GetEngine()->GetShaderProgramCache().AddBillboardProgram( l_program, l_pass->GetTextureFlags(), uint32_t( ProgramFlag::Billboards ) );
							}

							auto l_sceneBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
							auto l_passBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferPass );
							auto l_billboardBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
							Point4rFrameVariableSPtr l_pt4r;
							Point3rFrameVariableSPtr l_pt3r;
							Point2iFrameVariableSPtr l_pt2i;
							OneFloatFrameVariableSPtr l_1f;

							BillboardRenderNode l_renderNode =
							{	// BillboardRenderNode
								{	// SceneRenderNode
									{	// RenderNode
										*l_pass,
										*l_program,
										*l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
										*l_passBuffer,
										*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
										*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
										*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
										*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
										*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
										*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
									},
									*l_sceneBuffer,
									*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
								},
								l_billboard.second->GetGeometryBuffers( *l_program ),
								*l_sceneNode,
								*l_billboard.second,
								*l_billboardBuffer,
								*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i )
							};

							l_pass->BindToNode( l_renderNode.m_scene );
							DoAddRenderNode( l_pass, l_program, l_renderNode, p_nodes );
						}
					}
				}
			}
		}
	}

	//*************************************************************************************************

	RenderQueue::RenderQueue( RenderTechnique & p_technique )
		: OwnedBy< RenderTechnique >{ p_technique }
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
			auto l_itNodes = m_scenesRenderNodes.insert( { &p_scene, { p_scene } } ).first;
			auto l_itNew = std::find( m_newScenes.begin(), m_newScenes.end(), &p_scene );
			bool l_new = false;

			if ( l_itNew != m_newScenes.end() )
			{
				DoSortRenderNodes( l_itNodes->second );
				m_newScenes.erase( l_itNew );
				l_new = true;
			}
			else if ( p_scene.HasChanged() )
			{
				DoSortRenderNodes( l_itNodes->second );
			}

			if ( p_scene.HasChanged()
				 || l_new
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

	void RenderQueue::AddScene( Scene & p_scene )
	{
		m_newScenes.push_back( &p_scene );
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

	void RenderQueue::DoPrepareRenderNodes( Camera const & p_camera, SceneRenderNodes const & p_inputNodes, SceneRenderNodes & p_outputNodes )
	{
		p_outputNodes.m_instancedGeometries.m_opaqueRenderNodes.clear();
		p_outputNodes.m_staticGeometries.m_opaqueRenderNodes.clear();
		p_outputNodes.m_animatedGeometries.m_opaqueRenderNodes.clear();
		p_outputNodes.m_billboards.m_opaqueRenderNodes.clear();

		for ( size_t i = 0u; i < p_outputNodes.m_instancedGeometries.m_transparentRenderNodes.size(); ++i )
		{
			p_outputNodes.m_instancedGeometries.m_transparentRenderNodes[i].clear();
			p_outputNodes.m_staticGeometries.m_transparentRenderNodes[i].clear();
			p_outputNodes.m_animatedGeometries.m_transparentRenderNodes[i].clear();
			p_outputNodes.m_billboards.m_transparentRenderNodes[i].clear();
		}

		auto l_checkVisible = [&p_camera]( SceneNode & p_node, auto const & p_data )
		{
			return p_node.IsDisplayable()
				   && p_node.IsVisible()
				   && p_camera.IsVisible( p_data.GetCollisionSphere(), p_node.GetDerivedTransformationMatrix() );
		};

		DoTraverseNodes( p_inputNodes.m_instancedGeometries.m_opaqueRenderNodes
						 , [&p_camera, &p_outputNodes, &l_checkVisible]( ShaderProgramSPtr p_program
																		 , PassSPtr p_pass
																		 , SubmeshSPtr p_submesh
																		 , StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto const & l_node : p_renderNodes )
			{
				if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
				{
					DoAddRenderNode( p_pass, p_program, l_node, p_submesh, p_outputNodes.m_instancedGeometries );
				}
			}
		} );

		size_t l_index = 0u;

		for ( auto & l_blendModes : p_inputNodes.m_instancedGeometries.m_transparentRenderNodes )
		{
			DoTraverseNodes( l_blendModes
							 , [&p_camera, &p_outputNodes, &l_checkVisible]( ShaderProgramSPtr p_program
																			 , PassSPtr p_pass
																			 , SubmeshSPtr p_submesh
																			 , StaticGeometryRenderNodeArray & p_renderNodes )
			{
				for ( auto const & l_node : p_renderNodes )
				{
					if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
					{
						DoAddRenderNode( p_pass, p_program, l_node, p_submesh, p_outputNodes.m_instancedGeometries );
					}
				}
			} );
		}

		for ( auto & l_programs : p_inputNodes.m_staticGeometries.m_opaqueRenderNodes )
		{
			for ( auto const & l_node : l_programs.second )
			{
				if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
				{
					DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_staticGeometries.m_opaqueRenderNodes );
				}
			}
		}

		l_index = 0;

		for ( auto & l_blendModes : p_inputNodes.m_staticGeometries.m_transparentRenderNodes )
		{
			for ( auto & l_programs : l_blendModes )
			{
				for ( auto const & l_node : l_programs.second )
				{
					if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
					{
						DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_staticGeometries.m_transparentRenderNodes[l_index] );
					}
				}
			}

			++l_index;
		}

		for ( auto & l_programs : p_inputNodes.m_animatedGeometries.m_opaqueRenderNodes )
		{
			for ( auto const & l_node : l_programs.second )
			{
				if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
				{
					DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_animatedGeometries.m_opaqueRenderNodes );
				}
			}
		}

		l_index = 0;

		for ( auto & l_blendModes : p_inputNodes.m_animatedGeometries.m_transparentRenderNodes )
		{
			for ( auto & l_programs : l_blendModes )
			{
				for ( auto const & l_node : l_programs.second )
				{
					if ( l_checkVisible( l_node.m_sceneNode, l_node.m_data ) )
					{
						DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_animatedGeometries.m_transparentRenderNodes[l_index] );
					}
				}
			}

			++l_index;
		}

		for ( auto & l_programs : p_inputNodes.m_billboards.m_opaqueRenderNodes )
		{
			for ( auto const & l_node : l_programs.second )
			{
				if ( l_node.m_sceneNode.IsDisplayable()
					 && l_node.m_sceneNode.IsVisible() )
				{
					DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_billboards.m_opaqueRenderNodes );
				}
			}
		}

		l_index = 0;

		for ( auto & l_blendModes : p_inputNodes.m_billboards.m_transparentRenderNodes )
		{
			for ( auto & l_programs : l_blendModes )
			{
				for ( auto const & l_node : l_programs.second )
				{
					if ( l_node.m_sceneNode.IsDisplayable()
						 && l_node.m_sceneNode.IsVisible() )
					{
						DoAddRenderNode( l_programs.first, l_node, p_outputNodes.m_billboards.m_transparentRenderNodes[l_index] );
					}
				}
			}

			++l_index;
		}
	}

	void RenderQueue::DoSortRenderNodes( SceneRenderNodes & p_nodes )
	{
		Castor3D::DoSortRenderNodes( *GetOwner(), p_nodes.m_scene, p_nodes.m_staticGeometries, p_nodes.m_instancedGeometries, p_nodes.m_animatedGeometries );
		Castor3D::DoSortRenderNodes( *GetOwner(), p_nodes.m_scene, p_nodes.m_billboards );
	}
}
