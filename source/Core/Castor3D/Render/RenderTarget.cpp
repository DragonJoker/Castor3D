#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TechniqueCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>

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
		log::info << m_tabs << cuT( "Writing RenderTarget" ) << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " name" );

		if ( result && target.getScene() )
		{
			result = file.writeText( m_tabs + cuT( "\tscene \"" ) + target.getScene()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " scene" );
		}

		if ( result && target.getCamera() )
		{
			result = file.writeText( m_tabs + cuT( "\tcamera \"" ) + target.getCamera()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " camera" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tsize " )
				+ string::toString( target.getSize().getWidth(), std::locale{ "C" } ) + cuT( " " )
				+ string::toString( target.getSize().getHeight(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " size" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tformat " ) + PF::getFormatName( convert( target.getPixelFormat() ) ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " format" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " tone mapping" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_hdrPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " post effect" );
			}
		}

		if ( result )
		{
			for ( auto const & effect : target.m_srgbPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, target.getName() + " post effect" );
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
		: renderTarget{ renderTarget }
		, colourTexture{ *renderTarget.getEngine() }
	{
	}

	bool RenderTarget::TargetFbo::initialise( RenderDevice const & device
		, ashes::RenderPass & renderPass
		, VkFormat format
		, Size const & size )
	{
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();

		SamplerSPtr sampler = renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + renderTarget.getName() );
		ashes::ImageCreateInfo createInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			format,
			{
				size.getWidth(),
				size.getHeight(),
				1u,
			},
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		};
		auto texture = std::make_shared< TextureLayout >( renderSystem
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, renderTarget.getName() + cuT( "Colour" ) );
		colourTexture.setTexture( texture );
		colourTexture.setSampler( sampler );
		colourTexture.getTexture()->initialise( device );

		ashes::ImageViewCRefArray attaches;
		attaches.emplace_back( colourTexture.getTexture()->getDefaultView().getTargetView() );
		frameBuffer = renderPass.createFrameBuffer( renderTarget.getName()
			, VkExtent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup( RenderDevice const & device )
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
		, m_pixelFormat{ VK_FORMAT_R8G8B8A8_UNORM }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_hdrConfigUbo{ *getEngine() }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + string::toString( m_index ) }
		, m_objectsFrameBuffer{ *this }
		, m_overlaysFrameBuffer{ *this }
		, m_combinedFrameBuffer{ *this }
		, m_velocityTexture{ *getEngine() }
	{
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + getName() + cuT( "Linear" ) );
		sampler->setMinFilter( VK_FILTER_LINEAR );
		sampler->setMagFilter( VK_FILTER_LINEAR );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + getName() + cuT( "Nearest" ) );
		sampler->setMinFilter( VK_FILTER_NEAREST );
		sampler->setMagFilter( VK_FILTER_NEAREST );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::initialise( RenderDevice const & device )
	{
		if ( !m_initialised )
		{
			if ( !m_toneMapping )
			{
				m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" )
					, *getEngine()
					, device
					, m_hdrConfigUbo
					, Parameters{} );
			}

			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			auto & renderSystem = *getEngine()->getRenderSystem();
			m_hdrConfigUbo.initialise( device );
			doInitialiseRenderPass( device );
			m_initialised = doInitialiseFrameBuffer( device );

			if ( m_initialised )
			{
				m_initialised = doInitialiseVelocityTexture( device );
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseTechnique( device );
			}

			m_overlaysTimer = std::make_shared< RenderPassTimer >( *getEngine(), device, cuT( "Overlays" ), cuT( "Overlays" ) );
			m_toneMappingTimer = std::make_shared< RenderPassTimer >( *getEngine(), device, cuT( "Tone Mapping" ), cuT( "Tone Mapping" ) );

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResult();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device, *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( device
					, "HDR"
					, m_hdrCopyCommands
					, sourceView->getDefaultView().getSampledView()
					, m_renderTechnique->getResult().getDefaultView().getTargetView() );
				m_hdrCopyFinished = device->createSemaphore( getName() + "HDRCopy" );
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseToneMapping( device );
			}

			if ( !m_srgbPostEffects.empty() )
			{
				auto const * sourceView = m_objectsFrameBuffer.colourTexture.getTexture().get();

				for ( auto effect : m_srgbPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device, *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( device
					, "SRGB"
					, m_srgbCopyCommands
					, sourceView->getDefaultView().getSampledView()
					, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView().getTargetView() );
				m_srgbCopyFinished = device->createSemaphore( getName() + "SRGBCopy" );
			}

			m_overlayRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem()
				, *device.uboPools
				, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView().getTargetView() );
			m_overlayRenderer->initialise( device );

			doInitialiseCombine( device );

			m_signalReady = device->createSemaphore( getName() + "Ready" );
		}
	}

	void RenderTarget::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_culler.reset();

			m_signalReady.reset();
			m_combinePass.reset();

			m_toneMappingCommandBuffer.reset();

			if ( m_overlayRenderer )
			{
				m_overlayRenderer->cleanup( device );
				m_overlayRenderer.reset();
			}

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			if ( m_toneMapping )
			{
				m_toneMapping->cleanup();
				m_toneMapping.reset();
			}

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
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

			m_renderTechnique->cleanup( device );
			m_hdrConfigUbo.cleanup( device );
			m_overlaysFrameBuffer.cleanup( device );
			m_objectsFrameBuffer.cleanup( device );
			m_combinedFrameBuffer.cleanup( device );
			m_renderTechnique.reset();
			m_renderPass.reset();
			getEngine()->getRenderTechniqueCache().remove( getName() + cuT( "Technique" ) );
		}
	}

	void RenderTarget::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		auto & camera = *getCamera();
		camera.resize( m_size );
		camera.update();

		CU_Require( m_culler );
		m_culler->compute();

		m_hdrConfigUbo.cpuUpdate( getHdrConfig() );

		for ( auto effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}

#if C3D_DebugQuads
		auto technique = this->getTechnique();
		auto & debugConfig = technique->getDebugConfig();
		updater.combineIndex = debugConfig.debugIndex;
		m_combinePass->update( updater );
#endif
	}

	void RenderTarget::update( GpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		updater.jitter = m_jitter;
		updater.scene = getScene();
		updater.camera = getCamera();

		updater.scene->update( updater );

		m_toneMapping->update( updater );
		m_renderTechnique->update( updater );
		m_overlayRenderer->update( updater );

		for ( auto effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
	}

	void RenderTarget::render( RenderDevice const & device
		, RenderInfo & info )
	{
		if ( !m_initialised )
		{
			return;
		}

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
					doRender( device, info, m_objectsFrameBuffer, getCamera() );
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
		getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, name, parameters]( RenderDevice const & device )
			{
				if ( m_toneMapping )
				{
					m_toneMappingCommandBuffer.reset();
					ToneMappingSPtr toneMapping;
					std::swap( m_toneMapping, toneMapping );
					toneMapping->cleanup();
				}

				m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name
					, *getEngine()
					, device
					, m_hdrConfigUbo
					, parameters );
				doInitialiseToneMapping( device );
			} ) );
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

	HdrConfig const & RenderTarget::getHdrConfig()const
	{
		return getCamera()->getHdrConfig();
	}

	HdrConfig & RenderTarget::getHdrConfig()
	{
		return getCamera()->getHdrConfig();
	}

	void RenderTarget::setExposure( float value )
	{
		getCamera()->setExposure( value );
	}

	void RenderTarget::setGamma( float value )
	{
		getCamera()->setGamma( value );
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

	void RenderTarget::doInitialiseRenderPass( RenderDevice const & device )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( getName()
			, std::move( createInfo ) );
	}

	bool RenderTarget::doInitialiseFrameBuffer( RenderDevice const & device )
	{
		auto result = m_objectsFrameBuffer.initialise( device
			, *m_renderPass
			, VK_FORMAT_R8G8B8A8_UNORM
			, m_size );
		auto & dimensions = m_objectsFrameBuffer.colourTexture.getTexture()->getDimensions();
		m_size.set( dimensions.width, dimensions.height );

		if ( result )
		{
			addIntermediateView( "Target Colour"
				, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result = m_overlaysFrameBuffer.initialise( device
				, *m_renderPass
				, VK_FORMAT_R8G8B8A8_UNORM
				, m_size );
		}

		if ( result )
		{
			addIntermediateView( "Target Overlays"
				, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result = m_combinedFrameBuffer.initialise( device
				, *m_renderPass
				, getPixelFormat()
				, m_size );
		}

		return result;
	}

	bool RenderTarget::doInitialiseVelocityTexture( RenderDevice const & device )
	{
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			{ m_size[0], m_size[1], 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		};
		auto velocityTexture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, getName() + cuT( "Velocity" ) );
		m_velocityTexture.setTexture( std::move( velocityTexture ) );
		m_velocityTexture.setSampler( getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + getName() + cuT( "Nearest" ) ) );
		bool result = m_velocityTexture.initialise( device );

		if ( result )
		{
			addIntermediateView( "Target Velocity"
				, m_velocityTexture.getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		return  result;
	}

	bool RenderTarget::doInitialiseTechnique( RenderDevice const & device )
	{
		if ( !m_renderTechnique )
		{
			try
			{
				auto name = getName() + cuT( "Technique" );
				m_renderTechnique = getEngine()->getRenderTechniqueCache().add( name
					, std::make_shared< RenderTechnique >( name
						, *this
						, *getEngine()->getRenderSystem()
						, m_techniqueParameters
						, m_ssaoConfig ) );
			}
			catch ( Exception & p_exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << string::stringCast< xchar >( p_exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return m_renderTechnique->initialise( device, m_intermediates );
	}

	bool RenderTarget::doInitialiseToneMapping( RenderDevice const & device )
	{
		auto result = m_toneMapping->initialise( getSize()
			, m_renderTechnique->getResult()
			, *m_renderPass );

		m_toneMappingCommandBuffer = device.graphicsCommandPool->createCommandBuffer( "ToneMapping" );
		auto const clear{ makeClearValue( 0.0f, 0.0f, 1.0f, 1.0f ) };

		if ( result )
		{
			m_toneMappingCommandBuffer->begin();
			m_toneMappingCommandBuffer->beginDebugBlock(
				{
					"Tone Mapping",
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			m_toneMappingTimer->beginPass( *m_toneMappingCommandBuffer );
			// Put render technique image in shader input layout.
			m_toneMappingCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_renderTechnique->getResult().getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			m_toneMappingCommandBuffer->beginRenderPass( *m_renderPass
				, *m_objectsFrameBuffer.frameBuffer
				, { clear }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_toneMapping->registerPass( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endRenderPass();
			// Put render technique image back in colour attachment layout.
			m_toneMappingCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, m_renderTechnique->getResult().getDefaultView().getTargetView().makeColourAttachment( VK_IMAGE_LAYOUT_UNDEFINED ) );
			m_toneMappingTimer->endPass( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endDebugBlock();
			m_toneMappingCommandBuffer->end();
		}

		return result;
	}

	void RenderTarget::doInitialiseCopyCommands( RenderDevice const & device
		, castor::String const & name
		, ashes::CommandBufferPtr & commandBuffer
		, ashes::ImageView const & source
		, ashes::ImageView const & target )
	{
		commandBuffer = device.graphicsCommandPool->createCommandBuffer( getName() + name + "Copy" );

		commandBuffer->begin();
		commandBuffer->beginDebugBlock(
			{
				getName() + " - " + name + " Copy",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );

		if ( source->image != target->image )
		{
			// Put source image in transfer source layout.
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, source.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			// Put target image in transfer destination layout.
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, target.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			// Copy source to target.
			commandBuffer->copyImage( source, target );
		}

		// Put target image in fragment shader input layout.
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, target.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		commandBuffer->endDebugBlock();
		commandBuffer->end();
	}

	void RenderTarget::doInitialiseCombine( RenderDevice const & device )
	{
		static uint32_t constexpr LhsIdx = 0u;
		static uint32_t constexpr RhsIdx = 1u;
		auto & renderSystem = *getEngine()->getRenderSystem();

		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_textureLhs = writer.declOutput< Vec2 >( "vtx_textureLhs", LhsIdx );
			auto vtx_textureRhs = writer.declOutput< Vec2 >( "vtx_textureRhs", RhsIdx );
			auto out = writer.getOut();

			shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_textureLhs = utils.topDownToBottomUp( uv );
					vtx_textureRhs = uv;

					if ( getTargetType() != TargetType::eWindow )
					{
						vtx_textureLhs.y() = 1.0_f - vtx_textureLhs.y();
						vtx_textureRhs.y() = 1.0_f - vtx_textureRhs.y();
					}

					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			m_combineVtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapLhs = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLhs", 0u, 0u );
			auto c3d_mapRhs = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapRhs", 1u, 0u );

			auto vtx_textureLhs = writer.declInput< Vec2 >( "vtx_textureLhs", LhsIdx );
			auto vtx_textureRhs = writer.declInput< Vec2 >( "vtx_textureRhs", RhsIdx );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto lhsColor = writer.declLocale( "lhsColor"
						, c3d_mapLhs.sample( vtx_textureLhs ) );
					auto rhsColor = writer.declLocale( "rhsColor"
						, c3d_mapRhs.sample( vtx_textureRhs ) );
					lhsColor.rgb() *= 1.0_f - rhsColor.a();
					pxl_fragColor = vec4( lhsColor.rgb() + rhsColor.rgb(), 1.0_f );
				} );
			m_combinePxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		VkExtent2D extent
		{
			m_combinedFrameBuffer.colourTexture.getTexture()->getWidth(),
			m_combinedFrameBuffer.colourTexture.getTexture()->getHeight(),
		};

		CombinePassBuilder mainBuilder{};
		mainBuilder.resultTexture( m_combinedFrameBuffer.colourTexture.getTexture() );
		mainBuilder.texcoordConfig( rq::Texcoord{} );

#if C3D_DebugQuads
		auto intermediates = m_intermediates;
#else
		IntermediateViewArray intermediates{ m_intermediates[0] };
#endif

		m_combinePass = CombinePassBuilder{ mainBuilder }
			.binding( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D )
			.binding( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D )
			.build( *getEngine()
				, device
				, "Target"
				, getPixelFormat()
				, extent
				, m_combineVtx
				, m_combinePxl
				, intermediates
				, { ""
					, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView().getSampledView()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
	}

	void RenderTarget::doRender( RenderDevice const & device
		, RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto elapsedTime = m_timer.getElapsed();
		SceneSPtr scene = getScene();
		ashes::SemaphoreCRefArray signalsToWait;

		if ( m_type == TargetType::eWindow )
		{
			signalsToWait = scene->getRenderTargetsSemaphores();
		}

		//signalsToWait.push_back( toWait );

		// Render the scene through the RenderTechnique.
		m_signalFinished = &m_renderTechnique->render( device
			, signalsToWait
			, info );

		// Draw HDR post effects.
		m_signalFinished = &doApplyPostEffects( device
			, *m_signalFinished
			, m_hdrPostEffects
			, m_hdrCopyCommands
			, m_hdrCopyFinished
			, elapsedTime );

		// Then draw the render's result to the RenderTarget's frame buffer, applying the tone mapping operator.
		m_signalFinished = &doApplyToneMapping( device
			, *m_signalFinished );

		// Apply the sRGB post effects.
		m_signalFinished = &doApplyPostEffects( device
			, *m_signalFinished
			, m_srgbPostEffects
			, m_srgbCopyCommands
			, m_srgbCopyFinished
			, elapsedTime );

		// And now render overlays.
		m_signalFinished = &doRenderOverlays( device
			, *m_signalFinished );

		// Combine objects and overlays framebuffers, flipping them if necessary.
		m_signalFinished = &doCombine( *m_signalFinished );
	}

	ashes::Semaphore const & RenderTarget::doApplyPostEffects( RenderDevice const & device
		, ashes::Semaphore const & toWait
		, PostEffectPtrArray const & effects
		, ashes::CommandBufferPtr const & copyCommandBuffer
		, ashes::SemaphorePtr const & copyFinished
		, castor::Nanoseconds const & elapsedTime )
	{
		auto * result = &toWait;

		if ( !effects.empty() )
		{
			auto & queue = *device.graphicsQueue;

			for ( auto effect : effects )
			{
				auto timerBlock = effect->start();

				for ( auto & commands : effect->getCommands() )
				{
					timerBlock->notifyPassRender();

					queue.submit( *commands.commandBuffer
						, *result
						, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
						, *commands.semaphore
						, nullptr );
					result = commands.semaphore.get();
				}
			}

			queue.submit( *copyCommandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *copyFinished
				, nullptr );
			result = copyFinished.get();
		}

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doApplyToneMapping( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto & queue = *device.graphicsQueue;
		auto timerBlock = m_toneMappingTimer->start();
		timerBlock->notifyPassRender();
		auto * result = &toWait;

		queue.submit( *m_toneMappingCommandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, m_toneMapping->getSemaphore()
			, nullptr );
		result = &m_toneMapping->getSemaphore();

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doRenderOverlays( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		auto timerBlock = m_overlaysTimer->start();
		{
			using LockType = std::unique_lock< OverlayCache >;
			LockType lock{ castor::makeUniqueLock( getEngine()->getOverlayCache() ) };
			m_overlayRenderer->beginPrepare( *m_overlaysTimer
				, *result );
			auto preparer = m_overlayRenderer->getPreparer( device );

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
			m_overlayRenderer->render( device, *m_overlaysTimer );
			result = &m_overlayRenderer->getSemaphore();
		}
		return *result;
	}

	ashes::Semaphore const & RenderTarget::doCombine( ashes::Semaphore const & toWait )
	{
		return m_combinePass->combine( toWait );
	}
}
