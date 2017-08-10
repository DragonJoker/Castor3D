#include "MeshLightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	MeshLightPass::Program::Program( Engine & engine
		, GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )
		: LightPass::Program{ engine, vtx, pxl }
		, m_lightIntensity{ m_program->createUniform< UniformType::eVec2f >( cuT( "light.m_lightBase.m_intensity" ), ShaderType::ePixel ) }
	{
	}

	MeshLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr MeshLightPass::Program::doCreatePipeline( bool blend )
	{
		RasteriserState rsstate;
		rsstate.setCulledFaces( Culling::eFront );

		DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( WritingMask::eZero );
		dsstate.setStencilTest( true );
		dsstate.setStencilFunc( StencilFunc::eNEqual );
		dsstate.setStencilRef( 0u );

		BlendState blstate;

		if ( blend )
		{
			blstate.enableBlend( true );
			blstate.setRgbBlendOp( BlendOperation::eAdd );
			blstate.setRgbSrcBlend( BlendOperand::eOne );
			blstate.setRgbDstBlend( BlendOperand::eOne );
		}

		return m_program->getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, std::move( blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, LightType type
		, bool hasShadows )
		: LightPass{ engine, frameBuffer, depthAttach, gpInfoUbo, hasShadows }
		, m_modelMatrixUbo{ engine }
		, m_stencilPass{ frameBuffer, depthAttach, m_matrixUbo, m_modelMatrixUbo }
		, m_type{ type }
	{
	}

	MeshLightPass::~MeshLightPass()
	{
		m_stencilPass.cleanup();
		m_indexBuffer->cleanup();
		m_vertexBuffer->cleanup();
		m_modelMatrixUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
	}

	void MeshLightPass::initialise( Scene const & scene
		, SceneUbo & sceneUbo )
	{
		auto declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );

		auto data = doGenerateVertices();
		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, declaration );
		auto size = data.size() * sizeof( *data.data() );
		m_vertexBuffer->resize( uint32_t( size ) );
		std::memcpy( m_vertexBuffer->getData()
			, data.data()->constPtr()
			, size );
		m_vertexBuffer->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		auto faces = doGenerateFaces();
		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->resize( uint32_t( faces.size() ) );
		std::memcpy( m_indexBuffer->getData()
			, faces.data()
			, faces.size() * sizeof( *faces.data() ) );
		m_indexBuffer->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		m_stencilPass.initialise( *m_vertexBuffer
			, m_indexBuffer );

		doInitialise( scene
			, m_type
			, *m_vertexBuffer
			, m_indexBuffer
			, sceneUbo
			, &m_modelMatrixUbo );
	}

	void MeshLightPass::cleanup()
	{
		doCleanup();
	}

	uint32_t MeshLightPass::getCount()const
	{
		return m_indexBuffer->getSize();
	}

	void MeshLightPass::doUpdate( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		auto model = doComputeModelMatrix( light, camera );
		m_matrixUbo.update( camera.getView(), camera.getViewport().getProjection() );
		m_modelMatrixUbo.update( model );
		camera.apply();
		m_stencilPass.render( m_indexBuffer->getSize() );
	}
	
	GLSL::Shader MeshLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_GPINFO( writer );
		auto vertex = writer.declAttribute< Vec3 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0_f );
		} );

		return writer.finalise();
	}

	//*********************************************************************************************
}
