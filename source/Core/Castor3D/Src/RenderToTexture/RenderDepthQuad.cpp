#include "RenderDepthQuad.hpp"

#include "Engine.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <RenderPass/ClearValue.hpp>
#include <Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	RenderDepthQuad::RenderDepthQuad( RenderSystem & renderSystem )
		: RenderQuad{ renderSystem, false, false }
		, m_program{ renderSystem }
	{
		ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "RenderDepthQuad" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "RenderDepthQuad" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDepth" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDepth, vtx_texture.xy() ).x() );
					depth = 1.0_f - writer.paren( 1.0_f - depth ) * 25.0f;
					pxl_fragColor = vec4( depth, depth, depth, 1.0 );
				} );
			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		m_program.setSource( ashes::ShaderStageFlag::eVertex, std::move( vtx.shader ) );
		m_program.setSource( ashes::ShaderStageFlag::eFragment, std::move( pxl.shader ) );
	}

	RenderDepthQuad::~RenderDepthQuad()
	{
	}

	void RenderDepthQuad::initialise()
	{
		m_program.initialise();
	}

	void RenderDepthQuad::render( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		static ashes::ClearColorValue const clear{ 0.0, 0.0, 0.0, 0.0 };
		cleanup();
		auto & device = getCurrentDevice( m_renderSystem );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		createPipeline( { size.getWidth(), size.getHeight() }
			, position
			, m_program.getStates()
			, texture.getDefaultView()
			, renderPass
			, {}
			, {}
			, ashes::DepthStencilState{ 0u, false, false } );
		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		m_commandBuffer->beginRenderPass( renderPass
			, frameBuffer
			, { clear }
			, ashes::SubpassContents::eInline );
		registerFrame( *m_commandBuffer );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();

		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
		device.getGraphicsQueue().waitIdle();
	}
}
