#include "RenderDepthQuad.hpp"

#include "Engine.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderDepthQuad::RenderDepthQuad( RenderSystem & renderSystem )
		: RenderQuad{ renderSystem, false, false }
		, m_program{ renderSystem }
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texcoord;
					out.gl_Position() = vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDepth, vtx_texture.xy() ).x() );
					depth = 1.0_f - writer.paren( 1.0_f - depth ) * 25.0f;
					pxl_fragColor = vec4( depth, depth, depth, 1.0 );
				} );
			pxl = writer.finalise();
		}

		m_program.setSource( renderer::ShaderStageFlag::eVertex, vtx );
		m_program.setSource( renderer::ShaderStageFlag::eFragment, pxl );
	}

	RenderDepthQuad::~RenderDepthQuad()
	{
	}

	void RenderDepthQuad::initialise()
	{
		m_program.initialise();
	}

	void RenderDepthQuad::render( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		static renderer::ClearColorValue const clear{ 0.0, 0.0, 0.0, 0.0 };
		cleanup();
		auto & device = *m_renderSystem.getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		createPipeline( { size.getWidth(), size.getHeight() }
			, position
			, m_program.getStates()
			, texture.getDefaultView()
			, renderPass
			, {}
			, {}
			, renderer::DepthStencilState{ 0u, false, false } );
		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );
		m_commandBuffer->beginRenderPass( renderPass
			, frameBuffer
			, { clear }
			, renderer::SubpassContents::eInline );
		registerFrame( *m_commandBuffer );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();

		auto fence = device.createFence();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, fence.get() );
		fence->wait( renderer::FenceTimeout );
	}
}
