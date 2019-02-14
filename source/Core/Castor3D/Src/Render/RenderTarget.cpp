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
#include "Shader/Shaders/GlslUtils.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/SubpassDependency.hpp>
#include <Ashes/RenderPass/SubpassDescription.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>

#if !defined( NDEBUG )
#	define DISPLAY_DEBUG 1
#else
#	define DISPLAY_DEBUG 0
#endif

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

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

	bool RenderTarget::TargetFbo::initialise( ashes::RenderPass & renderPass
		, ashes::Format format
		, Size const & size )
	{
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();

		SamplerSPtr sampler = renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( renderTarget.m_index ) );
		ashes::ImageCreateInfo createInfo{};
		createInfo.flags = 0u;
		createInfo.arrayLayers = 1u;
		createInfo.extent.width = size.getWidth();
		createInfo.extent.height = size.getHeight();
		createInfo.extent.depth = 1u;
		createInfo.format = format;
		createInfo.imageType = ashes::TextureType::e2D;
		createInfo.initialLayout = ashes::ImageLayout::eUndefined;
		createInfo.mipLevels = 1u;
		createInfo.samples = ashes::SampleCountFlag::e1;
		createInfo.sharingMode = ashes::SharingMode::eExclusive;
		createInfo.tiling = ashes::ImageTiling::eOptimal;
		createInfo.usage = ashes::ImageUsageFlag::eColourAttachment
			| ashes::ImageUsageFlag::eSampled
			| ashes::ImageUsageFlag::eTransferDst
			| ashes::ImageUsageFlag::eTransferSrc;
		auto texture = std::make_shared< TextureLayout >( renderSystem
			, createInfo
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		colourTexture.setTexture( texture );
		colourTexture.setSampler( sampler );
		colourTexture.getTexture()->getDefaultImage().initialiseSource();
		colourTexture.getTexture()->initialise();

		ashes::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *renderPass.getAttachments().begin(), colourTexture.getTexture()->getDefaultView() );
		frameBuffer = renderPass.createFrameBuffer( ashes::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup()
	{
		colourTexture.cleanup();
		frameBuffer.reset();
	}

	//*************************************************************************************************

	RenderTarget::CombineQuad::CombineQuad( RenderSystem & renderSystem
		, ashes::TextureView const & ovView )
		: RenderQuad{ renderSystem, true }
		, m_ovView{ ovView }
	{
	}

	void RenderTarget::CombineQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_ovView
			, m_sampler->getSampler() );
	}

	//*************************************************************************************************
	
	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ ashes::Format::eR8G8B8A8_UNORM }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_objectsFrameBuffer{ *this }
		, m_overlaysFrameBuffer{ *this }
		, m_combinedFrameBuffer{ *this }
		, m_velocityTexture{ engine }
	{
		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" )
			, *getEngine()
			, m_hdrConfig
			, Parameters{} );
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) );
		sampler->setMinFilter( ashes::Filter::eLinear );
		sampler->setMagFilter( ashes::Filter::eLinear );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) );
		sampler->setMinFilter( ashes::Filter::eNearest );
		sampler->setMagFilter( ashes::Filter::eNearest );
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
				auto const * sourceView = m_objectsFrameBuffer.colourTexture.getTexture().get();

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
					, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView() );
				m_srgbCopyFinished = device.createSemaphore();
			}

			doInitialiseFlip();
			m_overlayRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem()
				, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView() );
			m_overlayRenderer->initialise();

			m_signalReady = device.createSemaphore();
			m_fence = device.createFence( ashes::FenceCreateFlag::eSignaled );
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

			m_combineQuad.reset();
			m_combineCommands.reset();
			m_combineFinished.reset();

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
			m_overlaysFrameBuffer.cleanup();
			m_objectsFrameBuffer.cleanup();
			m_combinedFrameBuffer.cleanup();
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

		CU_Require( m_culler );
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
					doRender( info, m_objectsFrameBuffer, getCamera() );
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
		ashes::RenderPassCreateInfo renderPass{};
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = getPixelFormat();
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 0u, ashes::ImageLayout::eColourAttachmentOptimal } };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = getCurrentDevice( *this ).createRenderPass( renderPass );
	}

	bool RenderTarget::doInitialiseFrameBuffer()
	{
		auto result = m_objectsFrameBuffer.initialise( *m_renderPass
			, ashes::Format::eR8G8B8A8_UNORM
			, m_size );
		auto & dimensions = m_objectsFrameBuffer.colourTexture.getTexture()->getDimensions();
		m_size.set( dimensions.width, dimensions.height );

		if ( result )
		{
			result = m_overlaysFrameBuffer.initialise( *m_renderPass
				, ashes::Format::eR8G8B8A8_UNORM
				, m_size );
		}

		if ( result )
		{
			result = m_combinedFrameBuffer.initialise( *m_renderPass
				, getPixelFormat()
				, m_size );
		}

		return result;
	}

	bool RenderTarget::doInitialiseVelocityTexture()
	{
		ashes::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = m_size.getWidth();
		image.extent.height = m_size.getHeight();
		image.extent.depth = 1u;
		image.format = ashes::Format::eR16G16B16A16_SFLOAT;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
		auto velocityTexture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
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
		ashes::ClearColorValue const clear{ 0.0f, 0.0f, 1.0f, 1.0f };

		if ( result )
		{
			m_toneMappingCommandBuffer->begin();
			m_toneMappingTimer->beginPass( *m_toneMappingCommandBuffer );
			// Put render technique image in shader input layout.
			m_toneMappingCommandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, m_renderTechnique->getResult().getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
			m_toneMappingCommandBuffer->beginRenderPass( *m_renderPass
				, *m_objectsFrameBuffer.frameBuffer
				, { clear }
				, ashes::SubpassContents::eInline );
			m_toneMapping->registerFrame( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endRenderPass();
			// Put render technique image back in colour attachment layout.
			m_toneMappingCommandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eColourAttachmentOutput
				, m_renderTechnique->getResult().getDefaultView().makeColourAttachment( ashes::ImageLayout::eUndefined, 0u ) );
			m_toneMappingTimer->endPass( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->end();
		}

		return result;
	}

	void RenderTarget::doInitialiseCopyCommands( ashes::CommandBufferPtr & commandBuffer
		, ashes::TextureView const & source
		, ashes::TextureView const & target )
	{
		auto & device = getCurrentDevice( *this );
		commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		commandBuffer->begin();

		if ( &source != &target )
		{
			// Put source image in transfer source layout.
			commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eTransfer
				, source.makeTransferSource( ashes::ImageLayout::eUndefined, 0u ) );
			// Put target image in transfer destination layout.
			commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
				, ashes::PipelineStageFlag::eTransfer
				, target.makeTransferDestination( ashes::ImageLayout::eUndefined, 0u ) );
			// Copy source to target.
			commandBuffer->copyImage( source, target );
		}

		// Put target image in fragment shader input layout.
		commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, target.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		commandBuffer->end();
	}

	void RenderTarget::doInitialiseFlip()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "Flip" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_textureObjects = writer.declOutput< Vec2 >( cuT( "vtx_textureObjects" ), 0u );
			auto vtx_textureOverlays = writer.declOutput< Vec2 >( cuT( "vtx_textureOverlays" ), 1u );
			auto out = writer.getOut();

			shader::Utils utils{ writer, renderSystem.isTopDown() };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_textureObjects = utils.topDownToBottomUp( uv );
					vtx_textureOverlays = uv;

					if ( getTargetType() != TargetType::eWindow )
					{
						vtx_textureObjects.y() = 1.0_f - vtx_textureObjects.y();
						vtx_textureOverlays.y() = 1.0_f - vtx_textureOverlays.y();
					}

					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "Flip" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapOverlays = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOverlays" ), 0u, 0u );
			auto c3d_mapObjects = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapObjects" ), 1u, 0u );
			auto vtx_textureObjects = writer.declInput< Vec2 >( cuT( "vtx_textureObjects" ), 0u );
			auto vtx_textureOverlays = writer.declInput< Vec2 >( cuT( "vtx_textureOverlays" ), 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto overlayColor = writer.declLocale( cuT( "overlayColor" )
						, texture( c3d_mapOverlays, vtx_textureOverlays ) );
					auto objectsColor = writer.declLocale( cuT( "objectsColor" )
						, texture( c3d_mapObjects, vtx_textureObjects ) );
					objectsColor.rgb() *= 1.0_f - overlayColor.a();
					//overlayColor.rgb() *= overlayColor.a();
					pxl_fragColor = vec4( objectsColor.rgb() + overlayColor.rgb(), 1.0_f );
				} );
			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray program;
		program.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		program.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( renderSystem.compileShader( vtx ) );
		program[1].module->loadShader( renderSystem.compileShader( pxl ) );

		m_combineCommands = device.getGraphicsCommandPool().createCommandBuffer();
		m_combineFinished = device.createSemaphore();
		m_combineQuad = std::make_unique< CombineQuad >( renderSystem
			, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView() );
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }
		};
		m_combineQuad->createPipeline( { m_size.getWidth(), m_size.getHeight() }
			, Position{}
			, program
			, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );

		m_combineCommands->begin();
		m_combineCommands->beginRenderPass( *m_renderPass
			, *m_combinedFrameBuffer.frameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		m_combineQuad->registerFrame( *m_combineCommands );
		m_combineCommands->endRenderPass();
		m_combineCommands->end();
	}

	void RenderTarget::doRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto elapsedTime = m_timer.getElapsed();
		SceneSPtr scene = getScene();
		m_toneMapping->update();
		ashes::SemaphoreCRefArray signalsToWait;
		
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

		// And now render overlays.
		m_signalFinished = &doRenderOverlays( *m_signalFinished, *camera );

		// Combine objects and overlays framebuffers, flipping them if necessary.
		m_signalFinished = &doCombine( *m_signalFinished );

#if DISPLAY_DEBUG

		m_renderTechnique->debugDisplay( Size{ camera->getWidth(), camera->getHeight() } );

#endif
	}

	ashes::Semaphore const & RenderTarget::doApplyPostEffects( ashes::Semaphore const & toWait
		, PostEffectPtrArray const & effects
		, ashes::CommandBufferPtr const & copyCommandBuffer
		, ashes::SemaphorePtr const & copyFinished
		, castor::Nanoseconds const & elapsedTime )
	{
		auto * result = &toWait;

		if ( !effects.empty() )
		{
			auto & queue = getCurrentDevice( *this ).getGraphicsQueue();

			for ( auto effect : effects )
			{
				auto timerBlock = effect->start();
				effect->update( elapsedTime );

				for ( auto & commands : effect->getCommands() )
				{
					effect->notifyPassRender();

					queue.submit( *commands.commandBuffer
						, *result
						, ashes::PipelineStageFlag::eColourAttachmentOutput
						, *commands.semaphore
						, nullptr );
					result = commands.semaphore.get();
				}
			}

			queue.submit( *copyCommandBuffer
				, *result
				, ashes::PipelineStageFlag::eColourAttachmentOutput
				, *copyFinished
				, nullptr );
			result = copyFinished.get();
		}

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doApplyToneMapping( ashes::Semaphore const & toWait )
	{
		auto & queue = getCurrentDevice( *this ).getGraphicsQueue();
		auto timerBlock = m_toneMappingTimer->start();
		m_toneMappingTimer->notifyPassRender();
		auto * result = &toWait;

		queue.submit( *m_toneMappingCommandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, m_toneMapping->getSemaphore()
			, nullptr );
		result = &m_toneMapping->getSemaphore();

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doRenderOverlays( ashes::Semaphore const & toWait
		, Camera const & camera )
	{
		auto * result = &toWait;
		auto timerBlock = m_overlaysTimer->start();
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
		return *result;
	}

	ashes::Semaphore const & RenderTarget::doCombine( ashes::Semaphore const & toWait )
	{
		auto & queue = getCurrentDevice( *this ).getGraphicsQueue();
		auto * result = &toWait;

		queue.submit( *m_combineCommands
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_combineFinished
			, nullptr );
		result = m_combineFinished.get();

		return *result;
	}
}
