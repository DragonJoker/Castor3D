#include "LumaEdgeDetection.hpp"

#include "SmaaUbo.hpp"
#include "SMAA.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Core/Renderer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, bool predicationEnabled
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();
			writer.enableExtension( cuT( "GL_ARB_texture_gather" ), 400u );

			// Shader inputs
			writeConstants( writer, config, renderTargetMetrics );
			writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			writer.declConstant( constants::LocalContrastAdaptationFactor
				, Float( config.data.localContrastAdaptationFactor ) );
			writer.declConstant( constants::Predication
				, predicationEnabled ? 1_i : 0_i
				, predicationEnabled );
			writer.declConstant( constants::PredicationThreshold
				, Float( config.data.predicationThreshold )
				, predicationEnabled );
			writer.declConstant( constants::PredicationScale
				, Float( config.data.predicationScale )
				, predicationEnabled );
			writer.declConstant( constants::PredicationStrength
				, Float( config.data.predicationStrength )
				, predicationEnabled );
			writer << getLumaEdgeDetectionPS();

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto c3d_colourTex = writer.declSampler< Sampler2D >( cuT( "c3d_colourTex" ), 0u, 0u );
			auto c3d_predicationTex = writer.declSampler< Sampler2D >( cuT( "c3d_predicationTex" ), 1u, 0u, predicationEnabled );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );

					if ( predicationEnabled )
					{
						writer << "pxl_fragColour.xy = SMAALumaEdgeDetectionPS( vtx_texture, vtx_offset, c3d_colourTex, c3d_predicationTex )" << endi;
					}
					else
					{
						writer << "pxl_fragColour.xy = SMAALumaEdgeDetectionPS( vtx_texture, vtx_offset, c3d_colourTex )" << endi;
					}
				} );
			return writer.finalise();
		}

		ashes::TextureViewPtr doCreatePredicationView( ashes::Texture const & texture )
		{
			ashes::ImageViewCreateInfo view{};
			view.format = texture.getFormat();
			view.viewType = ashes::TextureViewType::e2D;
			view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
			view.subresourceRange.baseArrayLayer = 0u;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return texture.createView( view );
		}
	}

	//*********************************************************************************************

	LumaEdgeDetection::LumaEdgeDetection( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & colourView
		, ashes::Texture const * predication
		, SmaaConfig const & config )
		: EdgeDetection{ renderTarget, config }
		, m_colourView{ colourView }
		, m_predicationView{ predication ? doCreatePredicationView( *predication ) : nullptr }
	{
		ashes::Extent2D size{ renderTarget.getSize().getWidth()
			, renderTarget.getSize().getHeight() };
		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_pixelShader = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, m_predicationView != nullptr
			, config );
		doInitialisePipeline();
	}

	castor3d::CommandsSemaphore LumaEdgeDetection::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore edgeDetectionCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & edgeDetectionCmd = *edgeDetectionCommands.commandBuffer;

		edgeDetectionCmd.begin();
		timer.beginPass( edgeDetectionCmd, passIndex );
		// Put source image in shader input layout.
		edgeDetectionCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_colourView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		if ( m_predicationView )
		{
			edgeDetectionCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, m_predicationView->makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		}

		edgeDetectionCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { ashes::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 }, ashes::DepthStencilClearValue{ 1.0f, 0 } }
			, ashes::SubpassContents::eInline );
		registerFrame( edgeDetectionCmd );
		edgeDetectionCmd.endRenderPass();
		timer.endPass( edgeDetectionCmd, passIndex );
		edgeDetectionCmd.end();

		return std::move( edgeDetectionCommands );
	}

	void LumaEdgeDetection::doInitialisePipeline()
	{
		ashes::Extent2D size{ m_colourView.getTexture().getDimensions().width
			, m_colourView.getTexture().getDimensions().height };
		auto & device = getCurrentDevice( m_renderSystem );
		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		stages[1].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );

		ashes::DepthStencilState dsstate{ 0u, false, false };
		dsstate.stencilTestEnable = true;
		dsstate.front.passOp = ashes::StencilOp::eReplace;
		dsstate.front.reference = 1u;
		dsstate.back = dsstate.front;
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		auto * view = &m_colourView;

		if ( m_predicationView )
		{
			setLayoutBindings.emplace_back( 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
			view = m_predicationView.get();
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
	}

	void LumaEdgeDetection::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		if ( m_predicationView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
				, m_colourView
				, m_sampler->getSampler() );
		}
	}
}
