#include "StencilPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/MatrixUbo.hpp>
#include <Shader/ModelMatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		GLSL::Shader DoGetVertexShader( Engine const & engine )
		{
			using namespace GLSL;
			GlslWriter writer = engine.GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_MODEL_MATRIX( writer );
			auto vertex = writer.DeclAttribute< Vec3 >( ShaderProgram::Position );

			// Shader outputs
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0 );
			} );

			return writer.Finalise();
		}

		GLSL::Shader DoGetPixelShader( Engine const & engine )
		{
			using namespace GLSL;
			GlslWriter writer = engine.GetRenderSystem()->CreateGlslWriter();

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
			} );

			return writer.Finalise();
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, MatrixUbo & matrixUbo
		, ModelMatrixUbo & modelMatrixUbo )
		: m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_matrixUbo{ matrixUbo }
		, m_modelMatrixUbo{ modelMatrixUbo }
	{
	}

	void StencilPass::Initialise( Castor3D::VertexBuffer & vbo
		, Castor3D::IndexBufferSPtr ibo )
	{
		auto & engine = *vbo.GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();

		m_program = engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, DoGetVertexShader( engine ) );
		m_program->SetSource( ShaderType::ePixel, DoGetPixelShader( engine ) );
		m_program->Initialise();

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { vbo }, ibo.get() );

		DepthStencilState dsstate;
		dsstate.SetDepthTest( true );
		dsstate.SetDepthMask( WritingMask::eZero );
		dsstate.SetStencilTest( true );
		dsstate.SetStencilReadMask( 0 );
		dsstate.SetStencilRef( 0 );
		dsstate.SetStencilFunc( StencilFunc::eAlways );
		dsstate.SetStencilBackOps( StencilOp::eKeep, StencilOp::eIncrWrap, StencilOp::eKeep );
		dsstate.SetStencilFrontOps( StencilOp::eKeep, StencilOp::eDecrWrap, StencilOp::eKeep );
		RasteriserState rsstate;
		rsstate.SetCulledFaces( Culling::eNone );
		m_pipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
	}

	void StencilPass::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_program.reset();
	}

	void StencilPass::Render( uint32_t count )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.SetDrawBuffers( FrameBuffer::AttachArray{} );
		m_depthAttach.Clear( 0 );
		m_pipeline->Apply();
		m_geometryBuffers->Draw( count, 0 );
		m_frameBuffer.Unbind();
	}

	//*********************************************************************************************
}
