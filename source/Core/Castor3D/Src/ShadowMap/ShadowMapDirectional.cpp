#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "Miscellaneous/GaussianBlur.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
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
#include <Sync/Fence.hpp>

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
				sampler->setBorderColour( renderer::BorderColour::eFloatOpaqueBlack );
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
			image.format = ShadowMapDirectional::VarianceFormat;

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

		TextureUnit doInitialiseLinearDepth( Engine & engine, Size const & size )
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
			image.format = ShadowMapDirectional::LinearDepthFormat;

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
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize } )
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
		m_shadowType = light.getShadowType();
		m_passes[0]->update( camera, queues, light, index );
	}

	renderer::Semaphore const & ShadowMapDirectional::render( renderer::Semaphore const & toWait )
	{
		static renderer::ClearColorValue const black{ 0.0f, 0.0f, 0.0f, 1.0f };
		static renderer::DepthStencilClearValue const zero{ 1.0f, 0 };
		m_passes[0]->updateDeviceDependent();
		auto & timer = m_passes[0]->getTimer();
		timer.start();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			timer.notifyPassRender();
			timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( m_passes[0]->getRenderPass()
				, *m_frameBuffer
				, { zero, black, black }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_passes[0]->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			timer.endPass( *m_commandBuffer );
			m_commandBuffer->end();
		}

		auto & device = getCurrentDevice( *this );
		auto * result = &toWait;
		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		if ( m_shadowType == ShadowType::eVariance )
		{
			result = &m_blur->blur( *result );
		}

		timer.stop();
		return *result;
	}

	void ShadowMapDirectional::debugDisplay( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, castor::Size const & size
		, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( displaySize.getWidth() * ( 0 + index * 2 ) ), int32_t( displaySize.getHeight() * 3u ) };
		getEngine()->renderDepth( renderPass
			, frameBuffer
			, position
			, displaySize
			, *m_shadowMap.getTexture() );
		position.offset( int32_t( displaySize.getWidth() ), 0 );
		getEngine()->renderDepth( renderPass
			, frameBuffer
			, position
			, displaySize
			, *m_linearMap.getTexture() );
	}

	void ShadowMapDirectional::doInitialise()
	{
		renderer::Extent2D size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & device = getCurrentDevice( *this );

		renderer::ImageCreateInfo depth{};
		depth.arrayLayers = 1u;
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.extent.depth = 1u;
		depth.imageType = renderer::TextureType::e2D;
		depth.mipLevels = 1u;
		depth.samples = renderer::SampleCountFlag::e1;
		depth.usage = renderer::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = ShadowMapDirectional::RawDepthFormat;
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

		auto & renderPass = m_passes[0]->getRenderPass();
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *m_depthView );
		attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), m_linearMap.getTexture()->getDefaultView() );
		attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), m_shadowMap.getTexture()->getDefaultView() );
		m_frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, m_shadowMap.getTexture()->getDefaultView()
			, size
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_blur.reset();
		m_frameBuffer.reset();
		m_depthView.reset();
		m_depthTexture.reset();
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

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
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
