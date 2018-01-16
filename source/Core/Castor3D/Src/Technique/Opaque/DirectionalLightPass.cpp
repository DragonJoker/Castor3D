#include "DirectionalLightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;
	}

	//*********************************************************************************************

	DirectionalLightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl )
		: LightPass::Program{ engine, vtx, pxl }
		, m_lightDirection{ m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_direction" ), ShaderType::ePixel ) }
		, m_lightTransform{ m_program->createUniform< UniformType::eMat4x4f >( cuT( "light.m_transform" ), ShaderType::ePixel ) }
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr DirectionalLightPass::Program::doCreatePipeline( bool blend )
	{
		DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( WritingMask::eZero );
		BlendState blstate;

		if ( blend )
		{
			blstate.enableBlend( true );
			blstate.setBlendOp( BlendOperation::eAdd );
			blstate.setSrcBlend( BlendOperand::eOne );
			blstate.setDstBlend( BlendOperand::eOne );
		}

		return m_program->getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, RasteriserState{}
			, std::move( blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void DirectionalLightPass::Program::doBind( Light const & light )
	{
		auto & directionalLight = *light.getDirectionalLight();
		m_lightDirection->setValue( directionalLight.getDirection() );
		m_lightTransform->setValue( directionalLight.getLightSpaceTransform() );
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: LightPass{ engine, frameBuffer, depthAttach, gpInfoUbo, hasShadows }
		, m_viewport{ engine }
	{
		auto declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
		} );

		real data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, declaration );
		m_vertexBuffer->resize( sizeof( data ) );
		uint8_t * buffer = m_vertexBuffer->getData();
		std::memcpy( buffer, data, sizeof( data ) );
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_vertexBuffer->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_viewport.initialise();
	}

	DirectionalLightPass::~DirectionalLightPass()
	{
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_viewport.cleanup();
		m_matrixUbo.getUbo().cleanup();
	}

	void DirectionalLightPass::initialise( Scene const & scene
		, SceneUbo & sceneUbo )
	{
		doInitialise( scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, sceneUbo
			, nullptr );
		m_viewport.update();
	}

	void DirectionalLightPass::cleanup()
	{
		doCleanup();
	}

	uint32_t DirectionalLightPass::getCount()const
	{
		return VertexCount;
	}

	void DirectionalLightPass::doUpdate( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		m_viewport.resize( size );
		m_matrixUbo.update( camera.getView(), m_viewport.getProjection() );
	}

	glsl::Shader DirectionalLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, 0u );
		UBO_GPINFO( writer, 0u );
		auto vertex = writer.declAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_projection * vec4( vertex, 0.0, 1.0 );
		} );

		return writer.finalise();
	}

	LightPass::ProgramPtr DirectionalLightPass::doCreateProgram( glsl::Shader const & vtx
		, glsl::Shader const & pxl )const
	{
		return std::make_unique< Program >( m_engine, vtx, pxl );
	}
}
