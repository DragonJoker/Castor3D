#include "Castor3D/Render/ToTexture/RenderDepthQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	RenderDepthQuad::RenderDepthQuad( RenderSystem & renderSystem )
		: RenderQuad{ renderSystem, false, false }
		, m_program{ renderSystem }
	{
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "RenderDepthQuad" };
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
					out.gl_out.gl_Position = vec4( position.x(), position.y(), 0.0_f, 1.0_f );
				} );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "RenderDepthQuad" };
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
					depth = 1.0_f - ( 1.0_f - depth ) * 25.0f;
					pxl_fragColor = vec4( depth, depth, depth, 1.0_f );
				} );
			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		m_program.setSource( VK_SHADER_STAGE_VERTEX_BIT, std::move( vtx.shader ) );
		m_program.setSource( VK_SHADER_STAGE_FRAGMENT_BIT, std::move( pxl.shader ) );
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
		cleanup();
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer();
		createPipeline( { size.getWidth(), size.getHeight() }
			, position
			, m_program.getStates()
			, texture.getDefaultView()
			, renderPass
			, {}
			, {}
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_commandBuffer->beginRenderPass( renderPass
			, frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( *m_commandBuffer );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();

		device.graphicsQueue->submit( *m_commandBuffer, nullptr );
		device.graphicsQueue->waitIdle();
	}
}
