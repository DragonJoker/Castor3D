#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderPass::RenderPass( String const & p_name, Engine & p_engine, bool p_multisampling )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_multisampling{ p_multisampling }
		, m_renderQueue{ *this }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	String RenderPass::GetPixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		String l_return;

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			l_return = DoGetTransparentPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}
		else
		{
			l_return = DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}

		return l_return;
	}

	void RenderPass::PreparePipeline( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t & p_programFlags, uint8_t p_sceneFlags, bool p_twoSided )
	{
		DoCompleteProgramFlags( p_programFlags );
		auto l_backProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, false );

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
			auto l_flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };
			DoPrepareTransparentFrontPipeline( *l_frontProgram, l_flags );
			DoPrepareTransparentBackPipeline( *l_backProgram, l_flags );
		}
		else
		{
			auto l_flags = PipelineFlags{ p_colourBlendMode, BlendMode::NoBlend, p_textureFlags, p_programFlags, p_sceneFlags };

			if ( p_twoSided )
			{
				auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
				DoPrepareOpaqueFrontPipeline( *l_frontProgram, l_flags );
			}

			DoPrepareOpaqueBackPipeline( *l_backProgram, l_flags );
		}
	}

	Pipeline & RenderPass::GetOpaquePipelineFront( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_frontOpaquePipelines.find( { p_colourBlendMode, BlendMode::NoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_frontOpaquePipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetOpaquePipelineBack( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_backOpaquePipelines.find( { p_colourBlendMode, BlendMode::NoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_backOpaquePipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineFront( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_frontTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_frontTransparentPipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineBack( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_backTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_backTransparentPipelines.end() );
		return *l_it->second;
	}

	AnimatedGeometryRenderNode RenderPass::CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )
	{
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode RenderPass::CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )
	{
		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardList & p_billboard )
	{
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( *p_billboard.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_billboard.GetParent(),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i )
		};
	}

	void RenderPass::UpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateOpaquePipeline( p_camera, p_pipeline, p_depthMaps );
	}

	void RenderPass::UpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateTransparentPipeline( p_camera, p_pipeline, p_depthMaps );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & p_pass, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_passBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferPass );
		FrameVariableBufferSPtr l_matrixBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
		FrameVariableBufferSPtr l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		Point4rFrameVariableSPtr l_pt4r;
		OneFloatFrameVariableSPtr l_1f;

		auto l_node = PassRenderNode
		{
			p_pass,
			p_pipeline,
			*l_matrixBuffer,
			*l_passBuffer,
			*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
			*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
		};
		p_pass.FillRenderNode( l_node );
		return l_node;
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & p_scene, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_sceneBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferScene );
		Point3rFrameVariableSPtr l_pt3r;

		return SceneRenderNode
		{
			*l_sceneBuffer,
			*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
		};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const
	{
		ShaderProgramSPtr l_program;

		if ( CheckFlag( p_programFlags, ProgramFlag::Billboards ) )
		{
			l_program = GetEngine()->GetShaderProgramCache().GetBillboardProgram( p_textureFlags, p_programFlags, p_sceneFlags );

			if ( !l_program )
			{
				l_program = GetEngine()->GetRenderSystem()->CreateBillboardsProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags );
				GetEngine()->GetShaderProgramCache().AddBillboardProgram( l_program, p_textureFlags, p_programFlags, p_sceneFlags );
			}
		}
		else
		{
			l_program = GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
		}

		return l_program;
	}
}
