#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderTechnique.hpp"

#include "AnimatedObjectGroupCache.hpp"
#include "BillboardCache.hpp"
#include "CameraCache.hpp"
#include "DepthStencilStateCache.hpp"
#include "Engine.hpp"
#include "GeometryCache.hpp"
#include "LightCache.hpp"
#include "OverlayCache.hpp"
#include "RasteriserStateCache.hpp"
#include "SamplerCache.hpp"
#include "ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "PostEffect/PostEffect.hpp"
#include "Render/Context.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
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
#include "State/DepthStencilState.hpp"
#include "State/RasteriserState.hpp"
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
						p_function( *l_itPrograms.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}
			}
		}

		template< typename NodeType, typename OpaqueMapType, typename TransparentMapType = OpaqueMapType >
		void DoAddRenderNode( PassSPtr p_pass, ShaderProgramSPtr p_program, NodeType const & p_node, RenderTechnique::stRENDER_NODES< NodeType, OpaqueMapType, TransparentMapType > & p_nodes )
		{
			if ( p_pass->HasAlphaBlending() )
			{
				using ObjectRenderNodesArray = typename TransparentMapType::mapped_type;
				auto l_itProgram = p_nodes.m_transparentRenderNodes.insert( { p_program, ObjectRenderNodesArray() } ).first;
				l_itProgram->second.push_back( p_node );
			}
			else
			{
				using ObjectRenderNodesArray = typename OpaqueMapType::mapped_type;
				auto l_itProgram = p_nodes.m_opaqueRenderNodes.insert( { p_program, ObjectRenderNodesArray() } ).first;
				l_itProgram->second.push_back( p_node );
			}
		}

		template< typename NodeType >
		void DoAddRenderNode( PassSPtr p_pass, ShaderProgramSPtr p_program, NodeType const & p_node, SubmeshSPtr p_object, RenderTechnique::stRENDER_NODES< NodeType, SubmeshStaticRenderNodesByProgramMap, SubmeshStaticRenderNodesByProgramMap > & p_nodes )
		{
			using ObjectRenderNodesByProgramMap = SubmeshStaticRenderNodesByProgramMap::mapped_type;
			using ObjectRenderNodesByPassMap = ObjectRenderNodesByProgramMap::mapped_type;
			using ObjectRenderNodesArray = ObjectRenderNodesByPassMap::mapped_type;

			if ( p_pass->HasAlphaBlending() )
			{
				auto l_itProgram = p_nodes.m_transparentRenderNodes.insert( { p_program, ObjectRenderNodesByProgramMap() } ).first;
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

		void DoSortRenderNodes( RenderTechnique const & p_technique,
								Scene & p_scene,
								RenderTechnique::stRENDER_NODES< StaticGeometryRenderNode, StaticGeometryRenderNodesByProgramMap > & p_static,
								RenderTechnique::stRENDER_NODES< StaticGeometryRenderNode, SubmeshStaticRenderNodesByProgramMap, SubmeshStaticRenderNodesByProgramMap > & p_instanced,
								RenderTechnique::stRENDER_NODES< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByProgramMap > & p_animated )
		{
			p_static.m_opaqueRenderNodes.clear();
			p_static.m_transparentRenderNodes.clear();
			p_instanced.m_opaqueRenderNodes.clear();
			p_instanced.m_transparentRenderNodes.clear();
			p_animated.m_opaqueRenderNodes.clear();
			p_animated.m_transparentRenderNodes.clear();
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
									{
										{
											{
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
										*l_primitive.second,
										l_submesh->GetGeometryBuffers( *l_program ),
										*l_submesh,
										*l_sceneNode,
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
									{
										{
											{
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
										*l_primitive.second,
										l_submesh->GetGeometryBuffers( *l_program ),
										*l_submesh,
										*l_sceneNode
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

		void DoSortRenderNodes( RenderTechnique const & p_technique, Scene & p_scene, RenderTechnique::stRENDER_NODES< BillboardRenderNode, BillboardRenderNodesByProgramMap > & p_nodes )
		{
			p_nodes.m_opaqueRenderNodes.clear();
			p_nodes.m_transparentRenderNodes.clear();
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
							{
								{
									{
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
								*l_billboard.second,
								l_billboard.second->GetGeometryBuffers( *l_program ),
								*l_sceneNode,
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

		template< typename MapType, typename NodeType >
		void DoResortAlpha( MapType p_input, Camera const & p_camera, std::multimap< double, NodeType > & p_output )
		{
			p_output.clear();
			
			for ( auto l_itPrograms : p_input )
			{
				for ( auto & l_renderNode : l_itPrograms.second )
				{
					if ( l_renderNode.m_sceneNode.IsDisplayable() && l_renderNode.m_sceneNode.IsVisible() )
					{
						Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
						Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
						Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
						l_renderNode.m_data.SortByDistance( l_ptCameraLocal );
						l_ptCameraLocal -= l_renderNode.m_sceneNode.GetPosition();
						p_output.insert( std::make_pair( point::distance_squared( l_ptCameraLocal ), l_renderNode ) );
					}
				}
			}
		}

		template< typename NodeType >
		void DoResortAlpha( SubmeshStaticRenderNodesByProgramMap p_input, Camera const & p_camera, std::multimap< double, NodeType > & p_output )
		{
			p_output.clear();
			
			DoTraverseNodes( p_input, [&p_camera, &p_output]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
			{
				for ( auto & l_renderNode : p_renderNodes )
				{
					if ( l_renderNode.m_sceneNode.IsDisplayable() && l_renderNode.m_sceneNode.IsVisible() )
					{
						Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
						Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
						Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
						l_renderNode.m_data.SortByDistance( l_ptCameraLocal );
						l_ptCameraLocal -= l_renderNode.m_sceneNode.GetPosition();
						p_output.insert( std::make_pair( point::distance_squared( l_ptCameraLocal ), l_renderNode ) );
					}
				}
			} );
		}

		template< typename NodeType, typename OpaqueMapType, typename TransparentMapType = OpaqueMapType >
		void DoRenderAlphaNodes( Scene & p_scene
								, RenderTechnique::stRENDER_NODES< NodeType, OpaqueMapType, TransparentMapType > & p_nodes
								, Pipeline & p_pipeline
								, Camera & p_camera
								, RasteriserStateSPtr p_frontRS
								, RasteriserStateSPtr p_backRS
								, DepthStencilStateSPtr p_noDepthWriteDS )
		{
			if ( !p_nodes.m_transparentRenderNodes.empty() )
			{
				DoResortAlpha( p_nodes.m_transparentRenderNodes, p_camera, p_nodes.m_distanceSortedRenderNodes );
				p_noDepthWriteDS->Apply();
				p_frontRS->Apply();
				p_nodes.m_distanceRender( p_scene, p_pipeline, p_nodes.m_distanceSortedRenderNodes );
				p_backRS->Apply();
				p_nodes.m_distanceRender( p_scene, p_pipeline, p_nodes.m_distanceSortedRenderNodes );
			}
		}

		template< typename NodeType, typename OpaqueMapType, typename TransparentMapType = OpaqueMapType >
		void DoRenderAlphaNodesMS( Scene & p_scene
								  , RenderTechnique::stRENDER_NODES< NodeType, OpaqueMapType, TransparentMapType > & p_nodes
								  , Pipeline & p_pipeline
								  , RasteriserStateSPtr p_frontRS
								  , RasteriserStateSPtr p_backRS )
		{
			if ( !p_nodes.m_transparentRenderNodes.empty() )
			{
				p_frontRS->Apply();
				p_nodes.m_renderTransparent( p_scene, p_pipeline, p_nodes.m_transparentRenderNodes );
				p_backRS->Apply();
				p_nodes.m_renderTransparent( p_scene, p_pipeline, p_nodes.m_transparentRenderNodes );
			}
		}

		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene, Pipeline & p_pipeline, MapType & p_nodes )
		{
			for ( auto l_itPrograms : p_nodes )
			{
				for ( auto & l_renderNode : l_itPrograms.second )
				{
					l_renderNode.Render( p_scene, p_pipeline );
				}
			}
		}

		template< typename NodeType >
		void DoRenderByDistance( Scene & p_scene, Pipeline & p_pipeline, std::multimap< double, NodeType > & p_nodes )
		{
			for ( auto & l_it : p_nodes )
			{
				l_it.second.Render( p_scene, p_pipeline );
			}
		}
	}

	//*************************************************************************************************

	RenderTechnique::stFRAME_BUFFER::stFRAME_BUFFER( RenderTechnique & p_technique )
		: m_technique{ p_technique }
	{
	}

	bool RenderTechnique::stFRAME_BUFFER::Initialise( Size p_size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_colourTexture->GetImage().SetSource( p_size, ePIXEL_FORMAT_ARGB16F32F );
		p_size = m_colourTexture->GetImage().GetDimensions();

		bool l_return = m_colourTexture->Create();

		if ( l_return )
		{
			l_return = m_colourTexture->Initialise();

			if ( !l_return )
			{
				m_colourTexture->Destroy();
			}
		}

		if ( l_return )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH32F );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( p_size );

			if ( !l_return )
			{
				m_depthBuffer->Destroy();
			}
		}

		if ( l_return )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			l_return = m_frameBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( p_size );

			if ( l_return && m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
			{
				m_frameBuffer->Attach( eATTACHMENT_POINT_COLOUR, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_depthAttach );
				l_return = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return l_return;
	}

	void RenderTechnique::stFRAME_BUFFER::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_colourTexture->Destroy();
			m_depthBuffer->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ), bool p_multisampling )
		: OwnedBy< Engine >{ *p_renderSystem.GetEngine() }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
		, m_renderSystem{ p_renderSystem }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_multisampling{ p_multisampling }
	{
		auto l_rsState = GetEngine()->GetRasteriserStateCache().Add( cuT( "RenderTechnique_" ) + p_name + cuT( "_Front" ) );
		l_rsState->SetCulledFaces( eFACE_FRONT );
		m_wpFrontRasteriserState = l_rsState;
		l_rsState = GetEngine()->GetRasteriserStateCache().Add( cuT( "RenderTechnique_" ) + p_name + cuT( "_Back" ) );
		l_rsState->SetCulledFaces( eFACE_BACK );
		m_wpBackRasteriserState = l_rsState;
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	bool RenderTechnique::Create()
	{
		return DoCreate();
	}

	void RenderTechnique::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechnique::Initialise( uint32_t & p_index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.GetSize();
			m_initialised = DoInitialise( p_index );

			if ( m_initialised )
			{
				m_initialised = m_frameBuffer.Initialise( m_size );
			}

			if ( m_initialised )
			{
				m_initialised = m_wpFrontRasteriserState.lock()->Initialise();
			}

			if ( m_initialised )
			{
				m_initialised = m_wpBackRasteriserState.lock()->Initialise();
			}
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_initialised = false;
		m_scenesRenderNodes.clear();
		m_wpBackRasteriserState.lock()->Cleanup();
		m_wpBackRasteriserState.lock()->Cleanup();
		m_frameBuffer.Cleanup();
		DoCleanup();
	}

	void RenderTechnique::Update()
	{
		for ( auto & l_sceneNodes : m_scenesRenderNodes )
		{
			if ( l_sceneNodes.second.m_scene.HasChanged() )
			{
				DoSortRenderNodes( l_sceneNodes.second );
			}
		}

		for ( auto & l_scene : m_newScenes )
		{
			auto l_pair = m_scenesRenderNodes.insert(
			{
				l_scene->GetName(),
				{
					*l_scene,
					{
						std::bind( &RenderTechnique::DoRenderStaticSubmeshesNonInstanced, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
						std::bind( &RenderTechnique::DoRenderStaticSubmeshesByDistance, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
					},
					{
						std::bind( &RenderTechnique::DoRenderInstancedSubmeshesInstanced, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
						[]( Scene &, Pipeline &, std::multimap< double, StaticGeometryRenderNode > & ){},
					},
					{
						std::bind( &RenderTechnique::DoRenderAnimatedSubmeshesNonInstanced, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
						std::bind( &RenderTechnique::DoRenderAnimatedSubmeshesByDistance, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
					},
					{
						std::bind( &RenderTechnique::DoRenderBillboards, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
						std::bind( &RenderTechnique::DoRenderBillboardsByDistance, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ),
					},
				}
			} );

			if ( l_pair.second )
			{
				DoSortRenderNodes( l_pair.first->second );
			}
		}

		m_newScenes.clear();
	}

	void RenderTechnique::AddScene( Scene & p_scene )
	{
		m_newScenes.push_back( &p_scene );
	}

	void RenderTechnique::Render( Scene & p_scene, Camera & p_camera, uint32_t p_frameTime )
	{
		auto l_it = m_scenesRenderNodes.find( p_scene.GetName() );

		if ( l_it != m_scenesRenderNodes.end() )
		{
			m_renderSystem.PushScene( &p_scene );

			if ( DoBeginRender( p_scene ) )
			{
				p_scene.RenderBackground( GetSize(), m_renderSystem.GetCurrentContext()->GetPipeline() );
				DoRender( m_scenesRenderNodes.find( p_scene.GetName() )->second, p_camera, p_frameTime );
				DoEndRender( p_scene );
			}

			for ( auto l_effect : m_renderTarget.GetPostEffects() )
			{
				l_effect->Apply( *m_frameBuffer.m_frameBuffer );
			}

			m_renderSystem.PopScene();
		}
	}

	String RenderTechnique::GetPixelShaderSource( uint32_t p_flags )const
	{
		return DoGetPixelShaderSource( p_flags );
	}
	
	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}

	void RenderTechnique::DoSortRenderNodes( stSCENE_RENDER_NODES & p_nodes )
	{
		Castor3D::DoSortRenderNodes( *this, p_nodes.m_scene, p_nodes.m_staticGeometries, p_nodes.m_instancedGeometries, p_nodes.m_animatedGeometries );
		Castor3D::DoSortRenderNodes( *this, p_nodes.m_scene, p_nodes.m_billboards );
	}

	void RenderTechnique::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, StaticGeometryRenderNodesByProgramMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, AnimatedGeometryRenderNodesByProgramMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRenderBillboards( Scene & p_scene, Pipeline & p_pipeline, BillboardRenderNodesByProgramMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRenderInstancedSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, SubmeshStaticRenderNodesByProgramMap & p_nodes )
	{
		DoTraverseNodes( p_nodes, [&p_scene, &p_pipeline]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto & l_renderNode : p_renderNodes )
			{
				l_renderNode.Render( p_scene, p_pipeline );
			}
		} );
	}

	void RenderTechnique::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Pipeline & p_pipeline, SubmeshStaticRenderNodesByProgramMap & p_nodes )
	{
		DoTraverseNodes( p_nodes, [&p_scene, &p_pipeline]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() )
			{
				auto l_count = p_submesh.GetRefCount( p_pass.GetParent() );
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_size = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, ( l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse() ).const_ptr(), l_size );
					l_buffer += l_size;
				}

				p_renderNodes[0].BindPass( p_scene, p_pipeline, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( p_scene );
			}
		} );
	}

	void RenderTechnique::DoRenderStaticSubmeshesByDistance( Scene & p_scene, Pipeline & p_pipeline, StaticGeometryRenderNodeByDistanceMMap & p_nodes )
	{
		DoRenderByDistance( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRenderAnimatedSubmeshesByDistance( Scene & p_scene, Pipeline & p_pipeline, AnimatedGeometryRenderNodeByDistanceMMap & p_nodes )
	{
		DoRenderByDistance( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRenderBillboardsByDistance( Scene & p_scene, Pipeline & p_pipeline, BillboardRenderNodeByDistanceMMap & p_nodes )
	{
		DoRenderByDistance( p_scene, p_pipeline, p_nodes );
	}

	void RenderTechnique::DoRender( Size const & p_size, stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		auto & l_pipeline = m_renderSystem.GetCurrentContext()->GetPipeline();
		p_camera.GetViewport().Resize( p_size );
		p_camera.Render( l_pipeline );
		DoRenderOpaqueNodes( p_nodes, l_pipeline, p_camera );
		p_nodes.m_scene.RenderForeground( p_size, p_camera, l_pipeline );
		DoRenderTransparentNodes( p_nodes, l_pipeline, p_camera );
		p_camera.EndRender();
	}

	void RenderTechnique::DoRenderOpaqueNodes( stSCENE_RENDER_NODES & p_nodes, Pipeline & p_pipeline, Camera & p_camera )
	{
		auto l_rsBack = m_wpBackRasteriserState.lock();

		if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_billboards.m_opaqueRenderNodes.empty() )
		{
			l_rsBack->Apply();

			if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodes.empty() )
			{
				DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_pipeline, p_nodes.m_staticGeometries.m_opaqueRenderNodes );
			}

			if ( !p_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty() )
			{
				if ( m_renderSystem.GetGpuInformations().HasInstancing() )
				{
					DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_pipeline, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
				}
				else
				{
					DoRenderInstancedSubmeshesNonInstanced( p_nodes.m_scene, p_pipeline, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
				}
			}

			if ( !p_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty() )
			{
				DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_pipeline, p_nodes.m_animatedGeometries.m_opaqueRenderNodes );
			}

			if ( !p_nodes.m_billboards.m_opaqueRenderNodes.empty() )
			{
				DoRenderBillboards( p_nodes.m_scene, p_pipeline, p_nodes.m_billboards.m_opaqueRenderNodes );
			}
		}
	}

	void RenderTechnique::DoRenderTransparentNodes( stSCENE_RENDER_NODES & p_nodes, Pipeline & p_pipeline, Camera & p_camera )
	{
		auto l_rsBack = m_wpBackRasteriserState.lock();

		if ( !p_nodes.m_staticGeometries.m_transparentRenderNodes.empty()
			|| !p_nodes.m_animatedGeometries.m_transparentRenderNodes.empty()
			|| !p_nodes.m_billboards.m_transparentRenderNodes.empty() )
		{
			auto l_context = m_renderSystem.GetCurrentContext();
			auto l_rsFront = m_wpFrontRasteriserState.lock();
			auto l_dsNoDepthWrite = l_context->GetNoDepthWriteState ();

			if ( l_context->IsMultiSampling() )
			{
				DoRenderAlphaNodesMS( p_nodes.m_scene, p_nodes.m_staticGeometries, p_pipeline, l_rsFront, l_rsBack );
				DoRenderAlphaNodesMS( p_nodes.m_scene, p_nodes.m_instancedGeometries, p_pipeline, l_rsFront, l_rsBack );
				DoRenderAlphaNodesMS( p_nodes.m_scene, p_nodes.m_animatedGeometries, p_pipeline, l_rsFront, l_rsBack );
				DoRenderAlphaNodesMS( p_nodes.m_scene, p_nodes.m_billboards, p_pipeline, l_rsFront, l_rsBack );
			}
			else
			{
				DoRenderAlphaNodes( p_nodes.m_scene, p_nodes.m_staticGeometries, p_pipeline, p_camera, l_rsFront, l_rsBack, l_dsNoDepthWrite );
				DoRenderAlphaNodes( p_nodes.m_scene, p_nodes.m_animatedGeometries, p_pipeline, p_camera, l_rsFront, l_rsBack, l_dsNoDepthWrite );
				DoRenderAlphaNodes( p_nodes.m_scene, p_nodes.m_billboards, p_pipeline, p_camera, l_rsFront, l_rsBack, l_dsNoDepthWrite );
			}
		}
		else
		{
			// Since Skybox culls front faces, it is necessary to switch back to back face culling when no mesh has transparency.
			l_rsBack->Apply();
		}
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_flags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		auto vtx_vertex( l_writer.GetInput< Vec3 >( cuT( "vtx_vertex" ) ) );
		auto vtx_normal( l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) ) );
		auto vtx_tangent( l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) ) );
		auto vtx_bitangent( l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) ) );
		auto vtx_texture( l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_flags, TextureChannel::Colour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_flags, TextureChannel::Ambient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_flags, TextureChannel::Diffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_flags, TextureChannel::Normal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_flags, TextureChannel::Opacity ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_flags, TextureChannel::Specular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_flags, TextureChannel::Emissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_flags, TextureChannel::Height ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_flags, TextureChannel::Gloss ) ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		// Fragment Outtputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.SWIZZLE_X, vtx_normal.SWIZZLE_Y, vtx_normal.SWIZZLE_Z ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.SWIZZLE_X, c3d_v3CameraPosition.SWIZZLE_Y, c3d_v3CameraPosition.SWIZZLE_Z ) );
			pxl_v4FragColor = vec4( Float( 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			if ( p_flags != 0 )
			{
				if ( CheckFlag( p_flags, TextureChannel::Normal ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ );
					l_v3MapNormal = Float( &l_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &l_writer, 1 ), 1.0, 1.0 );
					l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
				}

				if ( CheckFlag( p_flags, TextureChannel::Gloss ) )
				{
					l_fMatShininess = texture2D( c3d_mapGloss, vtx_texture.SWIZZLE_XY ).SWIZZLE_R;
				}

				if ( CheckFlag( p_flags, TextureChannel::Emissive ) )
				{
					l_v3Emissive = texture2D( c3d_mapEmissive, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}
			}

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.SWIZZLE_X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.SWIZZLE_Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.SWIZZLE_Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											  l_output );
			}
			ROF;

			if ( CheckFlag( p_flags, TextureChannel::Opacity ) )
			{
				l_fAlpha = texture2D( c3d_mapOpacity, vtx_texture.SWIZZLE_XY ).SWIZZLE_R * c3d_fMatOpacity;
			}

			if ( p_flags && p_flags != uint32_t( TextureChannel::Opacity ) )
			{
				if ( CheckFlag( p_flags, TextureChannel::Colour ) )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Ambient ) )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Diffuse ) )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Specular ) )
				{
					l_v3Specular *= texture2D( c3d_mapSpecular, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}
			}

			pxl_v4FragColor = vec4( l_writer.Paren( l_v3Ambient * c3d_v4MatAmbient.SWIZZLE_XYZ ) +
									l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.SWIZZLE_XYZ ) +
									l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.SWIZZLE_XYZ ) +
									l_v3Emissive, l_fAlpha );
		} );

		return l_writer.Finalise();
	}
}
