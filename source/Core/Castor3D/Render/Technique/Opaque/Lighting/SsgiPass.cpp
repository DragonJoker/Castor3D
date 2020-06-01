#include "Castor3D/Render/Technique/Opaque/Lighting/SsgiPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/Passes/CombinePass.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Ssgi/SsgiConfig.hpp"
#include "Castor3D/Render/Ssgi/SsgiRawGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

#define C3D_DebugRawPass 0

namespace castor3d
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapLhs = writer.declSampledImage< FImg2DRgba32 >( CombinePass::LhsMap, 0u, 0u );
			auto c3d_mapRhs = writer.declSampledImage< FImg2DRgba32 >( CombinePass::RhsMap, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = texture( c3d_mapLhs, vtx_texture );
					pxl_fragColor += texture( c3d_mapRhs, vtx_texture );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, bool transfer )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo createInfo
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, createInfo
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}
	}

	SsgiPass::SsgiPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfig & ssaoConfig
		, SsgiConfig & ssgiConfig
		, TextureLayout const & linearisedDepth
		, TextureLayoutSPtr scene )
		: m_engine{ engine }
		, m_size{ size.width >> 2, size.height >> 2 }
		, m_ssaoConfig{ ssaoConfig }
		, m_ssgiConfig{ ssgiConfig }
		, m_matrixUbo{ engine }
		, m_hdrCopy{ doCreateTexture( engine, "SsgiHdrCopy", scene->getPixelFormat(), VkExtent2D{ scene->getDimensions().width, scene->getDimensions().height }, true ) }
		, m_hdrCopyCommands
		{
			getCurrentRenderDevice( m_engine ).graphicsCommandPool->createCommandBuffer( "SsgiHdrCopy" ),
			getCurrentRenderDevice( m_engine )->createSemaphore( "SsgiHdrCopy" )
		}
		, m_combineVtx{ VK_SHADER_STAGE_VERTEX_BIT, "SSGI - Combine", getVertexProgram( *engine.getRenderSystem() ) }
		, m_combinePxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSGI - Combine", getPixelProgram( *engine.getRenderSystem() ) }
		, m_rawGiPass{ std::make_shared< SsgiRawGIPass >( engine
			, m_size
			, m_ssgiConfig
			, *m_hdrCopy.getTexture()
			, linearisedDepth ) }
#if !C3D_DebugRawPass
		, m_gaussianBlur{ std::make_shared< GaussianBlur >( engine
			, cuT( "SSGI" )
			, m_rawGiPass->getResult().getTexture()->getDefaultView().getTargetView()
			, ssgiConfig.blurSize.value().value() ) }
		, m_combine{ CombinePassBuilder{}
			.resultTexture( scene )
			.build( engine
				, cuT( "SSGI" )
				, scene->getPixelFormat()
				, VkExtent2D{ scene->getWidth(), scene->getHeight() }
				, m_combineVtx
				, m_combinePxl
				, m_hdrCopy.getTexture()->getDefaultView().getSampledView()
				, m_rawGiPass->getResult().getTexture()->getDefaultView().getSampledView() ) }
#endif
	{
		auto & cmd = *m_hdrCopyCommands.commandBuffer;
		auto copySrc = scene->getDefaultView().getTargetView();
		auto copyDst = m_hdrCopy.getTexture()->getDefaultView().getTargetView();
		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		cmd.beginDebugBlock(
			{
				"SSGI - Copy HDR texture",
				castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		// Copy original scene view to internal one
		// Transition scene view to transfer source
		cmd.memoryBarrier( VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, copySrc.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Transition internal to transfer destination
		cmd.memoryBarrier( VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, copyDst.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		cmd.copyImage( copySrc, copyDst );
		// Transition scene view back to color attachment
		cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, copySrc.makeColourAttachment( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		// And internal to shader input
		cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, copyDst.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		cmd.endDebugBlock();
		cmd.end();
	}

	SsgiPass::~SsgiPass()
	{
	}

	void SsgiPass::update( Camera const & camera )
	{
		m_ssaoConfig.blurRadius.reset();
	}

	ashes::Semaphore const & SsgiPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;

		auto & device = getCurrentRenderDevice( m_engine );
		device.graphicsQueue->submit( *m_hdrCopyCommands.commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_hdrCopyCommands.semaphore
			, nullptr );
		result = m_hdrCopyCommands.semaphore.get();

		result = &m_rawGiPass->compute( *result );
#if !C3D_DebugRawPass
		result = &m_gaussianBlur->blur( *result );
		result = &m_combine->combine( *result );
#endif
		return *result;
	}

	void SsgiPass::accept( PipelineVisitorBase & visitor )
	{
		m_rawGiPass->accept( m_ssgiConfig, visitor );
#if !C3D_DebugRawPass
		m_gaussianBlur->accept( visitor );
		m_combine->accept( visitor );
#endif
	}

	TextureLayout const & SsgiPass::getResult()const
	{
#if C3D_DebugRawPass
		return *m_rawGiPass->getResult().getTexture();
#else
		return m_combine->getResult();
#endif
	}
}
