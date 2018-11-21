#include "BlendingWeightCalculation.hpp"

#include "AreaTex.h"
#include "SearchTex.h"
#include "SmaaUbo.hpp"
#include "SMAA.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/UniformBuffer.hpp>
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
		glsl::Shader doBlendingWeightCalculationVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			writeConstants( writer, config, renderTargetMetrics );
			writer.declConstant( constants::MaxSearchSteps
				, Int( config.data.maxSearchSteps ) );
			writer << getBlendingWeightCalculationVS();

			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_pixcoord = writer.declOutput< Vec2 >( cuT( "vtx_pixcoord" ), 1u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 2u, 3u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = writer.ashesBottomUpToTopDown( texcoord );
					writer << "SMAABlendingWeightCalculationVS( vtx_texture, vtx_pixcoord, vtx_offset )" << endi;
				} );
			return writer.finalise();
		}

		glsl::Shader doBlendingWeightCalculationFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			writeConstants( writer, config, renderTargetMetrics );
			writer << getBlendingWeightCalculationPS();

			// Shader inputs
			glsl::Ubo ubo{ writer, cuT( "Subsample" ), 0u, 0u };
			auto c3d_subsampleIndices = ubo.declMember< IVec4 >( constants::SubsampleIndices );
			ubo.end();
			auto c3d_areaTex = writer.declSampler< Sampler2D >( cuT( "c3d_areaTex" ), 1u, 0u );
			auto c3d_searchTex = writer.declSampler< Sampler2D >( cuT( "c3d_searchTex" ), 2u, 0u );
			auto c3d_edgesTex = writer.declSampler< Sampler2D >( cuT( "c3d_edgesTex" ), 3u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_pixcoord = writer.declInput< Vec2 >( cuT( "vtx_pixcoord" ), 1u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 2u, 3u );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					writer << "pxl_fragColour = SMAABlendingWeightCalculationPS( vtx_texture, vtx_pixcoord, vtx_offset, c3d_edgesTex, c3d_areaTex, c3d_searchTex, c3d_subsampleIndices )" << endi;
				} );
			return writer.finalise();
		}

		ashes::SamplerPtr doCreateSampler( castor3d::Engine & engine
			, castor::String const & name )
		{
			auto & device = getCurrentDevice( engine );
			ashes::SamplerCreateInfo sampler
			{
				ashes::Filter::eNearest,
				ashes::Filter::eNearest,
				ashes::MipmapMode::eNone,
				ashes::WrapMode::eClampToEdge,
				ashes::WrapMode::eClampToEdge,
				ashes::WrapMode::eClampToEdge,
				0.0f,
				false,
				1.0f,
				false,
				ashes::CompareOp::eNever,
				-1000.0f,
				1000.0f,
				ashes::BorderColour::eFloatOpaqueBlack,
				false
			};
			return device.createSampler( sampler );
		}
	}

	//*********************************************************************************************

	BlendingWeightCalculation::BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & edgeDetectionView
		, castor3d::TextureLayoutSPtr depthView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), false, false }
		, m_edgeDetectionView{ edgeDetectionView }
		, m_surface{ *renderTarget.getEngine() }
		, m_pointSampler{ doCreateSampler( *renderTarget.getEngine(), cuT( "SMAA_Point" ) ) }
	{
		ashes::Extent2D size{ m_edgeDetectionView.getTexture().getDimensions().width
			, m_edgeDetectionView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		m_ubo = ashes::makeUniformBuffer< castor::Point4i >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		ashes::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = size.width;
		image.extent.height = size.height;
		image.extent.depth = 1u;
		image.format = ashes::Format::eR8G8_UNORM;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eSampled
			| ashes::ImageUsageFlag::eTransferDst;

		auto areaTexBuffer = PxBufferBase::create( Size{ AREATEX_WIDTH, AREATEX_HEIGHT }
			, PixelFormat::eA8L8
			, areaTexBytes
			, PixelFormat::eA8L8 );

		m_areaTex = std::make_shared< castor3d::TextureLayout >( renderSystem
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_areaTex->getDefaultImage().initialiseSource( areaTexBuffer );
		m_areaTex->initialise();

		auto searchTexBuffer = PxBufferBase::create( Size{ SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT }
			, PixelFormat::eL8
			, searchTexBytes
			, PixelFormat::eL8 );
		image.format = ashes::Format::eR8_UNORM;

		m_searchTex = std::make_shared< castor3d::TextureLayout >( renderSystem
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_searchTex->getDefaultImage().initialiseSource( searchTexBuffer );
		m_searchTex->initialise();

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = ashes::Format::eR8G8B8A8_UNORM;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.attachments[1].format = ashes::Format::eD24_UNORM_S8_UINT;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 1u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doBlendingWeightCalculationVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader = doBlendingWeightCalculationFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );

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
		dsstate.front.compareOp = ashes::CompareOp::eEqual;
		dsstate.front.reference = 1u;
		dsstate.back = dsstate.front;
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( 2u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );

		createPipeline( size
			, castor::Position{}
			, stages
			, m_edgeDetectionView
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, ashes::Format::eR8G8B8A8_UNORM
			, depthView );
	}

	castor3d::CommandsSemaphore BlendingWeightCalculation::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore blendingWeightCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & blendingWeightCmd = *blendingWeightCommands.commandBuffer;

		blendingWeightCmd.begin();
		timer.beginPass( blendingWeightCmd, passIndex );
		// Put edge detection image in shader input layout.
		blendingWeightCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_edgeDetectionView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		blendingWeightCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { ashes::ClearColorValue{}, ashes::DepthStencilClearValue{ 1.0f, 0 } }
			, ashes::SubpassContents::eInline );
		registerFrame( blendingWeightCmd );
		blendingWeightCmd.endRenderPass();
		timer.endPass( blendingWeightCmd, passIndex );
		blendingWeightCmd.end();

		return std::move( blendingWeightCommands );
	}

	void BlendingWeightCalculation::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	void BlendingWeightCalculation::update( castor::Point4i const & subsampleIndices )
	{
		auto & data = m_ubo->getData();
		data = Point4i{ subsampleIndices };
		m_ubo->upload();
	}

	void BlendingWeightCalculation::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, *m_ubo );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_areaTex->getDefaultView()
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_searchTex->getDefaultView()
			, *m_pointSampler );
	}

	//*********************************************************************************************
}
