#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Debug/DebugDrawer.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Overlays/OverlayPass.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
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

CU_ImplementSmartPtr( castor3d, RenderTarget )

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
			struct ImageViewSlice
			{
				VkImageViewCreateInfo info;
				uint32_t slice;
			};

			struct ImageViewSliceComp
			{
				bool operator()( ImageViewSlice const & lhs
					, ImageViewSlice const & rhs )const
				{
					return ( lhs.info.image < rhs.info.image
						|| ( lhs.info.image == rhs.info.image
							&& ( lhs.info.subresourceRange.baseArrayLayer < rhs.info.subresourceRange.baseArrayLayer
								|| ( lhs.info.subresourceRange.baseArrayLayer == rhs.info.subresourceRange.baseArrayLayer
									&& ( lhs.info.subresourceRange.baseMipLevel < rhs.info.subresourceRange.baseMipLevel
										|| ( lhs.info.subresourceRange.baseMipLevel == rhs.info.subresourceRange.baseMipLevel
											&& ( lhs.info.subresourceRange.layerCount < rhs.info.subresourceRange.layerCount
												|| ( lhs.info.subresourceRange.layerCount == rhs.info.subresourceRange.layerCount
													&& ( lhs.info.subresourceRange.levelCount < rhs.info.subresourceRange.levelCount
														|| ( lhs.info.subresourceRange.levelCount == rhs.info.subresourceRange.levelCount
															&& ( lhs.info.viewType < rhs.info.viewType
																|| ( lhs.info.viewType == rhs.info.viewType
																	&& ( lhs.info.format < rhs.info.format
																		|| ( lhs.info.format == rhs.info.format
																			&& lhs.slice < rhs.slice ) ) ) ) ) ) ) ) ) ) ) ) ) );

				}
			};
			using ImageViewCache = std::set< ImageViewSlice, ImageViewSliceComp >;

		public:
			static void submit( Scene const & scene
				, RenderTechnique & technique
				, IntermediateViewArray & intermediates )
			{
				ImageViewCache cache;

				PipelineFlags flags{ PassComponentCombine{}
					, scene.getDefaultLightingModel()
					, scene.getBackgroundModelId()
					, SubmeshFlag::ePosNmlTanTex
					, ProgramFlag::eNone
					, TextureCombine{}
					, ShaderFlag::eNone
					, VK_COMPARE_OP_ALWAYS };
				IntermediatesLister visOpaque{ flags, scene, cache, intermediates };
				technique.accept( visOpaque );

				flags.m_shaderFlags |= ShaderFlag::eOpacity;
				addFlags( flags.components, scene.getEngine()->getPassComponentsRegister().getAlphaBlendingFlag() );
				flags.alphaFunc = VK_COMPARE_OP_LESS;
				IntermediatesLister visTransparent{ flags, scene, cache, intermediates };
				technique.accept( visTransparent );
			}

			template< typename ValueT >
			static void submit( Scene const & scene
				, ValueT & value
				, IntermediateViewArray & intermediates )
			{
				ImageViewCache cache;

				PipelineFlags flags{ PassComponentCombine{}
					, scene.getDefaultLightingModel()
					, scene.getBackgroundModelId()
					, SubmeshFlag::ePosNmlTanTex
					, ProgramFlag::eNone
					, TextureCombine{}
					, ShaderFlag::eNone
					, VK_COMPARE_OP_ALWAYS };
				IntermediatesLister vis{ flags, scene, cache, intermediates };
				value.accept( vis );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, ImageViewCache & cache
				, IntermediateViewArray & result )
				: RenderTechniqueVisitor{ flags, scene, { false } }
				, m_handler{ scene.getEngine()->getGraphResourceHandler() }
				, m_result{ result }
				, m_cache{ cache }
			{
			}

			void doVisit3D( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto info = viewId.data->info;
				info.image = VkImage( uint64_t( viewId.data->image.id ) );

				if ( factors.isSlice )
				{
					m_cache.insert( { info, factors.slice } );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else
				{
					m_cache.insert( { info, 0u } );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
			}

			void doVisit2DArray( castor::String const & name
				, crg::ImageViewId viewId
				, VkImageLayout layout
				, TextureFactors const & factors )
			{
				auto info = viewId.data->info;
				info.image = VkImage( uint64_t( viewId.data->image.id ) );

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

						if ( doFilter( layerViewId, {} ) )
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

				if ( ( info.viewType == VK_IMAGE_VIEW_TYPE_2D )
					&& ( info.subresourceRange.layerCount == 1u ) )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					m_cache.insert( { info, 0u } );
					m_result.emplace_back( name
						, viewId
						, layout
						, factors );
				}
				else if ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					doVisit3D( name, viewId, layout, factors );
				}
				else
				{
					doVisit2DArray( name, viewId, layout, factors );
				}
			}

			bool doFilterArray( crg::ImageViewId const & viewId
				, TextureFactors const & factors )const
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
								, layerInfo.subresourceRange } )
							, factors ) && result;
						layerInfo.subresourceRange.baseArrayLayer++;
					}
				}

				return result;
			}

			bool doFilter( crg::ImageViewId const & viewId
				, TextureFactors const & factors )const override
			{
				auto info = viewId.data->info;

				if ( info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( { info, factors.slice } );
				}

				if ( info.subresourceRange.levelCount > 1u )
				{
					return false;
				}

				if ( info.viewType == VK_IMAGE_VIEW_TYPE_2D
					&& info.subresourceRange.layerCount == 1u )
				{
					info.image = VkImage( uint64_t( viewId.data->image.id ) );
					return m_cache.end() == m_cache.find( { info, 0u } );
				}

				return doFilterArray( viewId, factors );
			}

		private:
			crg::ResourceHandler & m_handler;
			IntermediateViewArray & m_result;
			ImageViewCache & m_cache;
		};

		static VkImageUsageFlags constexpr objectsUsageFlags = ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			| VK_IMAGE_USAGE_SAMPLED_BIT
			| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
			| VK_IMAGE_USAGE_TRANSFER_DST_BIT );
	}

	//*********************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine
		, TargetType type
		, castor::Size const & size
		, castor::PixelFormat pixelFormat )
		: OwnedBy< Engine >{ engine }
		, m_device{ getOwner()->getRenderSystem()->getRenderDevice() }
		, m_type{ type }
		, m_size{ size }
		, m_safeBandedSize{ getSafeBandedSize( size ) }
		, m_pixelFormat{ VkFormat( pixelFormat ) }
		, m_initialised{ false }
		, m_resources{ getOwner()->getGraphResourceHandler() }
		, m_renderTechnique{}
		, m_camera{}
		, m_index{ ++sm_uiCount }
		, m_name{ cuT( "Target" ) + castor::string::toString( m_index ) }
		, m_graph{ m_resources.getHandler(), m_name }
		, m_velocity{ m_device
			, m_resources
			, "Velocity"
			, 0u
			, makeExtent3D( m_safeBandedSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_STORAGE_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_srgbObjects{ Texture{ m_device
				, m_resources
				, "SRGBResult0"
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, getPixelFormat()
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
			, Texture{ m_device
				, m_resources
				, "SRGBResult1"
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, getPixelFormat()
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK } }
		, m_hdrObjects{ Texture{ m_device
				, m_resources
				, "HDRResult0"
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
			, Texture{ m_device
				, m_resources
				, "HDRResult1"
				, 0u
				, makeExtent3D( m_safeBandedSize )
				, 1u
				, 1u
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, rendtgt::objectsUsageFlags
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK } }
		, m_overlays{ m_device
			, m_resources
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
		, m_combined{ m_device
			, m_resources
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
		, m_overlayPassDesc{ doCreateOverlayPass( nullptr, engine.getRenderSystem()->getRenderDevice() ) }
	{
		m_graph.addInput( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addOutput( m_combined.wholeViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

		auto sampler = engine.addNewSampler( RenderTarget::DefaultSamplerName + getName() + cuT( "Linear" ), engine );
		sampler->setMinFilter( VK_FILTER_LINEAR );
		sampler->setMagFilter( VK_FILTER_LINEAR );

		sampler = engine.addNewSampler( RenderTarget::DefaultSamplerName + getName() + cuT( "Nearest" ), engine );
		sampler->setMinFilter( VK_FILTER_NEAREST );
		sampler->setMagFilter( VK_FILTER_NEAREST );

		for ( auto entry : engine.getPostEffectFactory().listRegisteredTypes() )
		{
			auto effect = engine.getPostEffectFactory().create( entry.key
				, *this
				, *engine.getRenderSystem()
				, Parameters{} );
			effect->enable( false );

			if ( effect->isAfterToneMapping() )
			{
				m_srgbPostEffects.push_back( std::move( effect ) );
			}
			else
			{
				m_hdrPostEffects.push_back( std::move( effect ) );
			}
		}

		{
			auto queueData = m_device.graphicsData();
			auto fence = m_device->createFence();
			auto commandBuffer = queueData->commandPool->createCommandBuffer();
			commandBuffer->begin();

			for ( auto & texture : m_hdrObjects )
			{
				texture.create();
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, texture.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			}

			for ( auto & texture : m_srgbObjects )
			{
				texture.create();
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, texture.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			}

			commandBuffer->end();
			queueData->queue->submit( *commandBuffer, *fence );
			fence->wait( ashes::MaxTimeout );
		}
	}

	RenderTarget::~RenderTarget()
	{
		for ( auto & texture : m_srgbObjects )
		{
			texture.destroy();
		}

		for ( auto & texture : m_hdrObjects )
		{
			texture.destroy();
		}
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
			doInitialise( device, queueData, progress );
		}

		while ( m_initialising )
		{
			std::this_thread::sleep_for( 1_ms );
		}
	}

	void RenderTarget::initialise( OnInitialisedFunc onInitialised
		, ProgressBar * progress )
	{
		if ( !m_initialising.exchange( true ) )
		{
			if ( !m_initialised )
			{
				getEngine()->pushCpuJob( [this, progress]()
					{
						auto &device = getEngine()->getRenderSystem()->getRenderDevice();
						auto queueWrapper = device.graphicsData();
						auto & queue = *queueWrapper;
						doInitialise( device, queue, progress );
						m_onInitialised( *this, queue );
						m_onTargetInitialised.clear();
					} );
			}
		}

		if ( m_initialising )
		{
			m_onTargetInitialised.push_back( m_onInitialised.connect( onInitialised ) );
		}

		if ( m_initialised )
		{
			auto & device = getEngine()->getRenderSystem()->getRenderDevice();
			auto queueWrapper = device.graphicsData();
			auto & queue = *queueWrapper;
			onInitialised( *this, queue );
		}
	}

	void RenderTarget::cleanup( RenderDevice const & device )
	{
		if ( m_initialised.exchange( false ) )
		{
			m_signalReady.reset();
			m_overlayPass = {};
#if C3D_DebugTimers
			getEngine()->unregisterTimer( cuT( "AAACPUTests" ), *m_cpuUpdateTimer );
			m_cpuUpdateTimer.reset();
			getEngine()->unregisterTimer( cuT( "AAAGPUTests" ), *m_gpuUpdateTimer );
			m_gpuUpdateTimer.reset();
#endif
			getEngine()->unregisterTimer( getName() + cuT( "/Overlays" ), *m_overlaysTimer );
			m_overlaysTimer.reset();
			m_intermediates.clear();
			getEngine()->unregisterTimer( m_runnable->getName() + "/Graph"
				, m_runnable->getTimer() );
			m_runnable.reset();
			m_debugDrawer.reset();
			m_combinePassSource = {};
			m_combinePass = {};

			for ( auto & effect : m_srgbPostEffects )
			{
				effect->cleanup( device );
			}

			m_toneMapping.reset();

			for ( auto & effect : m_hdrPostEffects )
			{
				effect->cleanup( device );
			}
		}

		m_overlays.destroy();
		m_velocity.destroy();
		m_combined.destroy();
		doCleanupTechnique();
		doCleanupCombineProgram();
		m_culler.reset();
		m_hdrConfigUbo.reset();
		m_frustumClusters.reset();
	}

	void RenderTarget::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_cpuUpdateTimer->start() );
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
		m_overlayPass->update( updater );

		if ( m_frustumClusters )
		{
			m_frustumClusters->update( updater );
		}

		m_hdrConfigUbo->cpuUpdate( getHdrConfig() );

		auto lastTarget = &doUpdatePostEffects( updater
			, m_hdrPostEffects
			, { &m_hdrObjects.front(), &m_hdrObjects.back() } );

		if ( m_toneMapping )
		{
			m_toneMapping->update( updater, lastTarget->sampledViewId );
		}
		else
		{
			m_hdrCopyPassIndex = ( lastTarget == m_hdrCopyPassSource ) ? 1u : 0u;
		}

		lastTarget = &doUpdatePostEffects( updater
			, m_srgbPostEffects
			, { &m_srgbObjects.front(), &m_srgbObjects.back() } );
		m_combinePassIndex = ( lastTarget == m_combinePassSource ) ? 1u : 0u;
	}

	void RenderTarget::update( GpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

#if C3D_DebugTimers
		auto block( m_gpuUpdateTimer->start() );
#endif

		auto & camera = *getCamera();
		auto & scene = *getScene();
		updater.jitter = m_jitter;
		updater.scene = &scene;
		updater.camera = &camera;

		m_renderTechnique->update( updater );
		m_overlayPass->update( updater );

		for ( auto & effect : m_hdrPostEffects )
		{
			effect->update( updater );
		}

		for ( auto & effect : m_srgbPostEffects )
		{
			effect->update( updater );
		}
	}

	void RenderTarget::upload( UploadData & uploader )
	{
		if ( m_initialised )
		{
			m_overlayPass->upload( uploader );
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

		if ( m_initialised
			&& scene
			&& scene->isInitialised()
			&& getCamera() )
		{
			result = doRender( queue, signalsToWait );

			if ( m_debugDrawer )
			{
				result = m_debugDrawer->render( queue, std::move( result ) );
			}
		}

		return result;
	}

	ViewportType RenderTarget::getViewportType()const
	{
		auto camera = getCamera();
		return ( camera ? camera->getViewportType() : ViewportType::eCount );
	}

	void RenderTarget::setViewportType( ViewportType value )
	{
		if ( auto camera = getCamera() )
		{
			camera->setViewportType( value );
		}
	}

	void RenderTarget::setCamera( Camera & camera )
	{
		auto myCamera = getCamera();

		if ( myCamera != &camera )
		{
			m_camera = &camera;
			m_camera->resize( m_size );

			if ( m_culler )
			{
				m_culler->resetCamera( getCamera() );
			}
			else
			{
				m_culler = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *getScene(), *getCamera() );
			}
		}
	}

	void RenderTarget::setScene( Scene & scene )
	{
		auto myScene = getScene();

		if ( myScene != &scene )
		{
			m_scene = &scene;
			m_graph.addInput( m_scene->getEnvironmentMap().getColourId().wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			m_culler.reset();
		}
	}

	void RenderTarget::setToneMappingType( castor::String const & name
		, Parameters const & parameters )
	{
		m_toneMappingName = name;

		if ( m_toneMapping )
		{
			getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					if ( m_initialised )
					{
						m_toneMapping->updatePipeline( m_toneMappingName );
					}
					else
					{
						m_toneMapping->initialise( m_toneMappingName
							, m_hdrObjects.back().sampledViewId );
					}
				} ) );
		}
	}

	PostEffectRPtr RenderTarget::getPostEffect( castor::String const & name )const
	{
		auto it = std::find_if( m_srgbPostEffects.begin()
			, m_srgbPostEffects.end()
			, [&name]( PostEffectUPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_srgbPostEffects.end() )
		{
			return it->get();
		}

		it = std::find_if( m_hdrPostEffects.begin()
			, m_hdrPostEffects.end()
			, [&name]( PostEffectUPtr const & lookup )
			{
				return lookup->getName() == name;
			} );

		if ( it != m_hdrPostEffects.end() )
		{
			return it->get();
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

	ShadowBuffer * RenderTarget::getShadowBuffer()const
	{
		if ( m_renderTechnique && m_renderTechnique->hasShadowBuffer() )
		{
			return &m_renderTechnique->getShadowBuffer();
		}

		return nullptr;
	}

	TechniquePassVector RenderTarget::getCustomRenderPasses()const
	{
		if ( m_renderTechnique )
		{
			return m_renderTechnique->getCustomRenderPasses();
		}

		return {};
	}

	CameraUbo const & RenderTarget::getCameraUbo()const
	{
		CU_Require( m_renderTechnique );
		return m_renderTechnique->getCameraUbo();
	}

	SceneUbo const & RenderTarget::getSceneUbo()const
	{
		CU_Require( m_renderTechnique );
		return m_renderTechnique->getSceneUbo();
	}

	bool RenderTarget::hasIndirect()const noexcept
	{
		return getScene()->needsGlobalIllumination()
			|| isFullLoadingEnabled();
	}

	bool RenderTarget::hasSss()const noexcept
	{
		return getScene()->needsSubsurfaceScattering()
			|| isFullLoadingEnabled();
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

	void RenderTarget::resetSemaphore()
	{
		m_signalFinished.clear();
	}

	crg::FramePass const & RenderTarget::createVertexTransformPass( crg::FramePassGroup & graph )
	{
		return getScene()->getRenderNodes().createVertexTransformPass( graph );
	}

	void RenderTarget::doInitialise( RenderDevice const & device
		, QueueData const & queueData
		, ProgressBar * progress )
	{
		m_hdrConfigUbo = std::make_unique< HdrConfigUbo >( device );
		m_culler = castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *getScene(), *getCamera() );
#if C3D_UseClusteredRendering
		m_frustumClusters = castor::makeUnique< FrustumClusters >( device, *getCamera() );
#endif

		doInitCombineProgram( progress );
		auto result = doInitialiseTechnique( device, queueData, progress );

		if ( !result )
		{
			log::error << "Couldn't initialise render technique, stopping here\n";
			return;
		}

		setProgressBarTitle( progress, "Initialising: Render Target" );
		auto * previousPass = &m_renderTechnique->getLastPass();
		auto hdrSource = &m_hdrObjects.front();
		auto hdrTarget = &m_hdrObjects.back();

		if ( !m_hdrPostEffects.empty() )
		{
			for ( auto & effect : m_hdrPostEffects )
			{
				if ( result
					&& ( isFullLoadingEnabled() || effect->isEnabled() ) )
				{
					stepProgressBar( progress, "Initialising post effect " + effect->getName() );
					result = effect->initialise( device
						, *hdrSource
						, *hdrTarget
						, *previousPass );
					std::swap( hdrSource, hdrTarget );
					previousPass = &effect->getPass();
				}
			}
		}

		if ( result )
		{
			m_hdrLastPass = previousPass;

			if ( castor::isFloatingPoint( getPixelFormat() ) )
			{
				previousPass = &doInitialiseCopyCommands( device
					, "HdrCopy"
					, crg::ImageViewIdArray{ hdrSource->sampledViewId, hdrTarget->sampledViewId }
					, m_srgbObjects.front().wholeViewId
					, *m_hdrLastPass
					, progress );
				m_hdrCopyPassSource = hdrSource;
			}
			else
			{
				stepProgressBar( progress, "Creating tone mapping pass" );
				m_toneMapping = castor::makeUnique< ToneMapping >( *getEngine()
					, device
					, m_size
					, m_graph
					, crg::ImageViewIdArray{ hdrSource->sampledViewId, hdrTarget->sampledViewId }
					, m_srgbObjects.front().wholeViewId
					, *m_hdrLastPass
					, *m_hdrConfigUbo
					, Parameters{}
					, progress );
				m_toneMapping->initialise( m_toneMappingName
					, m_hdrObjects.back().sampledViewId );
				previousPass = &m_toneMapping->getPass();
			}
		}

		auto srgbSource = &m_srgbObjects.front();
		auto srgbTarget = &m_srgbObjects.back();

		if ( !m_srgbPostEffects.empty() )
		{
			for ( auto & effect : m_srgbPostEffects )
			{
				if ( result
					&& ( isFullLoadingEnabled() || effect->isEnabled() ) )
				{
					stepProgressBar( progress, "Initialising post effect " + effect->getName() );
					result = effect->initialise( device
						, *srgbSource
						, *srgbTarget
						, *previousPass );
					std::swap( srgbSource, srgbTarget );
					previousPass = &effect->getPass();
				}
			}
		}

		if ( result )
		{
			m_combinePassSource = srgbSource;
			m_combinePass = &doCreateCombinePass( progress
				, crg::ImageViewIdArray{ srgbSource->sampledViewId, srgbTarget->sampledViewId } );
			m_combinePass->addDependency( *previousPass );

			stepProgressBar( progress, "Compiling render graph" );
			m_runnable = m_graph.compile( device.makeContext() );
			getEngine()->registerTimer( m_runnable->getName() + "/Graph"
				, m_runnable->getTimer() );
			printGraph( *m_runnable );
			doListIntermediateViews( m_intermediates );
			m_debugConfig.resetImages();

			for ( auto & intermediate : m_intermediates )
			{
				m_debugConfig.registerImage( intermediate.name );
			}

			m_overlays.create();
			m_velocity.create();
			m_combined.create();
			auto runnable = m_runnable.get();
			device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [runnable, result, this]( RenderDevice const &
					, QueueData const & )
				{
					runnable->record();
					m_initialised = result;
				} ) );
			m_debugDrawer = castor::makeUnique< DebugDrawer >(*this, device, m_combined, m_renderTechnique->getDepth() );
		}

		stepProgressBar( progress, "Creating overlay renderer" );
		m_overlaysTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), getName() + cuT( "/Overlays" ) );
		getEngine()->registerTimer( getName() + cuT( "/Overlays" ), *m_overlaysTimer );
#if C3D_DebugTimers
		m_cpuUpdateTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "AAACPUTests" ) );
		getEngine()->registerTimer( cuT( "AAACPUTests" ), *m_cpuUpdateTimer );
		m_gpuUpdateTimer = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "AAAGPUTests" ) );
		getEngine()->registerTimer( cuT( "AAAGPUTests" ), *m_gpuUpdateTimer );
#endif
		m_signalReady = device->createSemaphore( getName() + "Ready" );
		m_initialising = false;
	}

	crg::FramePass & RenderTarget::doCreateOverlayPass( ProgressBar * progress
		, RenderDevice const & device )
	{
		stepProgressBar( progress, "Creating overlays pass" );
		auto & group = m_graph.createPassGroup( "Overlays" );
		auto & result = group.createPass( "Overlays"
			, [this, progress, &device]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising overlays pass" );
				auto result = std::make_unique< OverlayPass >( pass
					, context
					, graph
					, device
					, *m_scene
					, makeExtent2D( m_overlays.getExtent() )
					, m_overlays
					, true );
				m_overlayPass = result.get();
				getOwner()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} );
		result.addOutputColourView( m_overlays.targetViewId );
		group.addGroupOutput( m_overlays.targetViewId );
		return result;
	}

	crg::FramePass & RenderTarget::doCreateCombinePass( ProgressBar * progress
		, crg::ImageViewIdArray source )
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
					.passIndex( &m_combinePassIndex )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_combineStages ) )
					.build( pass, context, graph, crg::ru::Config{ 2u } );
				getOwner()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} );
		result.addDependency( m_overlayPassDesc );
		result.addSampledView( source
			, rendtgt::CombineLhsIdx );
		result.addSampledView( m_overlays.sampledViewId
			, rendtgt::CombineRhsIdx );
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
					, m_hdrObjects.front()
					, m_hdrObjects.back()
					, m_ssaoConfig
					, progress
					, C3D_UseVisibilityBuffer != 0
					, C3D_UseWeightedBlendedRendering != 0 );
			}
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Couldn't load render technique: " ) << castor::string::stringCast< xchar >( exc.getFullDescription() ) << std::endl;
				throw;
			}
		}

		return true;
	}

	void RenderTarget::doCleanupTechnique()
	{
		m_renderTechnique.reset();
	}

	crg::FramePass const & RenderTarget::doInitialiseCopyCommands( RenderDevice const & device
		, castor::String const & name
		, crg::ImageViewIdArray const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
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
					, getSafeBandedExtent3D( m_size )
					, crg::ru::Config{ 2u}
					, crg::RunnablePass::GetPassIndexCallback( [this]() { return m_hdrCopyPassIndex; } ) );
				getOwner()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addTransferInputView( source );
		pass.addTransferOutputView( target );
		return pass;
	}

	void RenderTarget::doCleanupCopyCommands()
	{
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
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto lhsColor = writer.declLocale( "lhsColor"
						, c3d_mapLhs.sample( vtx_textureLhs ) );
					auto rhsColor = writer.declLocale( "rhsColor"
						, c3d_mapRhs.sample( vtx_textureRhs ) );
					lhsColor.rgb() *= 1.0_f - rhsColor.a();
					outColour = vec4( lhsColor.rgb() + rhsColor.rgb(), 1.0_f );
				} );
			m_combinePxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_combineStages.push_back( makeShaderState( renderSystem.getRenderDevice(), m_combineVtx ) );
		m_combineStages.push_back( makeShaderState( renderSystem.getRenderDevice(), m_combinePxl ) );
	}

	void RenderTarget::doCleanupCombineProgram()
	{
		m_combineStages.clear();
		m_combinePxl.shader.reset();
		m_combineVtx.shader.reset();
	}

	Texture const & RenderTarget::doUpdatePostEffects( CpuUpdater & updater
		, PostEffectArray const & effects
		, std::vector< Texture const * > const & images )const
	{
		Texture const * src = images.front();
		Texture const * dst = images.back();

		for ( auto & effect : effects )
		{
			if ( ( isFullLoadingEnabled() || effect->isEnabled() )
				&& effect->update( updater, *src ) )
			{
				std::swap( src, dst );
			}
		}

		return *dst;
	}

	crg::SemaphoreWaitArray RenderTarget::doRender( ashes::Queue const & queue
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

		// Compute all that is needed for the rendering of the scene.
		m_signalFinished = m_renderTechnique->preRender( signalsToWait
			, queue );

		// Then run the graph
		m_signalFinished = m_runnable->run( m_signalFinished
			, queue );

		return m_signalFinished;
	}

	void RenderTarget::doListIntermediateViews( IntermediateViewArray & result )const
	{
		result.emplace_back( "Target Result"
			, m_combined
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.emplace_back( "Target SRGB Colour"
			, m_srgbObjects.front()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( "Target HDR Colour"
			, m_hdrObjects.front()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		result.emplace_back( "Target Overlays"
			, m_overlays
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{ { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f } }.invert( true ) );
		result.emplace_back( "Target Velocity"
			, m_velocity
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			, TextureFactors{}.invert( true ) );

		for ( auto & postEffect : m_hdrPostEffects )
		{
			if ( isFullLoadingEnabled() || postEffect->isEnabled() )
			{
				rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
			}
		}

		for ( auto & postEffect : m_srgbPostEffects )
		{
			if (isFullLoadingEnabled() || postEffect->isEnabled())
			{
				rendtgt::IntermediatesLister::submit( *getScene(), *postEffect, result );
			}
		}

		if ( m_renderTechnique )
		{
			rendtgt::IntermediatesLister::submit( *getScene(), *m_renderTechnique, result );
		}

		rendtgt::IntermediatesLister::submit( *getScene(), *getScene()->getBackground(), result );
	}
}
