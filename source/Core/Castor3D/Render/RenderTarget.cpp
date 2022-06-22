#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <RenderGraph/FramePassTimer.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderTarget )

namespace castor3d
{
	namespace rendtgt
	{
		enum CombineIdx
		{
			CombineLhsIdx = 0u,
			CombineRhsIdx = 1u,
		};

		class IntermediatesLister
			: public RenderTechniqueVisitor
		{
			struct VkImageViewCreateInfoComp
			{
				bool operator()( VkImageViewCreateInfo const & lhs
					, VkImageViewCreateInfo const & rhs )const
				{
					return ( lhs.image < rhs.image
						|| ( lhs.image == rhs.image
							&& ( lhs.subresourceRange.baseArrayLayer < rhs.subresourceRange.baseArrayLayer
								|| ( lhs.subresourceRange.baseArrayLayer == rhs.subresourceRange.baseArrayLayer
									&& ( lhs.subresourceRange.baseMipLevel < rhs.subresourceRange.baseMipLevel
										|| ( lhs.subresourceRange.baseMipLevel == rhs.subresourceRange.baseMipLevel
											&& ( lhs.subresourceRange.layerCount < rhs.subresourceRange.layerCount
												|| ( lhs.subresourceRange.layerCount == rhs.subresourceRange.layerCount
													&& ( lhs.subresourceRange.levelCount < rhs.subresourceRange.levelCount
														|| ( lhs.subresourceRange.levelCount == rhs.subresourceRange.levelCount
															&& ( lhs.viewType < rhs.viewType
																|| ( lhs.viewType == rhs.viewType
																	&& ( lhs.format < rhs.format ) ) ) ) ) ) ) ) ) ) ) ) );

				}
			};

		public:
			static void submit( RenderTechnique & technique
				, IntermediateViewArray & intermediates )
			{
				std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > cache;

				PipelineFlags flags{};
				flags.passType = technique.getEngine()->getPassesType();
				IntermediatesLister visOpaque{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visOpaque );

				flags.passFlags |= PassFlag::eAlphaBlending;
				IntermediatesLister visTransparent{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visTransparent );
			}

			static void submit( Scene const & scene
				, PostEffect & effect
				, IntermediateViewArray & intermediates )
			{
				std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > cache;

				PipelineFlags flags{};
				IntermediatesLister vis{ flags, scene, cache, intermediates };
				effect.accept( vis );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & cache
				, IntermediateViewArray & result )
				: RenderTechniqueVisitor{ flags, scene, { true, false } }
				, m_handler{ scene.getEngine()->getGraphResourceHandler() }
				, m_result{ result }
				, m_cache{ cache }
			{
			}

			void doVisitArray( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto & info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY ) )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;
						auto layerViewId = m_handler.createViewId( crg::ImageViewData{ viewId.data->name + std::to_string( layer )
							, viewId.data->image
							, layerInfo.flags
							, layerInfo.viewType
							, layerInfo.format
							, layerInfo.subresourceRange } );

						if ( doFilter( layerViewId ) )
						{
							doVisit( name + std::to_string( layer )
								, layerViewId
								, layout
								, factors );
						}

						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}
			}

			void doVisit( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )override
			{
				auto info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
					|| ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
						&& ( info.subresourceRange.layerCount == 1u ) ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					m_cache.insert( info );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else
				{
					doVisitArray( name, viewId, layout, factors );
				}
			}

			bool doFilterArray( crg::ImageViewId const & viewId )const
			{
				bool result = false;
				auto & info = viewId.data->info;

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE
					|| info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY ) )
				{
					auto layerInfo = info;
					layerInfo.subresourceRange.layerCount = 1u;

					for ( uint32_t layerIdx = 0u; layerIdx < info.subresourceRange.layerCount; ++layerIdx )
					{
						auto layer = layerIdx + info.subresourceRange.baseArrayLayer;
						result = doFilter( m_handler.createViewId( crg::ImageViewData{ viewId.data->name + std::to_string( layer )
							, viewId.data->image
							, layerInfo.flags
							, layerInfo.viewType
							, layerInfo.format
							, layerInfo.subresourceRange } ) ) && result;
						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}

				return result;
			}

			bool doFilter( crg::ImageViewId const & viewId )const override
			{
				auto info = viewId.data->info;

				if ( info.subresourceRange.levelCount > 1u
					&& info.viewType != VK_IMAGE_VIEW_TYPE_3D )
				{
					return false;
				}

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
					|| ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
						&& ( info.subresourceRange.layerCount == 1u ) ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( info );
				}

				return doFilterArray( viewId );
			}

		private:
			crg::ResourceHandler & m_handler;
			IntermediateViewArray & m_result;
			std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & m_cache;
		};
	}

	//*********************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine
		, TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_size{ size }
		, m_safeBandedSize{ getSafeBandedSize( size ) }
		, m_pixelFormat{ VkFormat( pixelFormat ) }
		, m_initialised{ false }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + castor::string::toString( m_index ) }
		, m_graph{ getOwner()->getGraphResourceHandler(), m_name }
		, m_velocity{ std::make_shared< Texture >( getOwner()->getRenderSystem()->getRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Velocity"
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK ) }
		, m_objects{ getOwner()->getRenderSystem()->getRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Scene"
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_overlays{ getOwner()->getRenderSystem()->getRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Overlays"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combined{ getOwner()->getRenderSystem()->getRenderDevice()
			, getOwner()->getGraphResourceHandler()
			, "Target"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, getPixelFormat()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_combinePass{ doCreateCombinePass( nullptr ) }
	{
		auto sampler = engine.addNewSampler( RenderTarget::DefaultSamplerName + getName() + cuT( "Linear" ), engine ).lock();
		sampler->setMinFilter( VK_FILTER_LINEAR );
		sampler->setMagFilter( VK_FILTER_LINEAR );

		sampler = engine.addNewSampler( RenderTarget::DefaultSamplerName + getName() + cuT( "Nearest" ), engine ).lock();
		sampler->setMinFilter( VK_FILTER_NEAREST );
		sampler->setMagFilter( VK_FILTER_NEAREST );

		for ( auto name : engine.getPostEffectFactory().listRegisteredTypes() )
		{
			PostEffectSPtr effect = engine.getPostEffectFactory().create( name
				, *this
				, *engine.getRenderSystem()
				, Parameters{} );
			effect->enable( false );

			if ( effect->isAfterToneMapping() )
			{
				m_srgbPostEffects.push_back( effect );
			}
			else
			{
				m_hdrPostEffects.push_back( effect );
			}
		}
	}

	RenderTarget::~RenderTarget()
	{
	}

	uint32_t RenderTarget::countInitialisationSteps()const
	{
		uint32_t result = 0u;
		result += 1; // combine program
		result += RenderTechnique::countInitialisationSteps();
		result += uint32_t( m_hdrPostEffects.size() );
		result += 2; // HDR copy commands
		result += 2; // tone mapping
		result += uint32_t( m_srgbPostEffects.size() );
		result += 2; // SRGB copy commands
		result += 1; // compiling render graph
		result += 1; // overlay renderer
		return result;
	}

	void RenderTarget::initialise( RenderDevice const & device
		, QueueData const & queueData
		, ProgressBar * progress )
	{
		if ( !m_initialised
			&& !m_initialising.exchange( true ) )
		{
			m_hdrConfigUbo = std::make_unique< HdrConfigUbo >( device );
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			doInitCombineProgram( progress );
			auto result = doInitialiseTechnique( device, queueData, progress );
			setProgressBarTitle( progress, "Initialising: Render Target" );
			auto * previousPass = &m_renderTechnique->getLastPass();

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResultImgView();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( result )
					{
						stepProgressBar( progress, "Initialising post effect " + effect->getName() );
						result = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( result )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "HDR"
						, *sourceView
						, m_renderTechnique->getResultImgView()
						, *previousPass
						, progress );
				}
			}

			if ( result )
			{
				m_hdrLastPass = previousPass;
				stepProgressBar( progress, "Creating tone mapping pass" );
				m_toneMapping = std::make_shared< ToneMapping >( *getEngine()
					, device
					, m_size
					, m_graph
					, m_renderTechnique->getResultImgView()
					, m_objects.wholeViewId
					, *m_hdrLastPass
					, *m_hdrConfigUbo
					, Parameters{}
					, progress );
				m_toneMapping->initialise( m_toneMappingName );
				previousPass = &m_toneMapping->getPass();
			}

			if ( !m_srgbPostEffects.empty() ) 
			{
				auto const * sourceView = &m_objects.wholeViewId;

				for ( auto effect : m_srgbPostEffects )
				{
					if ( result )
					{
						stepProgressBar( progress, "Initialising post effect " + effect->getName() );
						result = effect->initialise( device
							, *sourceView
							, *previousPass );
						previousPass = &effect->getPass();
						sourceView = &effect->getResult();
					}
				}

				if ( result )
				{
					previousPass = &doInitialiseCopyCommands( device
						, "SRGB"
						, *sourceView
						, m_objects.wholeViewId
						, *previousPass
						, progress );
				}
			}

			if ( result )
			{
				stepProgressBar( progress, "Compiling render graph" );
				m_combinePass.addDependency( *previousPass );
				m_runnable = m_graph.compile( device.makeContext() );
				printGraph( *m_runnable );

				m_objects.create();
				m_overlays.create();
				m_velocity->create();
				m_combined.create();
				auto runnable = m_runnable.get();
				device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [runnable, result, this]( RenderDevice const &
						, QueueData const & )
					{
						runnable->record();
						m_initialised = result;
					} ) );
			}

			stepProgressBar( progress, "Creating overlay renderer" );
			m_overlaysTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), getName() + cuT( "/Overlays" ) );
			m_overlayRenderer = std::make_shared< OverlayRenderer >( device
				, m_overlays
				, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
			getEngine()->registerTimer( getName() + cuT( "/Overlays/Overlays" ), *m_overlaysTimer );
#if C3D_DebugTimers
			m_testTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "AAATests" ) );
			getEngine()->registerTimer( cuT( "AAATests" ), *m_testTimer );
#endif
			m_signalReady = device->createSemaphore( getName() + "Ready" );
			m_initialising = false;
		}

		while ( m_initialising )
		{
			std::this_thread::sleep_for( 1_ms );
		}
	}

	void RenderTarget::cleanup( RenderDevice const & device )
	{
		if ( m_initialised.exchange( false ) )
		{
			m_signalReady.reset();
			m_overlayRenderer.reset();
#if C3D_DebugTimers
			getEngine()->unregisterTimer( cuT( "AAATests" ), *m_testTimer );
			m_testTimer.reset();
#endif
			getEngine()->unregisterTimer( getName() + cuT( "/Overlays/Overlays" ), *m_overlaysTimer );
			m_overlaysTimer.reset();
			m_runnable.reset();

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			m_toneMapping.reset();

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
			}

			m_renderTechnique.reset();
			m_objects.destroy();
			m_overlays.destroy();
			m_velocity->destroy();
			m_combined.destroy();
			m_combineStages.clear();
			m_combinePxl.shader.reset();
			m_combineVtx.shader.reset();
			m_culler.reset();
			m_hdrConfigUbo.reset();
		}
	}

	void RenderTarget::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_testTimer->start() );
#endif

		auto & camera = *getCamera();
		auto & scene = *getScene();
		updater.scene = &scene;
		updater.camera = &camera;
		camera.resize( m_size );
		camera.update();

		CU_Require( m_culler );
		m_culler->update( updater );
		m_renderTechnique->update( updater );

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

#if C3D_DebugTimers
		auto block( m_testTimer->start() );
#endif

		auto & camera = *getCamera();
		auto & scene = *getScene();
		updater.jitter = m_jitter;
		updater.scene = &scene;
		updater.camera = &camera;
		// Scene is GPU updated here, because of LightCache which relies on the camera...
		// TODO: Move LightCache to RenderTarget :'(
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

	void RenderTarget::upload( ashes::CommandBuffer const & cb )
	{
		if ( m_initialised )
		{
			m_overlayRenderer->upload( cb );
		}
	}

	crg::SemaphoreWaitArray RenderTarget::render( RenderDevice const & device
		, RenderInfo & info
		, ashes::Queue const & queue
		, crg::SemaphoreWaitArray const & signalsToWait )
	{
		if ( !m_initialised )
		{
			return signalsToWait;
		}

		crg::SemaphoreWaitArray result{};
		auto scene = getScene();

		if ( scene )
		{
			if ( m_initialised && scene->isInitialised() )
			{
				CameraSPtr camera = getCamera();

				if ( camera )
				{
					getEngine()->getRenderSystem()->pushScene( scene );
					result = doRender( device, info, queue, getCamera(), signalsToWait );
					getEngine()->getRenderSystem()->popScene();
				}
			}
		}

		return result;
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

	void RenderTarget::setScene( Scene & scene )
	{
		auto myScene = getScene();

		if ( myScene != &scene )
		{
			m_scene = &scene;
			m_culler.reset();
		}
	}

	void RenderTarget::setToneMappingType( castor::String const & name
		, Parameters const & parameters )
	{
		m_toneMappingName = name;

		if ( m_toneMapping )
		{
			getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					if ( m_initialised )
					{
						m_toneMapping->updatePipeline( m_toneMappingName );
					}
					else
					{
						m_toneMapping->initialise( m_toneMappingName );
					}
				} ) );
		}
	}

	PostEffectSPtr RenderTarget::getPostEffect( castor::String const & name )const
	{
		auto it = std::find_if( m_srgbPostEffects.begin()
			, m_srgbPostEffects.end()
			, [&name]( PostEffectSPtr lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_srgbPostEffects.end() )
		{
			return *it;
		}

		it = std::find_if( m_hdrPostEffects.begin()
			, m_hdrPostEffects.end()
			, [&name]( PostEffectSPtr lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_hdrPostEffects.end() )
		{
			return *it;
		}

		return nullptr;
	}

	HdrConfig const & RenderTarget::getHdrConfig()const
	{
		return getCamera()->getHdrConfig();
	}

	HdrConfig & RenderTarget::getHdrConfig()
	{
		return getCamera()->getHdrConfig();
	}

	ShadowMapLightTypeArray RenderTarget::getShadowMaps()const
	{
		if ( m_renderTechnique )
		{
			return m_renderTechnique->getShadowMaps();
		}

		return {};
	}

	TechniquePassVector RenderTarget::getCustomRenderPasses()const
	{
		if ( m_renderTechnique )
		{
			return m_renderTechnique->getCustomRenderPasses();
		}

		return {};
	}

	void RenderTarget::setExposure( float value )
	{
		if ( auto camera = getCamera() )
		{
			camera->setExposure( value );
		}
	}

	void RenderTarget::setGamma( float value )
	{
		if ( auto camera = getCamera() )
		{
			camera->setGamma( value );
		}
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::listIntermediateViews( IntermediateViewArray & result )const
	{
		result.emplace_back( "Target Result"
			, m_combined
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.emplace_back( "Target Colour"
			, m_objects
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( "Target Overlays"
			, m_overlays
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{ { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f } }.invert( true ) );
		result.emplace_back( "Target Velocity"
			, *m_velocity
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		for ( auto & postEffect : m_hdrPostEffects )
		{
			rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
		}

		for ( auto & postEffect : m_srgbPostEffects )
		{
			rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
		}

		if ( m_renderTechnique )
		{
			rendtgt::IntermediatesLister::submit( *m_renderTechnique, result );
		}
	}

	void RenderTarget::resetSemaphore()
	{
		m_signalFinished.clear();
	}

	crg::FramePass const & RenderTarget::createVertexTransformPass()
	{
		return getScene()->getRenderNodes().createVertexTransformPass( m_graph );
	}

	crg::FramePass & RenderTarget::doCreateCombinePass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating combine pass" );
		auto & result = m_graph.createPass( "Other/Combine"
			, [this, progress]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising combine pass" );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( m_combined.getExtent() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_combineStages ) )
					.build( pass, context, graph );
				getOwner()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} );
		result.addSampledView( m_objects.sampledViewId
			, rendtgt::CombineLhsIdx );
		result.addSampledView( m_overlays.sampledViewId
			, rendtgt::CombineRhsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addOutputColourView( m_combined.targetViewId );
		return result;
	}

	bool RenderTarget::doInitialiseTechnique( RenderDevice const & device
		, QueueData const & queueData
		, ProgressBar * progress )
	{
		if ( !m_renderTechnique )
		{
			try
			{
				setProgressBarTitle( progress, "Initialising: Render Technique" );
				m_renderTechnique = castor::makeUnique< RenderTechnique >( getName()
					, *this
					, device
					, queueData
					, m_techniqueParameters
					, m_ssaoConfig
					, progress );
			}
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << castor::string::stringCast< xchar >( exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return true;
	}

	crg::FramePass const & RenderTarget::doInitialiseCopyCommands( RenderDevice const & device
		, castor::String const & name
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		if ( source != target )
		{
			stepProgressBar( progress, "Creating " + name + " copy commands" );
			auto & pass = m_graph.createPass( "Other/" + name + "Copy"
				, [this, progress, name]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					stepProgressBar( progress, "Initialising " + name + " copy commands" );
					auto result = std::make_unique< crg::ImageCopy >( pass
						, context
						, graph
						, getSafeBandedExtent3D( m_size ) );
					getOwner()->registerTimer( pass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( previousPass );
			pass.addTransferInputView( source );
			pass.addTransferOutputView( target );
			return pass;
		}

		return previousPass;
	}

	void RenderTarget::doInitCombineProgram( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating combine program" );
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto bandSize = double( castor3d::getSafeBandSize(m_size ) );
		auto bandedSize = castor3d::getSafeBandedExtent3D(m_size );
		auto bandRatioU = bandSize / bandedSize.width;
		auto bandRatioV = bandSize / bandedSize.height;
		castor::Point4f velocityMetrics{ bandRatioU
			, bandRatioV
			, float( 1.0 - 2.0 * bandRatioU )
			, float( 1.0 - 2.0 * bandRatioV ) };

		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_textureLhs = writer.declOutput< Vec2 >( "vtx_textureLhs", rendtgt::CombineLhsIdx );
			auto vtx_textureRhs = writer.declOutput< Vec2 >( "vtx_textureRhs", rendtgt::CombineRhsIdx );

			shader::Utils utils{ writer };

			auto getSafeBandedCoord = [&]( Vec2 const & texcoord )
			{
				return vec2( texcoord.x() * velocityMetrics->z + velocityMetrics->x
					, texcoord.y() * velocityMetrics->w + velocityMetrics->y );
			};

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_textureLhs = utils.topDownToBottomUp( uv );
					vtx_textureRhs = uv;

					if ( getTargetType() != TargetType::eWindow )
					{
						vtx_textureLhs.y() = 1.0_f - vtx_textureLhs.y();
						vtx_textureRhs.y() = 1.0_f - vtx_textureRhs.y();
					}

					vtx_textureLhs = getSafeBandedCoord( vtx_textureLhs );
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			m_combineVtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapLhs = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLhs", rendtgt::CombineLhsIdx, 0u );
			auto c3d_mapRhs = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapRhs", rendtgt::CombineRhsIdx, 0u );

			auto vtx_textureLhs = writer.declInput< Vec2 >( "vtx_textureLhs", rendtgt::CombineLhsIdx );
			auto vtx_textureRhs = writer.declInput< Vec2 >( "vtx_textureRhs", rendtgt::CombineRhsIdx );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
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

		m_combineStages.push_back( makeShaderState( renderSystem.getRenderDevice(), m_combineVtx ) );
		m_combineStages.push_back( makeShaderState( renderSystem.getRenderDevice(), m_combinePxl ) );
	}

	crg::SemaphoreWaitArray RenderTarget::doRender( RenderDevice const & device
		, RenderInfo & info
		, ashes::Queue const & queue
		, CameraSPtr camera
		, crg::SemaphoreWaitArray signalsToWait )
	{
		auto scene = getScene();

		if ( m_type == TargetType::eWindow )
		{
			auto targetSemaphores = scene->getRenderTargetsSemaphores();
			signalsToWait.insert( signalsToWait.end()
				, targetSemaphores.begin()
				, targetSemaphores.end() );
		}

		// Render overlays.
		m_signalFinished = doRenderOverlays( device
			, queue
			, signalsToWait );

		// Compute all that is needed for the rendering of the scene.
		m_signalFinished = m_renderTechnique->preRender( m_signalFinished
			, queue );

		// Then run the graph
		m_signalFinished = m_runnable->run( m_signalFinished
			, queue );

		return m_signalFinished;
	}

	crg::SemaphoreWaitArray RenderTarget::doRenderOverlays( RenderDevice const & device
		, ashes::Queue const & queue
		, crg::SemaphoreWaitArray const & toWait )
	{
		auto timerBlock = m_overlaysTimer->start();
		m_overlayRenderer->beginPrepare( *m_overlaysTimer );
		auto preparer = m_overlayRenderer->getPreparer( device );

		if ( m_type == TargetType::eWindow )
		{
			auto lock( castor::makeUniqueLock( getEngine()->getOverlayCache() ) );

			for ( auto category : getEngine()->getOverlayCache().getCategories() )
			{
				if ( category->getOverlay().isVisible() )
				{
					category->update( *m_overlayRenderer );
					category->accept( preparer );
				}
			}
		}

		auto lock( castor::makeUniqueLock( getScene()->getOverlayCache() ) );

		for ( auto category : getScene()->getOverlayCache().getCategories() )
		{
			if ( category->getOverlay().isVisible() )
			{
				category->update( *m_overlayRenderer );
				category->accept( preparer );
			}
		}

		m_overlayRenderer->endPrepare( *m_overlaysTimer );
		return m_overlayRenderer->render( *m_overlaysTimer
			, queue
			, toWait );
	}
}
