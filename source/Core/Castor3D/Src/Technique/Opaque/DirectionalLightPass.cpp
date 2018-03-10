#include "DirectionalLightPass.hpp"

#include "Engine.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/ShaderProgram.hpp"

#include <Buffer/UniformBuffer.hpp>

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
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr DirectionalLightPass::Program::doCreatePipeline( bool blend )
	{
		if ( !m_ubo )
		{
			m_ubo = renderer::makeUniformBuffer< Config >( *m_engine.getRenderSystem()->getCurrentDevice()
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_baseUbo = &m_ubo->getUbo();
		}

		renderer::DepthStencilState dsstate
		{
			0u,
			false,
			false
		};
		renderer::ColourBlendState blstate;

		if ( blend )
		{
			blstate.addAttachment( renderer::ColourBlendStateAttachment
			{
				true,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
			} );
		}

		return std::make_unique< RenderPipeline >( m_engine.getRenderSystem()
			, std::move( dsstate )
			, renderer::RasterisationState{}
			, std::move( blstate )
			, renderer::MultisampleState{}
			, m_program
			, PipelineFlags{} );
	}

	void DirectionalLightPass::Program::doBind( Light const & light )
	{
		auto & directionalLight = *light.getDirectionalLight();
		auto & data = m_ubo->getData( 0u );
		data.base.colour = light.getColour();
		data.base.intensity = light.getIntensity();
		data.base.farPlane = light.getFarPlane();
		data.direction = directionalLight.getDirection();
		data.transform = directionalLight.getLightSpaceTransform();
		m_ubo->upload();
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & engine
		, renderer::FrameBuffer & frameBuffer
		, renderer::TextureView & depthView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: LightPass{ engine, frameBuffer, depthView, gpInfoUbo, hasShadows }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
	}

	DirectionalLightPass::~DirectionalLightPass()
	{
		m_vertexBuffer.reset();
		m_matrixUbo.cleanup();
	}

	void DirectionalLightPass::initialise( Scene const & scene
		, SceneUbo & sceneUbo )
	{
		float data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_vertexBuffer = renderer::makeVertexBuffer< float >( device
			, 12u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, 12u
			, renderer::MemoryMapFlag::eInvalidateRange | renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, data, sizeof( data ) );
			m_vertexBuffer->unlock( 12u, true );
		}

		m_vertexLayout = renderer::makeLayout< float >( device, 0u );
		m_vertexLayout->createAttribute< castor::Point2f >( 0u, 0u );
		doInitialise( scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, *m_vertexLayout
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
		UBO_MATRIX( writer, 0u, 0u );
		UBO_GPINFO( writer, 1u, 0u );
		auto vertex = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

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
