#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "Miscellaneous/GaussianBlur.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Castor3DPrerequisites.hpp"
#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine, Size const & size )
		{
			String const name = cuT( "ShadowMap_Directional_Variance" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToBorder );
				sampler->setWrapT( renderer::WrapMode::eClampToBorder );
				sampler->setWrapR( renderer::WrapMode::eClampToBorder );
				sampler->setBorderColour( renderer::BorderColour::eFloatOpaqueWhite );
			}

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = renderer::Format::eR32G32_SFLOAT;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialiseDepth( Engine & engine, Size const & size )
		{
			String const name = cuT( "ShadowMap_Directional_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = renderer::Format::eR32_SFLOAT;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize } )
			, doInitialiseDepth( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize } )
			, { std::make_shared< ShadowMapPassDirectional >( engine, scene, *this ) } }
	{
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_passes[0]->update( camera, queues, light, index );
	}


	void ShadowMapDirectional::render( renderer::Semaphore const & toWait )
	{
		static renderer::ClearColorValue const black{ 0.0f, 0.0f, 0.0f, 1.0f };
		static renderer::DepthStencilClearValue const zero{ 1.0f, 0 };

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_passes[0]->startTimer( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { zero, black, black }
			, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_passes[0]->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_passes[0]->stopTimer( *m_commandBuffer );
			m_commandBuffer->end();
		}

		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eBottomOfPipe
			, *m_finished
			, nullptr );
		m_blur->blur();
	}

	void ShadowMapDirectional::debugDisplay( castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 256u, 256u };
		//Position position{ int32_t( displaySize.getWidth() * index * 3 ), int32_t( displaySize.getHeight() * 3u ) };
		//getEngine()->getRenderSystem()->getCurrentContext()->renderVariance( position
		//	, displaySize
		//	, *m_shadowMap.getTexture() );
		//position = Position{ int32_t( displaySize.getWidth() * ( 2 + index * 3 ) ), int32_t( displaySize.getHeight() * 3u ) };
		//getEngine()->getRenderSystem()->getCurrentContext()->renderDepth( position
		//	, displaySize
		//	, *m_linearMap.getTexture() );
	}

	void ShadowMapDirectional::doInitialise()
	{
		renderer::Extent2D size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();

		renderer::ImageCreateInfo depth{};
		depth.arrayLayers = 1u;
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.extent.depth = 1u;
		depth.imageType = renderer::TextureType::e2D;
		depth.mipLevels = 1u;
		depth.samples = renderer::SampleCountFlag::e1;
		depth.usage = renderer::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = renderer::Format::eD24_UNORM_S8_UINT;
		m_depthTexture = device.createTexture( depth, renderer::MemoryPropertyFlag::eDeviceLocal );

		renderer::ImageViewCreateInfo depthView;
		depthView.format = depth.format;
		depthView.viewType = renderer::TextureViewType::e2D;
		depthView.subresourceRange.aspectMask = renderer::ImageAspectFlag::eDepth;
		depthView.subresourceRange.baseArrayLayer = 0u;
		depthView.subresourceRange.layerCount = 1u;
		depthView.subresourceRange.baseMipLevel = 0u;
		depthView.subresourceRange.levelCount = 1u;
		m_depthView = m_depthTexture->createView( depthView );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].index = 0u;
		renderPass.attachments[0].format = m_depthView->getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].index = 1u;
		renderPass.attachments[1].format = m_linearMap.getTexture()->getPixelFormat();
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[2].index = 2u;
		renderPass.attachments[2].format = m_shadowMap.getTexture()->getPixelFormat();
		renderPass.attachments[2].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[2].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[2].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[2].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[2].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[2].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[2].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 1u, renderer::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].colorAttachments.push_back( { 2u, renderer::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 0u ), *m_depthView );
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 1u ), m_linearMap.getTexture()->getDefaultView() );
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 2u ), m_shadowMap.getTexture()->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, m_shadowMap.getTexture()->getDefaultView()
			, size
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_blur.reset();
	}

	void ShadowMapDirectional::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapDirectional );
	}

	glsl::Shader ShadowMapDirectional::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassDirectional::ShadowMapUbo, ShadowMapPassDirectional::UboBindingPoint };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassDirectional::FarPlane ) );
		shadowMap.end();

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), RenderPass::VertexOutputs::TextureLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" ), RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ), RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_linear( writer.declFragData< Float >( cuT( "pxl_linear" ), 0u ) );
		auto pxl_variance( writer.declFragData< Vec2 >( cuT( "pxl_variance" ), 1u ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, gl_FragCoord.z() );
			pxl_variance.x() = depth;
			pxl_variance.y() = pxl_variance.x() * pxl_variance.x();
			pxl_linear = vtx_viewPosition.z() / c3d_farPlane;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_variance.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return writer.finalise();
	}
}
