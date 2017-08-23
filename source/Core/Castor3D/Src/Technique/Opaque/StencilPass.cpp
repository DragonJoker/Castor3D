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
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Shader/Ubos/ModelMatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		glsl::Shader doGetVertexShader( Engine const & engine )
		{
			using namespace glsl;
			GlslWriter writer = engine.getRenderSystem()->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_MODEL_MATRIX( writer );
			auto vertex = writer.declAttribute< Vec3 >( ShaderProgram::Position );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0 );
			} );

			return writer.finalise();
		}

		glsl::Shader doGetPixelShader( Engine const & engine )
		{
			using namespace glsl;
			GlslWriter writer = engine.getRenderSystem()->createGlslWriter();

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
			} );

			return writer.finalise();
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

	void StencilPass::initialise( castor3d::VertexBuffer & vbo
		, castor3d::IndexBufferSPtr ibo )
	{
		m_vbo = &vbo;
		auto & engine = *vbo.getEngine();
		auto & renderSystem = *engine.getRenderSystem();

		m_program = engine.getShaderProgramCache().getNewProgram( false );
		m_program->createObject( ShaderType::eVertex );
		m_program->createObject( ShaderType::ePixel );
		m_program->setSource( ShaderType::eVertex, doGetVertexShader( engine ) );
		m_program->setSource( ShaderType::ePixel, doGetPixelShader( engine ) );
		m_program->initialise();

		m_geometryBuffers = m_program->getRenderSystem()->createGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->initialise( { vbo }, ibo.get() );

		DepthStencilState dsstate;
		dsstate.setDepthTest( true );
		dsstate.setDepthMask( WritingMask::eZero );
		dsstate.setStencilTest( true );
		dsstate.setStencilReadMask( 0 );
		dsstate.setStencilRef( 0 );
		dsstate.setStencilFunc( StencilFunc::eAlways );
		dsstate.setStencilBackOps( StencilOp::eKeep, StencilOp::eIncrWrap, StencilOp::eKeep );
		dsstate.setStencilFrontOps( StencilOp::eKeep, StencilOp::eDecrWrap, StencilOp::eKeep );
		RasteriserState rsstate;
		rsstate.setCulledFaces( Culling::eNone );
		m_pipeline = m_program->getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_modelMatrixUbo.getUbo() );
	}

	void StencilPass::cleanup()
	{
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_program.reset();
	}

	void StencilPass::render( uint32_t count )
	{
		m_frameBuffer.bind( FrameBufferTarget::eDraw );
		m_frameBuffer.setDrawBuffers( FrameBuffer::AttachArray{} );
		m_depthAttach.clear( 0 );
		m_pipeline->apply();
		m_geometryBuffers->draw( count, 0u );
		m_frameBuffer.unbind();
	}

	//*********************************************************************************************
}
