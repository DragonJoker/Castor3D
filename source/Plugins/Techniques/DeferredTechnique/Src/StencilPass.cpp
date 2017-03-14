#include "StencilPass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//*********************************************************************************************

	namespace
	{
		String DoGetVertexShader( Engine const & p_engine )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_engine.GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_MODEL_MATRIX( l_writer );
			auto vertex = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );

			// Shader outputs
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0 );
			} );

			return l_writer.Finalise();
		}

		String DoGetPixelShader( Engine const & p_engine )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_engine.GetRenderSystem()->CreateGlslWriter();

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
			} );

			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( FrameBuffer & p_frameBuffer
		, RenderBufferAttachment & p_depthAttach )
		: m_frameBuffer{ p_frameBuffer }
		, m_depthAttach{ p_depthAttach }
	{
	}

	void StencilPass::Initialise( Castor3D::VertexBuffer & p_vbo
		, Castor3D::IndexBufferSPtr p_ibo )
	{
		auto & l_engine = *p_vbo.GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		ShaderModel l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();

		m_program = l_engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, l_model, DoGetVertexShader( l_engine ) );
		m_program->SetSource( ShaderType::ePixel, l_model, DoGetPixelShader( l_engine ) );
		m_program->Initialise();

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, p_ibo );

		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( true );
		l_dsstate.SetDepthMask( WritingMask::eZero );
		l_dsstate.SetStencilTest( true );
		l_dsstate.SetStencilReadMask( 0 );
		l_dsstate.SetStencilBackRef( 0 );
		l_dsstate.SetStencilBackFunc( StencilFunc::eAlways );
		l_dsstate.SetStencilBackFailOp( StencilOp::eKeep );
		l_dsstate.SetStencilBackDepthFailOp( StencilOp::eIncrWrap );
		l_dsstate.SetStencilBackPassOp( StencilOp::eKeep );
		l_dsstate.SetStencilFrontRef( 0 );
		l_dsstate.SetStencilFrontFunc( StencilFunc::eAlways );
		l_dsstate.SetStencilFrontFailOp( StencilOp::eKeep );
		l_dsstate.SetStencilFrontDepthFailOp( StencilOp::eDecrWrap );
		l_dsstate.SetStencilFrontPassOp( StencilOp::eKeep );
		RasteriserState l_rsstate;
		l_rsstate.SetCulledFaces( Culling::eNone );
		m_pipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
			, std::move( l_rsstate )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void StencilPass::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_program.reset();
	}

	void StencilPass::Render( uint32_t p_count )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.SetDrawBuffers( FrameBuffer::AttachArray{} );
		m_depthAttach.Clear( BufferComponent::eStencil );
		m_pipeline->Apply();
		m_geometryBuffers->Draw( p_count, 0 );
		m_frameBuffer.Unbind();
	}

	//*********************************************************************************************
}
