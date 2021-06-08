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
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

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

CU_ImplementCUSmartPtr( castor3d, RenderTarget )

using namespace castor;

namespace castor3d
{
	RenderTarget::TargetFbo::TargetFbo( RenderTarget & renderTarget )
		: renderTarget{ renderTarget }
	{
	}

	bool RenderTarget::TargetFbo::initialise( RenderDevice const & device
		, ashes::ImagePtr image
		, ashes::ImageView view
		, ashes::RenderPass & renderPass )
	{
		colourImage = std::move( image );
		colourView = std::move( view );
		ashes::ImageViewCRefArray attaches;
		attaches.emplace_back( colourView );
		frameBuffer = renderPass.createFrameBuffer( renderTarget.getName()
			, VkExtent2D{ colourImage->getDimensions().width, colourImage->getDimensions().height }
			, std::move( attaches ) );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup( RenderDevice const & device )
	{
		colourImage.reset();
		frameBuffer.reset();
	}

	//*************************************************************************************************
	
	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine
		, TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_size{ size }
		, m_pixelFormat{ VkFormat( pixelFormat ) }
		, m_initialised{ false }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + string::toString( m_index ) }
		, m_objectsFrameBuffer{ *this }
		, m_overlaysFrameBuffer{ *this }
		, m_combinedFrameBuffer{ *this }
		, m_graph{ getOwner()->getGraphResourceHandler(), m_name }
		, m_velocity{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Velocity"
			, 0u
			, castor3d::makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_objects{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Scene"
			, 0u
			, castor3d::makeExtent3D( m_size )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_overlays{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Overlays"
			, 0u
			, castor3d::makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combined{ *getOwner()->getRenderSystem()->getMainRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Target"
			, 0u
			, castor3d::makeExtent3D( m_size )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combinePass{ doCreateCombinePass() }
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
			auto & renderSystem = device.renderSystem;

			m_hdrConfigUbo = std::make_unique< HdrConfigUbo >( device );

			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			doInitialiseRenderPass( device );
			doInitCombineProgram();
			m_initialised = doInitialiseTechnique( device );

			m_overlaysTimer = std::make_shared< RenderPassTimer >( device, cuT( "Overlays" ), cuT( "Overlays" ) );
			auto * previousPass = &m_renderTechnique->getLastPass();

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResultImgView();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( m_initialised )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "HDR"
						, *sourceView
						, m_renderTechnique->getResultImgView()
						, *previousPass );
				}
			}

			if ( m_initialised )
			{
				m_hdrLastPass = previousPass;
				m_toneMapping = std::make_shared< ToneMapping >( *getEngine()
					, device
					, m_size
					, m_graph
					, m_renderTechnique->getResultImgView()
					, m_objects.wholeViewId
					, *m_hdrLastPass
					, *m_hdrConfigUbo
					, Parameters{} );
				m_toneMapping->initialise( cuT( "linear" ) );
				previousPass = &m_toneMapping->getPass();
			}

			if ( !m_srgbPostEffects.empty() ) 
			{
				auto const * sourceView = &m_objects.wholeViewId;

				for ( auto effect : m_srgbPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( m_initialised )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "SRGB"
						, *sourceView
						, m_objects.wholeViewId
						, *previousPass );
				}
			}

			if ( m_initialised )
			{
				m_combinePass.addDependency( *previousPass );
				m_runnable = m_graph.compile( device.makeContext() );

				m_objects.image = m_runnable->createImage( m_objects.imageId );
				m_objects.wholeView = m_runnable->createImageView( m_objects.wholeViewId );
				auto image = std::make_unique< ashes::Image >( *device
					, m_objects.image
					, ashes::ImageCreateInfo{ m_objects.imageId.data->info } );
				ashes::ImageView view{ ashes::ImageViewCreateInfo{ m_objects.image, m_objects.wholeViewId.data->info }
					, m_objects.wholeView
					, image.get() };
				m_objectsFrameBuffer.initialise( device
					, std::move( image )
					, view
					, *m_renderPass );

				m_overlays.image = m_runnable->createImage( m_overlays.imageId );
				m_overlays.wholeView = m_runnable->createImageView( m_overlays.wholeViewId );
				image = std::make_unique< ashes::Image >( *device
					, m_overlays.image
					, ashes::ImageCreateInfo{ m_overlays.imageId.data->info } );
				view = ashes::ImageView{ ashes::ImageViewCreateInfo{ m_overlays.image, m_overlays.wholeViewId.data->info }
					, m_overlays.wholeView
					, image.get() };

				m_velocity.image = m_runnable->createImage( m_velocity.imageId );
				m_velocity.wholeView = m_runnable->createImageView( m_velocity.wholeViewId );
				m_velocityImage = std::make_unique< ashes::Image >( *device
					, m_velocity.image
					, ashes::ImageCreateInfo{ m_velocity.imageId.data->info } );
				m_velocityImageView = ashes::ImageView{ ashes::ImageViewCreateInfo{ m_velocity.image, m_velocity.wholeViewId.data->info }
					, m_velocity.wholeView
					, image.get() };
				m_overlaysFrameBuffer.initialise( device
					, std::move( image )
					, view
					, *m_renderPass );

				m_combined.image = m_runnable->createImage( m_combined.imageId );
				m_combined.wholeView = m_runnable->createImageView( m_combined.wholeViewId );
				image = std::make_unique< ashes::Image >( *device
					, m_combined.image
					, ashes::ImageCreateInfo{ m_combined.imageId.data->info } );
				view = ashes::ImageView{ ashes::ImageViewCreateInfo{ m_combined.image, m_combined.wholeViewId.data->info }
					, m_combined.wholeView
					, image.get() };
				m_combinedFrameBuffer.initialise( device
					, std::move( image )
					, view
					, *m_renderPass );

				m_runnable->record();
			}

			m_overlayRenderer = std::make_shared< OverlayRenderer >( device
				, m_overlaysFrameBuffer.colourView );

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
			m_runnable.reset();

			m_overlayRenderer.reset();

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			m_toneMapping.reset();

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
			}

			m_hdrPostEffects.clear();
			m_srgbPostEffects.clear();

			m_overlaysTimer.reset();

			m_hdrConfigUbo.reset();
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
		updater.camera = getCamera();
		camera.resize( m_size );
		camera.update();

		CU_Require( m_culler );
		m_culler->compute();

		m_hdrConfigUbo->cpuUpdate( getHdrConfig() );

		for ( auto effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
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
				if ( m_initialised )
				{
					m_toneMapping->updatePipeline( name );
				}
				else
				{
					m_toneMapping->initialise( name );
				}
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

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::listIntermediateViews( IntermediateViewArray & result )const
	{
		result.push_back( { "Target Colour"
			, m_objectsFrameBuffer.colourView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) } );
		result.push_back( { "Target Overlays"
			, m_overlaysFrameBuffer.colourView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) } );
		result.push_back( { "Target Velocity"
			, m_velocityImageView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) } );

		if ( auto technique = getTechnique() )
		{
			technique->listIntermediates( result );
		}
	}

	crg::FramePass & RenderTarget::doCreateCombinePass()
	{
		auto & result = m_graph.createPass( "CombinePass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_size ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_combineStages ) )
					.build( pass, context, graph );
			} );
		result.addSampledView( m_objects.wholeViewId
			, 0u
			, {} );
		result.addSampledView( m_overlays.wholeViewId
			, 1u
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addOutputColourView( m_combined.wholeViewId );
		return result;
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
						, device
						, m_techniqueParameters
						, m_ssaoConfig ) );
			}
			catch ( Exception & p_exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << string::stringCast< xchar >( p_exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return true;
	}

	crg::FramePass const & RenderTarget::doInitialiseCopyCommands( RenderDevice const & device
		, castor::String const & name
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass )
	{
		if ( source != target )
		{
			auto & pass = m_graph.createPass( getName() + name + "Copy"
				, [this, &source, &target]( crg::FramePass const & pass
					, crg::GraphContext const & context
					, crg::RunnableGraph & graph )
				{
					return std::make_unique< crg::ImageCopy >( pass
						, context
						, graph
						, makeExtent3D( m_size ) );
				} );
			pass.addDependency( previousPass );
			pass.addTransferInputView( source );
			pass.addTransferOutputView( target );
			return pass;
		}

		return previousPass;
	}

	void RenderTarget::doInitCombineProgram()
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

		m_combineStages.push_back( makeShaderState( *renderSystem.getMainRenderDevice(), m_combineVtx ) );
		m_combineStages.push_back( makeShaderState( *renderSystem.getMainRenderDevice(), m_combinePxl ) );
	}

	void RenderTarget::doRender( RenderDevice const & device
		, RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto elapsedTime = m_timer.getElapsed();
		SceneSPtr scene = getScene();
		crg::SemaphoreWaitArray signalsToWait;

		if ( m_type == TargetType::eWindow )
		{
			signalsToWait = scene->getRenderTargetsSemaphores();
		}

		// Render overlays.
		m_signalFinished = doRenderOverlays( device
			, signalsToWait );

		m_signalFinished = m_renderTechnique->preRender( m_signalFinished );

		// Then run the graph
		m_signalFinished = m_runnable->run( m_signalFinished
			, *device.graphicsQueue );
	}

	crg::SemaphoreWait RenderTarget::doRenderOverlays( RenderDevice const & device
		, crg::SemaphoreWaitArray const & toWait )
	{
		crg::SemaphoreWait result;
		auto timerBlock = m_overlaysTimer->start();
		using LockType = std::unique_lock< OverlayCache >;
		LockType lock{ castor::makeUniqueLock( getEngine()->getOverlayCache() ) };
		m_overlayRenderer->beginPrepare( *m_overlaysTimer, toWait );
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
		return m_overlayRenderer->render( *m_overlaysTimer );
	}
}
