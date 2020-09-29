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
	RenderDepthQuad::RenderDepthQuad( RenderSystem & renderSystem
		, RenderDevice const & device )
		: RenderQuad{ renderSystem
			, device
			, "RenderDepthQuad"
			, VK_FILTER_LINEAR
			, { ashes::nullopt
				, RenderQuadConfig::Texcoord{} } }
		, m_program{ "RenderDepthQuad", renderSystem }
	{
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "RenderDepthQuad" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position.x(), position.y(), 0.0_f, 1.0_f );
				} );
			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "RenderDepthQuad" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto depth = writer.declLocale( "depth", texture( c3d_mapDepth, vtx_texture.xy() ).x() );
					depth = 1.0_f - ( 1.0_f - depth ) * 25.0f;
					pxl_fragColor = vec4( depth, depth, depth, 1.0_f );
				} );
			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_program.setSource( VK_SHADER_STAGE_VERTEX_BIT, std::move( vtx.shader ) );
		m_program.setSource( VK_SHADER_STAGE_FRAGMENT_BIT, std::move( pxl.shader ) );
	}

	void RenderDepthQuad::initialise()
	{
		m_program.initialise( m_device );
	}

	void RenderDepthQuad::render( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer
		, Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		cleanup();
		m_commandBuffer = m_device.graphicsCommandPool->createCommandBuffer( "RenderDepthQuad" );
		createPipeline( { size.getWidth(), size.getHeight() }
			, position
			, m_program.getStates()
			, texture.getDefaultView().getSampledView()
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

		m_device.graphicsQueue->submit( *m_commandBuffer, nullptr );
		m_device.graphicsQueue->waitIdle();
	}
}
