#include "RenderTarget.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "HDR/ToneMapping.hpp"
#include "Overlay/OverlayRenderer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/Culling/FrustumCuller.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <Core/Device.hpp>
#include <Image/Texture.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

#include <GlslSource.hpp>

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
			result = file.writeText( m_tabs + cuT( "\tsize " )
				+ string::toString( target.getSize().getWidth(), std::locale{ "C" } ) + cuT( " " )
				+ string::toString( target.getSize().getHeight(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tformat " ) + PF::getFormatName( convert( target.getPixelFormat() ) ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget tone mapping" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_hdrPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			for ( auto const & effect : target.m_srgbPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			result = HdrConfig::TextWriter( m_tabs + cuT( "\t" ) )( target.getHdrConfig(), file );
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
		: renderTarget{ renderTarget }
		, colourTexture{ *renderTarget.getEngine() }
	{
	}

	bool RenderTarget::TargetFbo::initialise( renderer::RenderPass & renderPass
		, Size const & size )
	{
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();

		SamplerSPtr sampler = renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( renderTarget.m_index ) );
		renderer::ImageCreateInfo createInfo{};
		createInfo.flags = 0u;
		createInfo.arrayLayers = 1u;
		createInfo.extent.width = size.getWidth();
		createInfo.extent.height = size.getHeight();
		createInfo.extent.depth = 1u;
		createInfo.format = renderTarget.getPixelFormat();
		createInfo.imageType = renderer::TextureType::e2D;
		createInfo.initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.mipLevels = 1u;
		createInfo.samples = renderer::SampleCountFlag::e1;
		createInfo.sharingMode = renderer::SharingMode::eExclusive;
		createInfo.tiling = renderer::ImageTiling::eOptimal;
		createInfo.usage = renderer::ImageUsageFlag::eColourAttachment
			| renderer::ImageUsageFlag::eSampled
			| renderer::ImageUsageFlag::eTransferDst
			| renderer::ImageUsageFlag::eTransferSrc;
		auto texture = std::make_shared< TextureLayout >( renderSystem
			, createInfo
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		colourTexture.setTexture( texture );
		colourTexture.setSampler( sampler );
		colourTexture.getTexture()->getDefaultImage().initialiseSource();
		colourTexture.getTexture()->initialise();

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *renderPass.getAttachments().begin(), colourTexture.getTexture()->getDefaultView() );
		frameBuffer = renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup()
	{
		colourTexture.cleanup();
		frameBuffer.reset();
	}

	//*************************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ renderer::Format::eR8G8B8A8_UNORM }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_workFrameBuffer{ *this }
		, m_flippedFrameBuffer{ *this }
		, m_velocityTexture{ engine }
	{
		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" )
			, *getEngine()
			, m_hdrConfig
			, Parameters{} );
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

	void RenderTarget::initialise()
	{
		if ( !m_initialised )
		{
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			auto & renderSystem = *getEngine()->getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			doInitialiseRenderPass();
			m_initialised = doInitialiseFrameBuffer();

			if ( m_initialised )
			{
				m_initialised = doInitialiseVelocityTexture();
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseTechnique();
			}

			m_overlaysTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Overlays" ), cuT( "Overlays" ) );
			m_toneMappingTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Tone Mapping" ), cuT( "Tone Mapping" ) );

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResult();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( m_hdrCopyCommands
					, sourceView->getImage().getView()
					, m_renderTechnique->getResult().getDefaultView() );
				m_hdrCopyFinished = device.createSemaphore();
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseToneMapping();
			}

			if ( !m_srgbPostEffects.empty() )
			{
				auto const * sourceView = m_workFrameBuffer.colourTexture.getTexture().get();

				for ( auto effect : m_srgbPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( m_srgbCopyCommands
					, sourceView->getDefaultView()
					, m_workFrameBuffer.colourTexture.getTexture()->getDefaultView() );
				m_srgbCopyFinished = device.createSemaphore();
			}

			doInitialiseFlip();
			m_overlayRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem()
				, m_flippedFrameBuffer.colourTexture.getTexture()->getDefaultView() );
			m_overlayRenderer->initialise();

			m_signalReady = device.createSemaphore();
			m_fence = device.createFence( renderer::FenceCreateFlag::eSignaled );
		}
	}

	void RenderTarget::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_culler.reset();

			m_fence.reset();
			m_signalReady.reset();

			m_flipQuad.reset();
			m_flipCommands.reset();
			m_flipFinished.reset();

			m_toneMappingCommandBuffer.reset();
			m_overlayRenderer->cleanup();
			m_overlayRenderer.reset();

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup();
			}

			m_toneMapping->cleanup();
			m_toneMapping.reset();

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup();
			}

			m_hdrCopyCommands.reset();
			m_srgbCopyCommands.reset();
			m_hdrCopyFinished.reset();
			m_srgbCopyFinished.reset();
			m_hdrPostEffects.clear();
			m_srgbPostEffects.clear();

			m_overlaysTimer.reset();
			m_toneMappingTimer.reset();

			m_velocityTexture.cleanup();
			m_velocityTexture.setTexture( nullptr );

			m_renderTechnique->cleanup();
			m_workFrameBuffer.cleanup();
			m_flippedFrameBuffer.cleanup();
			m_renderTechnique.reset();
			m_renderPass.reset();
			getEngine()->getRenderTechniqueCache().remove( cuT( "RenderTargetTechnique_" ) + string::toString( m_index ) );
		}
	}

	void RenderTarget::update()
	{
		auto & camera = *getCamera();
		camera.resize( m_size );
		camera.update();

		REQUIRE( m_culler );
		m_culler->compute();
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
					getEngine()->getRenderSystem()->pushScene( scene.get() );
					scene->getGeometryCache().fillInfo( info );
					doRender( info, m_workFrameBuffer, getCamera() );
					getEngine()->getRenderSystem()->popScene();
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
		auto myCamera = getCamera();

		if ( myCamera != camera )
		{
			m_camera = camera;
			camera->resize( m_size );
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
		}
	}

	void RenderTarget::setScene( SceneSPtr scene )
	{
		auto myScene = getScene();

		if ( myScene != scene )
		{
			m_scene = scene;
			m_culler.reset();
		}
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

		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name
			, *getEngine()
			, m_hdrConfig
			, parameters );
	}

	void RenderTarget::addPostEffect( PostEffectSPtr effect )
	{
		if ( effect->isAfterToneMapping() )
		{
			m_srgbPostEffects.push_back( effect );
		}
		else
		{
			m_hdrPostEffects.push_back( effect );
		}
	}

	void RenderTarget::setSize( Size const & size )
	{
		m_size = size;
		auto camera = getCamera();

		if ( camera )
		{
			camera->resize( m_size );
		}
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::doInitialiseRenderPass()
	{
		renderer::RenderPassCreateInfo renderPass{};
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = getPixelFormat();
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 0u, renderer::ImageLayout::eColourAttachmentOptimal } };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = getCurrentDevice( *this ).createRenderPass( renderPass );
	}

	bool RenderTarget::doInitialiseFrameBuffer()
	{
		auto result = m_workFrameBuffer.initialise( *m_renderPass
			, m_size );
		m_size.set( m_workFrameBuffer.colourTexture.getTexture()->getDimensions().width
			, m_workFrameBuffer.colourTexture.getTexture()->getDimensions().height );

		if ( result )
		{
			result = m_flippedFrameBuffer.initialise( *m_renderPass
				, m_size );
		}

		return result;
	}

	bool RenderTarget::doInitialiseVelocityTexture()
	{
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
		m_velocityTexture.getTexture()->getDefaultImage().initialiseSource();
		return m_velocityTexture.initialise();
	}

	bool RenderTarget::doInitialiseTechnique()
	{
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

		return m_renderTechnique->initialise();
	}

	bool RenderTarget::doInitialiseToneMapping()
	{
		auto & device = getCurrentDevice( *this );
		auto result = m_toneMapping->initialise( getSize()
			, m_renderTechnique->getResult()
			, *m_renderPass );

		m_toneMappingCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		renderer::ClearColorValue const clear{ 0.0f, 0.0f, 1.0f, 1.0f };

		if ( result )
		{
			result = m_toneMappingCommandBuffer->begin();
		}

		if ( result )
		{
			m_toneMappingTimer->beginPass( *m_toneMappingCommandBuffer );
			// Put render technique image in shader input layout.
			m_toneMappingCommandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_renderTechnique->getResult().getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			m_toneMappingCommandBuffer->beginRenderPass( *m_renderPass
				, *m_workFrameBuffer.frameBuffer
				, { clear }
				, renderer::SubpassContents::eInline );
			m_toneMapping->registerFrame( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endRenderPass();
			// Put render technique image back in colour attachment layout.
			m_toneMappingCommandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, m_renderTechnique->getResult().getDefaultView().makeColourAttachment( renderer::ImageLayout::eUndefined, 0u ) );
			m_toneMappingTimer->endPass( *m_toneMappingCommandBuffer );
			result = m_toneMappingCommandBuffer->end();
		}

		return result;
	}

	void RenderTarget::doInitialiseCopyCommands( renderer::CommandBufferPtr & commandBuffer
		, renderer::TextureView const & source
		, renderer::TextureView const & target )
	{
		auto & device = getCurrentDevice( *this );
		commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		commandBuffer->begin();

		if ( &source != &target )
		{
			// Put source image in transfer source layout.
			commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eTransfer
				, source.makeTransferSource( renderer::ImageLayout::eUndefined, 0u ) );
			// Put target image in transfer destination layout.
			commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
				, renderer::PipelineStageFlag::eTransfer
				, target.makeTransferDestination( renderer::ImageLayout::eUndefined, 0u ) );
			// Copy source to target.
			commandBuffer->copyImage( source, target );
		}

		// Put target image in fragment shader input layout.
		commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::PipelineStageFlag::eFragmentShader
			, target.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
		commandBuffer->end();
	}

	void RenderTarget::doInitialiseFlip()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = vec2( texcoord.x(), 1.0_f - texcoord.y() );
				out.gl_Position() = vec4( position, 0.0, 1.0 );
			} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vtx_texture ).xyz(), 1.0 );
			} );
			pxl = writer.finalise();
		}

		renderer::ShaderStageStateArray program;
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( vtx.getSource() );
		program[1].module->loadShader( pxl.getSource() );

		m_flipCommands = device.getGraphicsCommandPool().createCommandBuffer();
		m_flipFinished = device.createSemaphore();
		m_flipQuad = std::make_unique< RenderQuad >( renderSystem, true );
		m_flipQuad->createPipeline( { m_size.getWidth(), m_size.getHeight() }
			, Position{}
			, program
			, m_workFrameBuffer.colourTexture.getTexture()->getDefaultView()
			, *m_renderPass
			, {}
			, {} );

		m_flipCommands->begin();
		m_flipCommands->beginRenderPass( *m_renderPass
			, *m_flippedFrameBuffer.frameBuffer
			, { renderer::ClearColorValue{} }
			, renderer::SubpassContents::eInline );
		m_flipQuad->registerFrame( *m_flipCommands );
		m_flipCommands->endRenderPass();
		m_flipCommands->end();
	}

	void RenderTarget::doRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto elapsedTime = m_timer.getElapsed();
		SceneSPtr scene = getScene();
		m_toneMapping->update();
		renderer::SemaphoreCRefArray signalsToWait;
		
		if ( m_type == TargetType::eWindow )
		{
			signalsToWait = scene->getRenderTargetsSemaphores();
		}

		// Render the scene through the RenderTechnique.
		m_signalFinished = &m_renderTechnique->render( m_jitter
			, signalsToWait
			, info );

		// Draw HDR post effects.
		m_signalFinished = &doApplyPostEffects( *m_signalFinished
			, m_hdrPostEffects
			, m_hdrCopyCommands
			, m_hdrCopyFinished
			, elapsedTime );

		// Then draw the render's result to the RenderTarget's frame buffer, applying the tone mapping operator.
		m_signalFinished = &doApplyToneMapping( *m_signalFinished );

		// Apply the sRGB post effects.
		m_signalFinished = &doApplyPostEffects( *m_signalFinished
			, m_srgbPostEffects
			, m_srgbCopyCommands
			, m_srgbCopyFinished
			, elapsedTime );

		// Put the result in bottom up (since all work has been done in top down).
		m_signalFinished = &doFlip( *m_signalFinished );

		// And now render overlays.
		m_signalFinished = &doRenderOverlays( *m_signalFinished, *camera );

#if DISPLAY_DEBUG

		m_renderTechnique->debugDisplay( Size{ camera->getWidth(), camera->getHeight() } );

#endif
	}

	renderer::Semaphore const & RenderTarget::doApplyPostEffects( renderer::Semaphore const & toWait
		, PostEffectPtrArray const & effects
		, renderer::CommandBufferPtr const & copyCommandBuffer
		, renderer::SemaphorePtr const & copyFinished
		, castor::Nanoseconds const & elapsedTime )
	{
		auto * result = &toWait;

		if ( !effects.empty() )
		{
			auto & queue = getCurrentDevice( *this ).getGraphicsQueue();

			for ( auto effect : effects )
			{
				effect->start();
				effect->update( elapsedTime );

				for ( auto & commands : effect->getCommands() )
				{
					effect->notifyPassRender();
					queue.submit( *commands.commandBuffer
						, *result
						, renderer::PipelineStageFlag::eColourAttachmentOutput
						, *commands.semaphore
						, nullptr );
					result = commands.semaphore.get();
				}

				effect->stop();
			}

			queue.submit( *copyCommandBuffer
				, *result
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, *copyFinished
				, nullptr );
			result = copyFinished.get();
		}

		return *result;
	}

	renderer::Semaphore const & RenderTarget::doApplyToneMapping( renderer::Semaphore const & toWait )
	{
		auto & queue = getCurrentDevice( *this ).getGraphicsQueue();
		m_toneMappingTimer->start();
		m_toneMappingTimer->notifyPassRender();
		auto * result = &toWait;
		queue.submit( *m_toneMappingCommandBuffer
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, m_toneMapping->getSemaphore()
			, nullptr );
		m_toneMappingTimer->stop();
		result = &m_toneMapping->getSemaphore();
		return *result;
	}

	renderer::Semaphore const & RenderTarget::doRenderOverlays( renderer::Semaphore const & toWait
		, Camera const & camera )
	{
		auto * result = &toWait;
		m_overlaysTimer->start();
		{
			auto lock = makeUniqueLock( getEngine()->getOverlayCache() );
			m_overlayRenderer->beginPrepare( camera
				, *m_overlaysTimer
				, *result );
			auto preparer = m_overlayRenderer->getPreparer();

			for ( auto category : getEngine()->getOverlayCache() )
			{
				SceneSPtr scene = category->getOverlay().getScene();

				if ( category->getOverlay().isVisible()
					&& ( ( !scene && m_type == TargetType::eWindow )
						|| scene.get() == getScene().get() ) )
				{
					category->update( *m_overlayRenderer );
					category->accept( preparer );
				}
			}

			m_overlayRenderer->endPrepare( *m_overlaysTimer );
			m_overlayRenderer->render( *m_overlaysTimer );
			result = &m_overlayRenderer->getSemaphore();
		}
		m_overlaysTimer->stop();
		return *result;
	}

	renderer::Semaphore const & RenderTarget::doFlip( renderer::Semaphore const & toWait )
	{
		auto & queue = getCurrentDevice( *this ).getGraphicsQueue();
		auto * result = &toWait;
		queue.submit( *m_flipCommands
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_flipFinished
			, nullptr );
		result = m_flipFinished.get();
		return *result;
	}
}
