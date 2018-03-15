#include "RenderTarget.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "HDR/ToneMapping.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Core/Device.hpp>
#include <Image/Texture.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>

#include <Graphics/Image.hpp>

#if !defined( NDEBUG )
#	define DISPLAY_DEBUG 1
#else
#	define DISPLAY_DEBUG 0
#endif

using namespace castor;

namespace castor3d
{
	RenderTarget::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< RenderTarget >{ tabs }
	{
	}

	bool RenderTarget::TextWriter::operator()( RenderTarget const & target, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing RenderTarget" ) );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget name" );

		if ( result && target.getScene() )
		{
			result = file.writeText( m_tabs + cuT( "\tscene \"" ) + target.getScene()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget scene" );
		}

		if ( result && target.getCamera() )
		{
			result = file.writeText( m_tabs + cuT( "\tcamera \"" ) + target.getCamera()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget camera" );
		}

		if ( result )
		{
			result = file.print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), target.getSize().getWidth(), target.getSize().getHeight() ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tformat " ) + renderer::getName( target.getPixelFormat() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget tone mapping" );
		}

		if ( result
			&& target.m_renderTechnique
			&& target.m_renderTechnique->isMultisampling() )
		{
			result = file.writeText( m_tabs + cuT( "\t" ) )
				&& target.m_renderTechnique->writeInto( file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget technique" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_postEffects )
			{
				result = file.writeText( m_tabs + cuT( "\tpostfx \"" ) + effect->getName() + cuT( "\"" ) )
						   && effect->writeInto( file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			result = SsaoConfig::TextWriter{ m_tabs + cuT( "\t" ) }( target.m_ssaoConfig, file );
		}

		file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	RenderTarget::TargetFbo::TargetFbo( RenderTarget & renderTarget )
		: m_renderTarget{ renderTarget }
		, m_colourTexture{ *renderTarget.getEngine() }
	{
	}

	bool RenderTarget::TargetFbo::initialise( renderer::RenderPass & renderPass
		, uint32_t index
		, Size const & size )
	{
		auto & renderSystem = *m_renderTarget.getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		SamplerSPtr sampler = m_renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( m_renderTarget.m_index ) );
		renderer::ImageCreateInfo createInfo{};
		createInfo.flags = 0u;
		createInfo.arrayLayers = 1u;
		createInfo.extent.width = size.getWidth();
		createInfo.extent.height = size.getHeight();
		createInfo.extent.depth = 1u;
		createInfo.format = m_renderTarget.getPixelFormat();
		createInfo.imageType = renderer::TextureType::e2D;
		createInfo.initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.mipLevels = 1u;
		createInfo.samples = renderer::SampleCountFlag::e1;
		createInfo.sharingMode = renderer::SharingMode::eExclusive;
		createInfo.tiling = renderer::ImageTiling::eOptimal;
		createInfo.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
		auto texture = std::make_shared< TextureLayout >( renderSystem
			, createInfo
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_colourTexture.setTexture( texture );
		m_colourTexture.setSampler( sampler );
		m_colourTexture.setIndex( index );
		m_colourTexture.getTexture()->getImage().initialiseSource();
		m_colourTexture.getTexture()->initialise();

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *renderPass.getAttachments().begin(), m_colourTexture.getTexture()->getDefaultView() );
		m_frameBuffer = renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
			, {  } );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup()
	{
		m_colourTexture.cleanup();
		m_frameBuffer.reset();
	}

	//*************************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ PixelFormat::eA8R8G8B8 }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_frameBuffer{ *this }
		, m_velocityTexture{ engine }
	{
		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" ), *getEngine(), Parameters{} );
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) );
		sampler->setMinFilter( renderer::Filter::eLinear );
		sampler->setMagFilter( renderer::Filter::eLinear );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) );
		sampler->setMinFilter( renderer::Filter::eNearest );
		sampler->setMagFilter( renderer::Filter::eNearest );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::initialise( uint32_t index )
	{
		if ( !m_initialised )
		{
			auto & renderSystem = *getEngine()->getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();

			renderer::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].index = 0u;
			createInfo.attachments[0].format = getPixelFormat();
			createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 2u );
			createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			m_renderPass = device.createRenderPass( createInfo );
			m_frameBuffer.initialise( *m_renderPass
				, index
				, m_size );

			if ( !m_renderTechnique )
			{
				try
				{
					auto name = cuT( "RenderTargetTechnique_" ) + string::toString( m_index );
					m_renderTechnique = getEngine()->getRenderTechniqueCache().add( name
						, std::make_shared< RenderTechnique >( name
							, *this
							, *getEngine()->getRenderSystem()
							, m_techniqueParameters
							, m_ssaoConfig ) );
				}
				catch ( Exception & p_exc )
				{
					Logger::logError( cuT( "Couldn't load render technique: " ) + string::stringCast< xchar >( p_exc.getFullDescription() ) );
					throw;
				}
			}

			m_size.set( m_frameBuffer.m_colourTexture.getTexture()->getDimensions().width
				, m_frameBuffer.m_colourTexture.getTexture()->getDimensions().height );
			m_renderTechnique->initialise( index );

			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = m_size.getWidth();
			image.extent.height = m_size.getHeight();
			image.extent.depth = 1u;
			image.format = renderer::Format::eR16G16B16A16_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			auto velocityTexture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			m_velocityTexture.setTexture( velocityTexture );
			m_velocityTexture.setSampler( getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) ) );
			m_velocityTexture.getTexture()->getImage().initialiseSource();
			m_velocityTexture.getTexture()->initialise();

			m_postPostFxTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "sRGB Post effects" ), cuT( "sRGB Post effects" ) );
			m_overlaysTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Overlays" ), cuT( "Overlays" ) );

			for ( auto effect : m_postEffects )
			{
				effect->initialise();
			}

			m_initialised = m_toneMapping->initialise( getSize()
				, m_renderTechnique->getResult()
				, *m_renderPass );

			for ( auto effect : m_postPostEffects )
			{
				effect->initialise();
			}

			m_overlayRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem()
				, m_frameBuffer.m_colourTexture.getTexture()->getDefaultView() );
			m_overlayRenderer->initialise();
		}
	}

	void RenderTarget::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_overlayRenderer->cleanup();
			m_overlayRenderer.reset();

			for ( auto effect : m_postPostEffects )
			{
				effect->cleanup();
			}

			m_toneMapping->cleanup();
			m_toneMapping.reset();

			for ( auto effect : m_postEffects )
			{
				effect->cleanup();
			}

			m_postEffects.clear();
			m_postPostEffects.clear();

			m_postPostFxTimer.reset();
			m_overlaysTimer.reset();

			m_velocityTexture.cleanup();
			m_velocityTexture.setTexture( nullptr );

			m_renderTechnique->cleanup();
			m_frameBuffer.cleanup();
			m_renderTechnique.reset();
			getEngine()->getRenderTechniqueCache().remove( cuT( "RenderTargetTechnique_" ) + string::toString( m_index ) );
		}
	}

	void RenderTarget::render( RenderInfo & info )
	{
		SceneSPtr scene = getScene();

		if ( scene )
		{
			if ( m_initialised && scene->isInitialised() )
			{
				CameraSPtr camera = getCamera();

				if ( camera )
				{
					scene->getGeometryCache().fillInfo( info );
					doRender( info, m_frameBuffer, getCamera() );
				}
			}
		}
	}

	ViewportType RenderTarget::getViewportType()const
	{
		return ( getCamera() ? getCamera()->getViewportType() : ViewportType::eCount );
	}

	void RenderTarget::setViewportType( ViewportType value )
	{
		if ( getCamera() )
		{
			getCamera()->setViewportType( value );
		}
	}

	void RenderTarget::setCamera( CameraSPtr camera )
	{
		m_camera = camera;
	}

	void RenderTarget::setToneMappingType( String const & name
		, Parameters const & parameters )
	{
		if ( m_toneMapping )
		{
			ToneMappingSPtr toneMapping;
			std::swap( m_toneMapping, toneMapping );
			// Give ownership of the tone mapping to the event (capture by value in the lambda).
			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, [toneMapping]()
			{
				toneMapping->cleanup();
			} ) );
		}

		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name, *getEngine(), parameters );
	}

	void RenderTarget::addPostEffect( PostEffectSPtr effect )
	{
		if ( effect->isAfterToneMapping() )
		{
			m_postPostEffects.push_back( effect );
		}
		else
		{
			m_postEffects.push_back( effect );
		}
	}

	void RenderTarget::setSize( Size const & size )
	{
		m_size = size;
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::doRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto & queue = getEngine()->getRenderSystem()->getCurrentDevice()->getGraphicsQueue();
		auto const * semaphoreToWait = m_signalReady.get();
		SceneSPtr scene = getScene();

		// Render the scene through the RenderTechnique.
		m_renderTechnique->render( m_jitter
			, *semaphoreToWait
			, info );
		semaphoreToWait = &m_renderTechnique->getSemaphore();

		// Then draw the render's result to the RenderTarget's frame buffer.
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_toneMapping->update( scene->getHdrConfig() );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *fbo.m_frameBuffer
				, { convert( RgbaColour::fromRGBA( toRGBAFloat( scene->getBackgroundColour() ) ) ) }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_toneMapping->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}

		queue.submit( *m_commandBuffer
			, *semaphoreToWait
			, renderer::PipelineStageFlag::eAllCommands
			, m_toneMapping->getSemaphore()
			, nullptr );
		semaphoreToWait = &m_toneMapping->getSemaphore();

		// Apply the post effects.
		m_postPostFxTimer->start();

		for ( auto & effect : m_postPostEffects )
		{
			queue.submit( effect->getCommands()
				, *semaphoreToWait
				, renderer::PipelineStageFlag::eAllCommands
				, effect->getSemaphore()
				, nullptr );
			semaphoreToWait = &effect->getSemaphore();
		}

		m_postPostFxTimer->stop();

		// And now render overlays.
		m_overlaysTimer->start();
		{
			auto lock = makeUniqueLock( getEngine()->getOverlayCache() );
			m_overlayRenderer->beginRender( camera->getViewport() );

			for ( auto category : getEngine()->getOverlayCache() )
			{
				SceneSPtr scene = category->getOverlay().getScene();

				if ( category->getOverlay().isVisible() && ( !scene || scene->getName() == scene->getName() ) )
				{
					category->render( *m_overlayRenderer );
				}
			}

			m_overlayRenderer->endRender();
			queue.submit( m_overlayRenderer->getCommands()
				, *semaphoreToWait
				, renderer::PipelineStageFlag::eAllCommands
				, *m_signalFinished
				, nullptr );
		}
		m_overlaysTimer->stop();

#if DISPLAY_DEBUG

		m_renderTechnique->debugDisplay( Size{ camera->getWidth(), camera->getHeight() } );

#endif
	}
}
