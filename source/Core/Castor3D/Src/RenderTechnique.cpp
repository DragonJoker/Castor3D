#include "RenderTechnique.hpp"

#include "AnimatedObject.hpp"
#include "CameraManager.hpp"
#include "ColourRenderBuffer.hpp"
#include "Context.hpp"
#include "DepthStencilState.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "FrameBuffer.hpp"
#include "FrameVariableBuffer.hpp"
#include "GeometryManager.hpp"
#include "LightManager.hpp"
#include "Material.hpp"
#include "OverlayManager.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "PostEffect.hpp"
#include "RasteriserState.hpp"
#include "RenderBufferAttachment.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "SamplerManager.hpp"
#include "Scene.hpp"
#include "ShaderManager.hpp"
#include "Submesh.hpp"
#include "TextureAttachment.hpp"
#include "VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename FuncT >
		void DoTraverseNodes( SubmeshRenderNodesByProgramMap & p_nodes, FuncT p_function )
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
	}

	RenderTechnique::RenderTechnique( String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ) )
		: OwnedBy< Engine >( *p_renderSystem->GetEngine() )
		, m_renderTarget( &p_renderTarget )
		, m_renderSystem( p_renderSystem )
		, m_name( p_name )
		, m_initialised( false )
	{
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	bool RenderTechnique::Create()
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoCreate();
		}

		return l_return;
	}

	void RenderTechnique::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechnique::Initialise( uint32_t & p_index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget->GetSize();
			m_initialised = DoInitialise( p_index );
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_scenesRenderNodes.clear();
		m_initialised = false;
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
	}

	void RenderTechnique::AddScene( Scene & p_scene )
	{
		auto l_pair = m_scenesRenderNodes.insert( { p_scene.GetName(), { p_scene } } );

		if ( l_pair.second )
		{
			DoSortRenderNodes( l_pair.first->second );
		}
	}

	bool RenderTechnique::BeginRender()
	{
		return DoBeginRender();
	}

	bool RenderTechnique::Render( Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		m_renderSystem->PushScene( &p_scene );
		return DoRender( m_scenesRenderNodes.find( p_scene.GetName() )->second, p_camera, p_dFrameTime );
	}

	void RenderTechnique::EndRender()
	{
		DoEndRender();

		for ( auto && l_effect : m_renderTarget->GetPostEffects() )
		{
			l_effect->Apply();
		}

		GetEngine()->GetOverlayManager().Render( *m_renderSystem->GetTopScene(), m_renderTarget->GetSize() );
		m_renderTarget->GetFrameBuffer()->Unbind();
		m_renderSystem->PopScene();
	}

	String RenderTechnique::GetPixelShaderSource( uint32_t p_flags )const
	{
		return DoGetPixelShaderSource( p_flags );
	}

	void RenderTechnique::DoSortRenderNodes( stSCENE_RENDER_NODES & p_nodes )
	{
		p_nodes.m_renderNodes.clear();
		p_nodes.m_opaqueRenderNodes.clear();
		p_nodes.m_transparentRenderNodes.clear();

		auto l_lock = make_unique_lock( p_nodes.m_scene.GetGeometryManager() );

		for ( auto && l_primitive : p_nodes.m_scene.GetGeometryManager() )
		{
			MeshSPtr l_mesh = l_primitive.second->GetMesh();
			SceneNodeSPtr l_sceneNode = l_primitive.second->GetParent();

			if ( l_mesh )
			{
				for ( auto && l_submesh : *l_mesh )
				{
					MaterialSPtr l_material( l_primitive.second->GetMaterial( l_submesh ) );

					if ( l_material )
					{
						for ( auto l_pass : *l_material )
						{
							ShaderProgramSPtr l_program;
							uint32_t l_programFlags = l_submesh->GetProgramFlags();

							if ( !l_primitive.second->GetAnimatedObject() )
							{
								l_programFlags &= ~ePROGRAM_FLAG_SKINNING;
							}

							l_program = GetEngine()->GetShaderManager().GetAutomaticProgram( *this, l_pass->GetTextureFlags(), l_programFlags );

							auto l_sceneBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
							auto l_passBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferPass );
							Point4rFrameVariableSPtr l_pt4r;
							Point3rFrameVariableSPtr l_pt3r;
							OneFloatFrameVariableSPtr l_1f;
							OneTextureFrameVariableSPtr l_1tex;

							GeometryRenderNode l_renderNode =
							{
								*l_primitive.second,
								*l_submesh,
								*l_sceneNode,
								*l_sceneBuffer,
								*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r ),
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
								}
							};

							l_pass->BindToNode( l_renderNode.m_scene );

							if ( l_material->HasAlphaBlending() )
							{
								auto l_itProgram = p_nodes.m_transparentRenderNodes.insert( { l_program, SubmeshRenderNodesByPassMap() } ).first;
								auto l_itMap = l_itProgram->second.insert( { l_pass, SubmeshRenderNodesMap() } ).first;
								auto l_itSubmesh = l_itMap->second.insert( { l_submesh, GeometryRenderNodeArray() } ).first;
								l_itSubmesh->second.push_back( l_renderNode );
							}
							else
							{
								auto l_itProgram = p_nodes.m_opaqueRenderNodes.insert( { l_program, SubmeshRenderNodesByPassMap() } ).first;
								auto l_itMap = l_itProgram->second.insert( { l_pass, SubmeshRenderNodesMap() } ).first;
								auto l_itSubmesh = l_itMap->second.insert( { l_submesh, GeometryRenderNodeArray() } ).first;
								l_itSubmesh->second.push_back( l_renderNode );
							}

							auto l_itProgram = p_nodes.m_renderNodes.insert( { l_program, SubmeshRenderNodesByPassMap() } ).first;
							auto l_itMap = l_itProgram->second.insert( { l_pass, SubmeshRenderNodesMap() } ).first;
							auto l_itSubmesh = l_itMap->second.insert( { l_submesh, GeometryRenderNodeArray() } ).first;
							l_itSubmesh->second.push_back( l_renderNode );
						}
					}
				}
			}
		}
	}

	void RenderTechnique::DoBindPass( Scene & p_scene, Pipeline & p_pipeline, GeometryRenderNode & p_node, uint64_t p_excludedMtxFlags )
	{
		if ( GetEngine()->GetPerObjectLighting() )
		{
			p_scene.GetLightManager().BindLights( p_node.m_scene.m_node.m_program, p_node.m_scene.m_sceneUbo );
			p_scene.GetCameraManager().BindCamera( p_node.m_scene.m_sceneUbo );
		}

		p_pipeline.ApplyMatrices( p_node.m_scene.m_node.m_matrixUbo, ( 0xFFFFFFFFFFFFFFFF & ~p_excludedMtxFlags ) );
		auto l_animated = p_node.m_geometry.GetAnimatedObject();

		if ( l_animated )
		{
			Matrix4x4rFrameVariableSPtr l_variable;
			p_node.m_scene.m_node.m_matrixUbo.GetVariable( Pipeline::MtxBones, l_variable );

			if ( l_variable )
			{
				l_animated->FillShader( *l_variable );
			}
		}

		p_node.m_scene.m_node.m_pass.FillShaderVariables( p_node.m_scene.m_node );
		p_node.m_scene.m_node.m_program.Bind();
		p_node.m_scene.m_node.m_pass.Render();
	}

	void RenderTechnique::DoUnbindPass( Scene & p_scene, GeometryRenderNode & p_renderNode )
	{
		p_renderNode.m_scene.m_node.m_pass.EndRender();
		p_renderNode.m_scene.m_node.m_program.Unbind();

		if ( GetEngine()->GetPerObjectLighting() )
		{
			p_scene.GetLightManager().UnbindLights( p_renderNode.m_scene.m_node.m_program, p_renderNode.m_scene.m_sceneUbo );
		}
	}

	void RenderTechnique::DoRenderSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes )
	{
		DoTraverseNodes( p_nodes, [this, &p_scene, &p_camera, &p_pipeline]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, GeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto l_renderNode : p_renderNodes )
			{
				p_pipeline.SetModelMatrix( l_renderNode.m_sceneNode.GetDerivedTransformationMatrix() );
				DoBindPass( p_scene, p_pipeline, l_renderNode, 0 );
				l_renderNode.m_submesh.Draw( l_renderNode.m_scene.m_node.m_program );
				DoUnbindPass( p_scene, l_renderNode );
			}
		} );
	}

	void RenderTechnique::DoRenderSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes )
	{
		DoTraverseNodes( p_nodes, [this, &p_scene, &p_camera, &p_pipeline]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, GeometryRenderNodeArray & p_renderNodes )
		{
			auto l_count = p_submesh.GetRefCount( p_pass.GetParent() );

			if ( l_count > 1 && p_submesh.HasMatrixBuffer()
				 && ( p_submesh.GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) != ePROGRAM_FLAG_SKINNING )
			{
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_size = 16 * sizeof( real );

				for ( auto && l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, ( l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse() ).const_ptr(), l_size );
					l_buffer += l_size;
				}

				DoBindPass( p_scene, p_pipeline, p_renderNodes[0], MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_scene.m_node.m_program, l_count );
				DoUnbindPass( p_scene, p_renderNodes[0] );
			}
			else
			{
				for ( auto l_renderNode : p_renderNodes )
				{
					p_pipeline.SetModelMatrix( l_renderNode.m_sceneNode.GetDerivedTransformationMatrix() );
					DoBindPass( p_scene, p_pipeline, l_renderNode, 0 );
					l_renderNode.m_submesh.Draw( l_renderNode.m_scene.m_node.m_program );
					DoUnbindPass( p_scene, l_renderNode );
				}
			}
		} );
	}

	void RenderTechnique::DoRenderSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, RenderNodeByDistanceMMap & p_nodes )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		for ( auto l_it : p_nodes )
		{
			p_pipeline.SetModelMatrix( l_it.second.m_sceneNode.GetDerivedTransformationMatrix() );
			DoBindPass( p_scene, p_pipeline, l_it.second, 0 );
			l_it.second.m_submesh.Draw( l_it.second.m_scene.m_node.m_program );
			DoUnbindPass( p_scene, l_it.second );
		}
	}

	void RenderTechnique::DoResortAlpha( SubmeshRenderNodesByProgramMap p_input, Camera const & p_camera, RenderNodeByDistanceMMap & p_output )
	{
		p_output.clear();

		DoTraverseNodes( p_input, [this, &p_camera, &p_output]( ShaderProgram & p_program, Pass & p_pass, Submesh & p_submesh, GeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto l_renderNode : p_renderNodes )
			{
				if ( l_renderNode.m_sceneNode.IsDisplayable() && l_renderNode.m_sceneNode.IsVisible() )
				{
					if ( p_camera.IsVisible( l_renderNode.m_submesh.GetParent()->GetCollisionBox(), l_renderNode.m_sceneNode.GetDerivedTransformationMatrix() ) )
					{
						Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
						Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
						Point3r l_ptCameraLocal = l_mtxMeshGlobal * l_position;
						l_renderNode.m_submesh.SortFaces( l_ptCameraLocal );
						l_ptCameraLocal -= l_renderNode.m_submesh.GetCubeBox().GetCenter();
						p_output.insert( std::make_pair( point::distance_squared( l_ptCameraLocal ), l_renderNode ) );
					}
				}
			}
		} );
	}

	void RenderTechnique::DoRenderSubmeshes( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes )
	{
		if ( GetEngine()->GetRenderSystem()->HasInstancing() )
		{
			DoRenderSubmeshesInstanced( p_scene, p_camera, p_pipeline, p_nodes );
		}
		else
		{
			DoRenderSubmeshesNonInstanced( p_scene, p_camera, p_pipeline, p_nodes );
		}
	}

	bool RenderTechnique::DoRender( Size const & p_size, stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, double p_dFrameTime )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();
		Pipeline & l_pipeline = l_renderSystem->GetPipeline();
		ContextRPtr l_context = l_renderSystem->GetCurrentContext();

		p_camera.GetViewport().SetSize( p_size );
		p_camera.Render();

		if ( !p_nodes.m_opaqueRenderNodes.empty() )
		{
			l_context->CullFace( eFACE_BACK );
			DoRenderSubmeshes( p_nodes.m_scene, p_camera, l_pipeline, p_nodes.m_opaqueRenderNodes );
		}

		if ( !p_nodes.m_transparentRenderNodes.empty() )
		{
			if ( l_context->IsMultiSampling() )
			{
				l_context->CullFace( eFACE_FRONT );
				DoRenderSubmeshes( p_nodes.m_scene, p_camera, l_pipeline, p_nodes.m_transparentRenderNodes );
				l_context->CullFace( eFACE_BACK );
				DoRenderSubmeshes( p_nodes.m_scene, p_camera, l_pipeline, p_nodes.m_transparentRenderNodes );
			}
			else
			{
				l_context->GetNoDepthWriteState()->Apply();
				DoResortAlpha( p_nodes.m_transparentRenderNodes, p_camera, p_nodes.m_distanceSortedTransparentRenderNodes );
				l_context->CullFace( eFACE_FRONT );
				DoRenderSubmeshesNonInstanced( p_nodes.m_scene, p_camera, l_pipeline, p_nodes.m_distanceSortedTransparentRenderNodes );
				l_context->CullFace( eFACE_BACK );
				DoRenderSubmeshesNonInstanced( p_nodes.m_scene, p_camera, l_pipeline, p_nodes.m_distanceSortedTransparentRenderNodes );
			}
		}

		p_camera.EndRender();
		return true;
	}
}
