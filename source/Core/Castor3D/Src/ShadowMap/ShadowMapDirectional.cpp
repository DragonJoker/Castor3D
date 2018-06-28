#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "Miscellaneous/GaussianBlur.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Culling/FrustumCuller.hpp"
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
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size
			, uint32_t cascadeCount )
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
			image.arrayLayers = cascadeCount;
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

		TextureUnit doInitialiseLinearDepth( Engine & engine
			, Size const & size
			, uint32_t cascadeCount )
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
			image.arrayLayers = cascadeCount;
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

		std::vector< ShadowMap::PassData > createPasses( Engine & engine
			, Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			std::vector< ShadowMap::PassData > result;
			auto const width = ShadowMapPassDirectional::TextureSize;
			auto const height = ShadowMapPassDirectional::TextureSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0_r, 5120.0_r );
			viewport.resize( { width, height } );
			viewport.update();

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapDirectional_" ) + string::toString( cascade + 1, std::locale{ "C" } )
						, scene
						, scene.getCameraRootNode()
						, std::move( viewport )
						, true ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				passData.pass = std::make_shared< ShadowMapPassDirectional >( engine
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap
					, cascade );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine
		, Scene & scene
		, uint32_t cascades )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize }, cascades )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize }, cascades )
			, createPasses( engine, scene, *this, cascades ) }
		, m_frameBuffers( m_passes.size() )
		, m_cascades{ cascades }
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

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			m_passes[cascade].pass->update( camera, queues, light, cascade );
		}
	}

	renderer::Semaphore const & ShadowMapDirectional::render( renderer::Semaphore const & toWait )
	{
		static renderer::ClearColorValue const black{ 0.0f, 0.0f, 0.0f, 1.0f };
		static renderer::DepthStencilClearValue const zero{ 1.0f, 0 };
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			m_passes[cascade].pass->updateDeviceDependent( cascade );
		}

		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];

			timer.notifyPassRender();
			timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( pass.pass->getRenderPass()
				, *frameBuffer.frameBuffer
				, { zero, black, black }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { pass.pass->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			timer.endPass( *m_commandBuffer );
		}

		m_commandBuffer->end();
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
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				result = &m_frameBuffers[cascade].blur->blur( *result );
			}
		}
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

	void ShadowMapDirectional::doInitialiseDepth()
	{
		renderer::Extent2D const size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & device = getCurrentDevice( *this );

		renderer::ImageCreateInfo depth{};
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.arrayLayers = m_cascades;
		depth.imageType = renderer::TextureType::e2D;
		depth.usage = renderer::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = ShadowMapDirectional::RawDepthFormat;
		m_depthTexture = device.createTexture( depth, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	void ShadowMapDirectional::doInitialiseFramebuffers()
	{
		renderer::Extent2D const size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize };
		auto & variance = m_shadowMap.getTexture()->getTexture();
		auto & linear = m_linearMap.getTexture()->getTexture();
		auto & depth = *m_depthTexture;

		renderer::ImageViewCreateInfo varianceView;
		varianceView.format = variance.getFormat();
		varianceView.viewType = renderer::TextureViewType::e2D;
		varianceView.subresourceRange.aspectMask = renderer::ImageAspectFlag::eColour;

		renderer::ImageViewCreateInfo linearView;
		linearView.format = linear.getFormat();
		linearView.viewType = renderer::TextureViewType::e2D;
		linearView.subresourceRange.aspectMask = renderer::ImageAspectFlag::eColour;

		renderer::ImageViewCreateInfo depthView;
		depthView.format = depth.getFormat();
		depthView.viewType = renderer::TextureViewType::e2D;
		depthView.subresourceRange.aspectMask = renderer::ImageAspectFlag::eDepth;

		for ( uint32_t cascade = 0u; cascade < m_passes.size(); ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];
			depthView.subresourceRange.baseArrayLayer = cascade;
			varianceView.subresourceRange.baseArrayLayer = cascade;
			linearView.subresourceRange.baseArrayLayer = cascade;
			frameBuffer.depthView = depth.createView( depthView );
			frameBuffer.varianceView = variance.createView( varianceView );
			frameBuffer.linearView = linear.createView( linearView );
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *frameBuffer.depthView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), *frameBuffer.linearView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), *frameBuffer.varianceView );
			frameBuffer.frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );

			frameBuffer.blur = std::make_unique< GaussianBlur >( *getEngine()
				, *frameBuffer.varianceView
				, size
				, variance.getFormat()
				, 5u );
		}
	}

	void ShadowMapDirectional::doInitialise()
	{
		doInitialiseDepth();
		doInitialiseFramebuffers();
		m_commandBuffer = getCurrentDevice( *this ).getGraphicsCommandPool().createCommandBuffer();
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_frameBuffers.clear();
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
