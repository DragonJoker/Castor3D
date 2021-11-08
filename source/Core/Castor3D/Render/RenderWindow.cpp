#include "Castor3D/Render/RenderWindow.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Picking.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Core/SwapChainCreateInfo.hpp>
#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Design/BlockGuard.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderWindow )

#define C3D_PersistLoadingScreen 1

namespace castor3d
{
	namespace
	{
		QueuesData const * getQueueFamily( ashes::Surface const & surface
			, QueueFamilies const & queues )
		{
			auto it = std::find_if( queues.begin()
				, queues.end()
				, [&surface]( QueuesData const & lookup )
				{
					return surface.getSupport( lookup.familyIndex );
				} );

			if ( it == queues.end() )
			{
				throw ashes::Exception{ VK_ERROR_INITIALIZATION_FAILED
					, "Couldn't find a queue supporting presentation." };
			}

			return &( *it );
		}

		uint32_t getImageCount( ashes::Surface const & surface )
		{
			auto surfaceCaps = surface.getCapabilities();
			uint32_t desiredNumberOfSwapChainImages{ surfaceCaps.minImageCount + 1 };

			if ( ( surfaceCaps.maxImageCount > 0 ) &&
				( desiredNumberOfSwapChainImages > surfaceCaps.maxImageCount ) )
			{
				desiredNumberOfSwapChainImages = surfaceCaps.maxImageCount;
			}

			return desiredNumberOfSwapChainImages;
		}

		VkSurfaceFormatKHR selectFormat( ashes::Surface const & surface )
		{
			VkSurfaceFormatKHR result;
			auto formats = surface.getFormats();

			if ( formats.size() == 1u && formats[0].format == VK_FORMAT_UNDEFINED )
			{
				result.format = VK_FORMAT_R8G8B8A8_UNORM;
				result.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			}
			else
			{
				assert( !formats.empty() );
				auto it = std::find_if( formats.begin()
					, formats.end()
					, []( VkSurfaceFormatKHR const & lookup )
					{
						return lookup.format == VK_FORMAT_R8G8B8A8_UNORM;
					} );

				if ( it != formats.end() )
				{
					result = *it;
				}
				else
				{
					result = formats.front();
				}
			}

			return result;
		}

		VkPresentModeKHR selectPresentMode( ashes::Surface const & surface )
		{
			auto presentModes = surface.getPresentModes();
			VkPresentModeKHR result{ VK_PRESENT_MODE_FIFO_KHR };

			for ( auto mode : presentModes )
			{
				if ( mode == VK_PRESENT_MODE_MAILBOX_KHR )
				{
					result = mode;
					break;
				}

				if ( ( result != VK_PRESENT_MODE_MAILBOX_KHR )
					&& ( mode == VK_PRESENT_MODE_IMMEDIATE_KHR ) )
				{
					result = mode;
				}
			}

			return result;
		}

		ashes::SwapChainCreateInfo getSwapChainCreateInfo( ashes::Surface const & surface
			, VkExtent2D const & size )
		{
			VkExtent2D swapChainExtent{};
			auto surfaceCaps = surface.getCapabilities();

			if ( surfaceCaps.currentExtent.width == uint32_t( -1 ) )
			{
				swapChainExtent = size;
			}
			else
			{
				swapChainExtent = surfaceCaps.currentExtent;
			}

			VkSurfaceTransformFlagBitsKHR preTransform{};

			if ( ashes::checkFlag( surfaceCaps.supportedTransforms, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) )
			{
				preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			}
			else
			{
				preTransform = surfaceCaps.currentTransform;
			}

			auto presentMode = selectPresentMode( surface );
			auto surfaceFormat = selectFormat( surface );
			return ashes::SwapChainCreateInfo
			{
				0u,
				surface,
				getImageCount( surface ),
				surfaceFormat.format,
				surfaceFormat.colorSpace,
				swapChainExtent,
				1u,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				{},
				preTransform,
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				presentMode,
				VK_TRUE,
				nullptr
			};
		}

		IntermediateView doCreateBarrierView( RenderDevice const & device
			, IntermediateView const & view )
		{
			auto & handler = device.renderSystem.getEngine()->getGraphResourceHandler();
			auto imageId = view.viewId.data->image;
			auto info = view.viewId.data->info;
			crg::ImageViewId viewId{ handler.createViewId( crg::ImageViewData{ view.name + "Barrier"
				, imageId
				, info.flags
				, info.viewType
				, info.format
				, { VkImageAspectFlags( ashes::isDepthStencilFormat( info.format )
						? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
						: ( ashes::isDepthFormat( info.format )
							? VK_IMAGE_ASPECT_DEPTH_BIT
							: ( ashes::isStencilFormat( info.format )
								? VK_IMAGE_ASPECT_STENCIL_BIT
								: VK_IMAGE_ASPECT_COLOR_BIT ) ) )
					, info.subresourceRange.baseMipLevel
					, info.subresourceRange.levelCount
					, info.subresourceRange.baseArrayLayer
					, info.subresourceRange.layerCount } } ) };
			return { view.name
				, viewId
				, view.layout
				, view.factors };
		}

		IntermediateViewArray doCreateBarrierViews( RenderDevice const & device
			, IntermediateView const & tex3DResult
			, IntermediateViewArray const & views )
		{
			using castor3d::operator!=;
			IntermediateViewArray result;

			if ( views.size() == 1u )
			{
				auto & view = views[0u];
				result.push_back( doCreateBarrierView( device, view ) );
				return result;
			}

			for ( auto & view : views )
			{
				if ( view.viewId.data->info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					result.push_back( doCreateBarrierView( device, tex3DResult ) );
				}
				else
				{
					result.push_back( doCreateBarrierView( device, view ) );
				}
			}

			return result;
		}

		IntermediateView doCreateSampledView( RenderDevice const & device
			, IntermediateView const & view )
		{
			auto & handler = device.renderSystem.getEngine()->getGraphResourceHandler();
			auto imageId = view.viewId.data->image;
			auto info = view.viewId.data->info;
			crg::ImageViewId viewId{ handler.createViewId( crg::ImageViewData{ view.name + "Sampled"
				, imageId
				, info.flags
				, info.viewType
				, info.format
				, { VkImageAspectFlags( ashes::isDepthFormat( info.format )
						? VK_IMAGE_ASPECT_DEPTH_BIT
						: ( ashes::isStencilFormat( info.format )
							? VK_IMAGE_ASPECT_STENCIL_BIT
							: ( ashes::isDepthOrStencilFormat( info.format )
								? VK_IMAGE_ASPECT_DEPTH_BIT
								: VK_IMAGE_ASPECT_COLOR_BIT ) ) )
					, info.subresourceRange.baseMipLevel
					, info.subresourceRange.levelCount
					, info.subresourceRange.baseArrayLayer
					, info.subresourceRange.layerCount } } ) };
			return { view.name
				, viewId
				, view.layout
				, view.factors };
		}

		IntermediateViewArray doCreateSampledViews( RenderDevice const & device
			, IntermediateView const & tex3DResult
			, IntermediateViewArray const & views )
		{
			using castor3d::operator!=;
			IntermediateViewArray result;

			if ( views.size() == 1u )
			{
				auto & view = views[0u];
				result.push_back( doCreateSampledView( device, view ) );
				return result;
			}

			for ( auto & view : views )
			{
				if ( view.viewId.data->info.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					result.push_back( doCreateBarrierView( device, tex3DResult ) );
				}
				else
				{
					result.push_back( doCreateSampledView( device, view ) );
				}
			}

			return result;
		}

		OverlayResPtr getOverlay( Scene const & scene, castor::String const & name )
		{
			return scene.getOverlayView().tryFind( name );
		}

		TextOverlaySPtr getTextOverlay( Scene const & scene, castor::String const & name )
		{
			TextOverlaySPtr result;
			auto o = getOverlay( scene, name ).lock();

			if ( o && o->getType() == OverlayType::eText )
			{
				result = o->getTextOverlay();
			}

			return result;
		}

		castor::Size getScreenSize()
		{
			castor::Size result;
			castor::System::getScreenSize( 0u, result );
			return result;
		}
	}

	//*************************************************************************************************

	void RenderWindow::EvtHandler::doProcessMouseEvent( MouseEventSPtr event )
	{
		m_window->m_mousePosition = event->getPosition();
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( castor::String const & name
		, Engine & engine
		, castor::Size const & size
		, ashes::WindowHandle handle )
		: OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_evtHandler{ std::make_shared< EvtHandler >( *this ) }
		, m_index{ s_nbRenderWindows++ }
		, m_device{ engine.getRenderSystem()->getRenderDevice() }
		, m_surface{ m_device.renderSystem.getInstance().createSurface( m_device.renderSystem.getPhysicalDevice()
			, std::move( handle ) ) }
		, m_queues{ getQueueFamily( *m_surface, m_device.queueFamilies ) }
		, m_queue{ m_queues->reserveQueue() }
		, m_listener{ getEngine()->getFrameListenerCache().add( getName() + castor::string::toString( m_index ) ) }
		, m_size{ size }
		, m_loading{ engine.isThreaded() }
		, m_configUbo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
		log::debug << "Created RenderWindow, size: " << size << std::endl;

		if ( !m_surface )
		{
			CU_Exception( "Could not create Vulkan surface." );
		}

		getEngine()->getMaterialCache().initialise( m_device
			, getEngine()->getPassesType() );
		doCreateProgram();
		doCreateSwapchain();

		if ( engine.isThreaded() )
		{
#if C3D_PersistLoadingScreen
			doCreateLoadingScreen();
#endif
			getEngine()->registerWindow( *this );
		}

		log::debug << "Created render window " << m_index << std::endl;
	}

	RenderWindow::~RenderWindow()
	{
		log::debug << "Destroyed render window " << m_index << std::endl;
		auto & engine = *getEngine();
		auto listener = getListener();
		engine.getFrameListenerCache().remove( getName() + castor::string::toString( m_index ) );

#if C3D_PersistLoadingScreen
		if ( engine.isThreaded() )
		{
			doDestroyLoadingScreen();
		}
#endif

		doDestroySwapchain();
		doDestroyProgram();
		m_device.uboPools->putBuffer( m_configUbo );
	}

	void RenderWindow::initialise( RenderTargetSPtr target )
	{
		m_renderTarget = target;

		if ( !target )
		{
			CU_Exception( "No render target for render window." );
		}

		if ( m_loadingScreen )
		{
			if ( !m_loading.exchange( true ) )
			{
				m_loadingScreen->enable();
			}

			auto progress = m_progressBar.get();
			incProgressBarRange( progress, 6u + target->countInitialisationSteps() );
			getEngine()->pushGpuJob( [this]( RenderDevice const & device, QueueData const & queueData )
				{
					auto target = m_renderTarget.lock();

					if ( target )
					{
						auto progress = m_progressBar.get();
						target->initialise( device, queueData, progress );
						setProgressBarTitle( progress, "Render Window" );
						stepProgressBar( progress, "Initialising picking" );
						doCreatePickingPass( queueData );
						stepProgressBar( progress, "Initialising intermediate views" );
						doCreateIntermediateViews( queueData );
						stepProgressBar( progress, "Initialising combine quad" );
						doCreateRenderQuad( queueData );
						stepProgressBar( progress, "Initialising command buffers" );
						doCreateCommandBuffers( queueData );
						stepProgressBar( progress, "Initialising save data" );
						doCreateSaveData( queueData );
						stepProgressBar( progress, "Finalising..." );
					}

					getListener()->postEvent( makeCpuFunctorEvent( EventType::ePostRender
						, [this]()
						{
							if ( m_loadingScreen )
							{
								m_loadingScreen->disable();
							}

							m_loading = false;
							m_initialised = true;
						} ) );
				} );
		}
		else
		{
			auto queueData = m_device.graphicsData();
			target->initialise( m_device, *queueData, nullptr );
			doCreatePickingPass( *queueData );
			doCreateIntermediateViews( *queueData );
			doCreateRenderQuad( *queueData );
			doCreateCommandBuffers( *queueData );
			doCreateSaveData( *queueData );

			if ( m_loadingScreen )
			{
				m_loadingScreen->disable();
			}

			m_loading = false;
			m_initialised = true;
			getEngine()->registerWindow( *this );
		}
	}

	void RenderWindow::cleanup()
	{
		auto & engine = *getEngine();
		engine.unregisterWindow( *this );

		doWaitFrame( {} );
		getDevice()->waitIdle();

		doDestroySaveData();

		if ( engine.isThreaded() )
		{
			doDestroyLoadingScreen();
		}

		doDestroyCommandBuffers();
		doDestroyRenderQuad();
		doDestroyIntermediateViews();
		doDestroyPickingPass();

		if ( auto target = getRenderTarget() )
		{
			target->cleanup( m_device );
		}
	}

	void RenderWindow::update( CpuUpdater & updater )
	{
		if ( m_skip )
		{
			return;
		}

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->update( updater );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->update( updater );
#if C3D_DebugQuads

			if ( m_initialised )
			{
				updater.combineIndex = m_debugConfig.debugIndex;
				auto & intermediate = m_intermediates[m_debugConfig.debugIndex];

				if ( intermediate.factors.grid )
				{
					updater.cellSize = ( *intermediate.factors.grid )->w;
					updater.gridCenter = castor::Point3f{ *intermediate.factors.grid };
				}
				else
				{
					updater.cellSize = 0.0f;
					updater.gridCenter = {};
				}

				m_texture3Dto2D->update( updater );
				auto & config = m_configUbo.getData();
				config.multiply = castor::Point4f{ intermediate.factors.multiply };
				config.add = castor::Point4f{ intermediate.factors.add };
			}

#endif
		}
	}

	void RenderWindow::update( GpuUpdater & updater )
	{
		if ( m_skip )
		{
			return;
		}

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->update( updater );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->update( updater );
		}
	}

	void RenderWindow::render( RenderInfo & info
		, bool waitOnly
		, crg::SemaphoreWaitArray & baseToWait )
	{
		if ( m_skip )
		{
			return;
		}

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			if ( auto resources = doGetResources() )
			{
				crg::Fence * fence{};
				auto toWait = doSubmitLoadingFrame( *resources
					, *m_loadingScreen
					, fence
					, std::move( baseToWait ) );
				doPresentLoadingFrame( fence
					, *resources
					, std::move( toWait ) );
			}
		}
		else if ( m_initialised )
		{
			RenderTargetSPtr target = getRenderTarget();

			if ( target && target->isInitialised() )
			{
				auto toWait = { target->render( m_device
					, info
					, *m_queue->queue
					, baseToWait ) };
				baseToWait.clear();

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
				m_pickingPass->pick( *m_device
					, m_mousePosition
					, *target->getCamera() );
#endif

				if ( waitOnly )
				{
					doWaitFrame( toWait );
				}
				else if ( auto resources = doGetResources() )
				{
					try
					{
						doSubmitFrame( resources, toWait );
						doPresentFrame( resources );
					}
					catch ( ashes::Exception & exc )
					{
						std::cerr << "Can't render: " << exc.what() << std::endl;

						if ( exc.getResult() == VK_ERROR_DEVICE_LOST )
						{
							m_skip = true;
						}

						throw;
					}
				}
				else
				{
					std::cerr << "Can't render" << std::endl;
				}
			}
		}
	}

	void RenderWindow::resize( uint32_t x, uint32_t y )
	{
		resize( { x, y } );
	}

	void RenderWindow::resize( castor::Size const & size )
	{
		m_size = size;

		if ( !m_skip.exchange( true ) )
		{
			log::debug << "Resizing RenderWindow to " << size << std::endl;
			getListener()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					doResetSwapChain();
					m_skip = false;
				} ) );
		}
	}

	void RenderWindow::setCamera( CameraSPtr camera )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setCamera( camera );
		}
	}

	void RenderWindow::enableFullScreen( bool value )
	{
		m_fullscreen = value;
	}

	SceneSPtr RenderWindow::getScene()const
	{
		SceneSPtr result;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getScene();
		}

		return result;
	}

	CameraSPtr RenderWindow::getCamera()const
	{
		CameraSPtr result;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getCamera();
		}

		return result;
	}

	ViewportType RenderWindow::getViewportType()const
	{
		ViewportType result{};
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getViewportType();
		}

		return result;
	}

	void RenderWindow::setViewportType( ViewportType value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setViewportType( value );
		}
	}

	VkFormat RenderWindow::getPixelFormat()const
	{
		VkFormat result = VK_FORMAT_UNDEFINED;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getPixelFormat();
		}

		return result;
	}

	void RenderWindow::setScene( SceneSPtr value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setScene( value );
		}
	}

	bool RenderWindow::isUsingStereo()const
	{
		bool result = false;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_result = target->isUsingStereo();
		}

		return result;
	}

	void RenderWindow::setStereo( bool value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_target->setStereo( value );
		}
	}

	float RenderWindow::getIntraOcularDistance()const
	{
		float result = 0;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_result = target->getIntraOcularDistance();
		}

		return result;
	}

	void RenderWindow::setIntraOcularDistance( float value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_target->setIntraOcularDistance( value );
		}
	}

	castor::Size RenderWindow::getSize()const
	{
		return m_size;
	}

	void RenderWindow::addPickingScene( Scene & scene )
	{
		auto camera = getCamera();

		if ( camera )
		{
			m_picking->addScene( scene, *camera );
		}
	}

	PickNodeType RenderWindow::pick( castor::Position const & position )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		return m_pickingPass->getPickedNodeType();

#else
		PickNodeType result = PickNodeType::eNone;
		auto camera = getCamera();

		if ( camera && !m_picking->isPicking() )
		{
			result = m_picking->pick( m_device
				, position
				, *camera );
		}

		return result;

#endif
	}

	IntermediateViewArray const & RenderWindow::listIntermediateViews()const
	{
		return m_intermediates;
	}

	GeometrySPtr RenderWindow::getPickedGeometry()const
	{
		return m_picking->getPickedGeometry();
	}

	BillboardBaseSPtr RenderWindow::getPickedBillboard()const
	{
		return m_picking->getPickedBillboard();
	}

	SubmeshSPtr RenderWindow::getPickedSubmesh()const
	{
		return m_picking->getPickedSubmesh();
	}

	uint32_t RenderWindow::getPickedFace()const
	{
		return m_picking->getPickedFace();
	}

	ShadowMapLightTypeArray RenderWindow::getShadowMaps()const
	{
		auto target = getRenderTarget();

		if ( target )
		{
			return target->getShadowMaps();
		}

		return {};
	}

	void RenderWindow::enableLoading()
	{
		m_loading = getEngine()->isThreaded();
	}

	void RenderWindow::doCreateRenderPass()
	{
		ashes::VkAttachmentDescriptionArray attaches{ { 0u
			, m_swapChain->getFormat()
			, VK_SAMPLE_COUNT_1_BIT
			, VK_ATTACHMENT_LOAD_OP_CLEAR
			, VK_ATTACHMENT_STORE_OP_STORE
			, VK_ATTACHMENT_LOAD_OP_DONT_CARE
			, VK_ATTACHMENT_STORE_OP_DONT_CARE
			, VK_IMAGE_LAYOUT_UNDEFINED
			, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR } };
		ashes::SubpassDescriptionArray subpasses;
		subpasses.push_back( { 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
			, {}
			, ashes::nullopt
			, {} } );
		ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_ACCESS_MEMORY_READ_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
				, VK_ACCESS_MEMORY_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo createInfo{ 0u
			, std::move( attaches )
			, std::move( subpasses )
			, std::move( dependencies ) };
		m_renderPass = getDevice()->createRenderPass( "RenderPass"
			, std::move( createInfo ) );
	}

	void RenderWindow::doDestroyRenderPass()
	{
		m_renderPass.reset();
	}

	void RenderWindow::doCreateProgram()
	{
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, getName() };
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, getName() };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto c3d_mapResult = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapResult", 0u, 0u );
			auto c3d_config = writer.declUniformBuffer( "c3d_config", 1u, 0u );
			auto c3d_multiply = c3d_config.declMember< sdw::Vec4 >( "c3d_multiply" );
			auto c3d_add = c3d_config.declMember< sdw::Vec4 >( "c3d_add" );
			c3d_config.end();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = vec4( fma( c3d_mapResult.sample( vtx_texture ).xyz(), c3d_multiply.xyz(), c3d_add.xyz() )
						, 1.0_f );
				} );
			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_program = ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( getDevice(), vtx ),
			makeShaderState( getDevice(), pxl ),
		};
	}

	void RenderWindow::doDestroyProgram()
	{
		m_program.clear();
	}

	void RenderWindow::doCreateSwapchain()
	{
		m_swapChain = getDevice()->createSwapChain( getSwapChainCreateInfo( *m_surface
			, { m_size.getWidth(), m_size.getHeight() } ) );
		m_swapChainImages = m_swapChain->getImages();
		doCreateRenderPass();
		doCreateRenderingResources();
		doCreateFrameBuffers();

#if !C3D_PersistLoadingScreen
		if ( getEngine()->isThreaded() )
		{
			doCreateLoadingScreen();
		}
#endif
	}

	void RenderWindow::doDestroySwapchain()
	{
#if !C3D_PersistLoadingScreen
		if ( getEngine()->isThreaded() )
		{
			doDestroyLoadingScreen();
		}
#endif

		doDestroyFrameBuffers();
		doDestroyRenderingResources();
		doDestroyRenderPass();
		m_swapChainImages.clear();
		m_swapChain.reset();
	}

	void RenderWindow::doCreateRenderingResources()
	{
		for ( uint32_t i = 0u; i < uint32_t( m_swapChainImages.size() ); ++i )
		{
			m_renderingResources.emplace_back( std::make_unique< RenderingResources >( getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "ImageAvailable" )
				, getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "FinishedRendering" )
				, getDevice()->createFence( getName() + castor::string::toString( i ), VkFenceCreateFlags{ 0u } )
				, m_queue->commandPool->createCommandBuffer( getName() + castor::string::toString( i ) )
				, 0u ) );
		}
	}

	void RenderWindow::doDestroyRenderingResources()
	{
		m_renderingResources.clear();
	}

	void RenderWindow::doCreateFrameBuffers()
	{
		auto prepareAttaches = [this]( uint32_t backBuffer )
		{
			ashes::ImageViewCRefArray attaches;
			m_swapchainViews.clear();

			for ( size_t i = 0u; i < m_renderPass->getAttachments().size(); ++i )
			{
				m_swapchainViews.push_back( m_swapChainImages[backBuffer].createView( makeVkType< VkImageViewCreateInfo >( 0u
					, m_swapChainImages[backBuffer]
					, VK_IMAGE_VIEW_TYPE_2D
					, m_swapChain->getFormat()
					, VkComponentMapping{}
					, VkImageSubresourceRange
					{  ashes::getAspectMask( m_swapChain->getFormat() )
						, 0u
						, 1u
						, 0u
						, 1u } ) ) );
				attaches.emplace_back( m_swapchainViews.back() );
			}

			return attaches;
		};

		m_frameBuffers.resize( m_swapChainImages.size() );

		for ( size_t i = 0u; i < m_frameBuffers.size(); ++i )
		{
			auto attaches = prepareAttaches( uint32_t( i ) );
			m_frameBuffers[i] = m_renderPass->createFrameBuffer( "RenderPass"
				, m_swapChain->getDimensions()
				, std::move( attaches ) );
		}
	}

	void RenderWindow::doDestroyFrameBuffers()
	{
		m_frameBuffers.clear();
	}

	void RenderWindow::doCreateLoadingScreen()
	{
		auto scene = getEngine()->getLoadingScene();

		if ( !scene )
		{
			return;
		}

		if ( !m_progressBar )
		{
			m_progressBar = castor::makeUnique< ProgressBar >( *getEngine()
				, getOverlay( *scene, cuT( "Progress" ) )
				, getOverlay( *scene, cuT( "ProgressBar" ) )
				, getTextOverlay( *scene, cuT( "ProgressTitle" ) )
				, getTextOverlay( *scene, cuT( "ProgressLabel" ) )
				, 1u );
			m_progressBar->setTitle( "Initialising..." );
			m_progressBar->setLabel( "" );
		}

		m_loadingScreen = castor::makeUnique< LoadingScreen >( *m_progressBar
			, m_device
			, getEngine()->getGraphResourceHandler()
			, scene
			, *m_renderPass
#if C3D_PersistLoadingScreen
			, getScreenSize() );
#else
			, m_size );
#endif

		if ( m_loading && m_loadingScreen )
		{
			m_loadingScreen->enable();
		}
	}

	void RenderWindow::doDestroyLoadingScreen()
	{
		if ( m_loading && m_loadingScreen )
		{
			m_loadingScreen->disable();
		}

		m_loadingScreen.reset();
	}

	void RenderWindow::doCreatePickingPass( QueueData const & queueData )
	{
		auto target = getRenderTarget();

		if ( target )
		{
			m_picking = std::make_shared< Picking >( getOwner()->getGraphResourceHandler()
				, m_device
				, queueData
				, target->getSize()
				, target->getTechnique().getMatrixUbo()
				, target->getCuller() );
		}
	}

	void RenderWindow::doDestroyPickingPass()
	{
		m_picking.reset();
	}

	void RenderWindow::doCreateRenderQuad( QueueData const & queueData )
	{
		auto target = getRenderTarget();

		if ( target )
		{
			m_renderQuad = RenderQuadBuilder{}
				.binding( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D )
				.binding( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER )
				.texcoordConfig( rq::Texcoord{} )
				.tex3DResult( m_tex3DTo2DIntermediate )
				.build( m_device
					, getName()
					, VK_FILTER_LINEAR );
			m_renderQuad->createPipeline( VkExtent2D{ m_size[0], m_size[1] }
				, castor::Position{}
				, m_program
				, *m_renderPass );
			auto & handler = m_device.renderSystem.getEngine()->getGraphResourceHandler();
			auto & context = m_device.makeContext();

			for ( auto & intermediate : m_intermediateSampledViews )
			{
				m_renderQuad->registerPassInputs( { RenderQuad::makeDescriptorWrite( handler.createImageView( context, intermediate.viewId ), m_renderQuad->getSampler().getSampler(), 0u )
						, RenderQuad::makeDescriptorWrite( m_configUbo, 1u ) }
					, intermediate.factors.invertY );
			}

			m_renderQuad->initialisePasses();
		}
	}

	void RenderWindow::doDestroyRenderQuad()
	{
		m_renderQuad.reset();
	}

	void RenderWindow::doCreateCommandBuffers( QueueData const & queueData )
	{
		if ( !getRenderTarget() )
		{
			return;
		}

		auto pass = 0u;
		m_commandBuffers.resize( m_intermediates.size() );
		auto & handler = m_device.renderSystem.getEngine()->getGraphResourceHandler();
		auto & context = m_device.makeContext();

		for ( auto & commandBuffers : m_commandBuffers )
		{
			commandBuffers.resize( m_swapChainImages.size() );
			auto & intermediate = m_intermediates[pass];
			auto & intermediateBarrierView = m_intermediateBarrierViews[pass];
			uint32_t index = 0u;

			for ( auto & commandBuffer : commandBuffers )
			{
				auto & frameBuffer = *m_frameBuffers[index];
				auto name = getName() + castor::string::toString( index );
				commandBuffer = queueData.commandPool->createCommandBuffer( name );
				commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
				commandBuffer->beginDebugBlock( { "RenderWindow " + name
					, makeFloatArray( getEngine()->getNextRainbowColour() ) } );

				if ( intermediate.layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
				{
					commandBuffer->memoryBarrier( ashes::getStageMask( intermediateBarrierView.layout )
						, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, makeLayoutTransition( handler.createImage( context, intermediateBarrierView.viewId.data->image )
							, intermediateBarrierView.viewId.data->info.subresourceRange
							, intermediateBarrierView.layout
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, VK_QUEUE_FAMILY_IGNORED
							, VK_QUEUE_FAMILY_IGNORED ) );
				}

				commandBuffer->beginRenderPass( *m_renderPass
					, frameBuffer
					, { opaqueWhiteClearColor }
					, VK_SUBPASS_CONTENTS_INLINE );
				m_renderQuad->registerPass( *commandBuffer, pass );
				commandBuffer->endRenderPass();

				if ( intermediate.layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					&& intermediate.layout != VK_IMAGE_LAYOUT_UNDEFINED )
				{
					commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, ashes::getStageMask( intermediateBarrierView.layout )
						, makeLayoutTransition( handler.createImage( context, intermediateBarrierView.viewId.data->image )
							, intermediateBarrierView.viewId.data->info.subresourceRange
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, intermediateBarrierView.layout
							, VK_QUEUE_FAMILY_IGNORED
							, VK_QUEUE_FAMILY_IGNORED ) );
				}

				commandBuffer->endDebugBlock();
				commandBuffer->end();
				index++;
			}

			++pass;
		}
	}

	void RenderWindow::doDestroyCommandBuffers()
	{
		for ( auto & commandBuffers : m_commandBuffers )
		{
			commandBuffers.clear();
		}

		m_commandBuffers.clear();
	}

	void RenderWindow::doCreateIntermediateViews( QueueData const & queueData )
	{
		auto target = m_renderTarget.lock();

		if ( !target )
		{
			return;
		}

		target->listIntermediateViews( m_intermediates );

		VkExtent2D extent{ m_size.getWidth(), m_size.getHeight() };
		m_texture3Dto2D = castor::makeUnique< Texture3DTo2D >( m_device
			, extent
			, target->getTechnique().getMatrixUbo() );
		m_tex3DTo2DIntermediate = { "Texture3DTo2DResult"
			, m_texture3Dto2D->getTarget().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) };
#if C3D_DebugQuads
		auto intermediates = m_intermediates;
#else
		IntermediateViewArray intermediates{ m_intermediates[0] };
#endif
		m_texture3Dto2D->createPasses( queueData, intermediates );

		m_intermediateBarrierViews = doCreateBarrierViews( m_device
			, m_tex3DTo2DIntermediate
			, m_intermediates );
		m_intermediateSampledViews = doCreateSampledViews( m_device
			, m_tex3DTo2DIntermediate
			, m_intermediates );
	}

	void RenderWindow::doDestroyIntermediateViews()
	{
		m_texture3Dto2D.reset();
		m_intermediateSampledViews.clear();
		m_intermediateBarrierViews.clear();
		m_intermediates.clear();
	}

	void RenderWindow::doCreateSaveData( QueueData const & queueData )
	{
		auto target = getRenderTarget();
		m_saveBuffer = castor::PxBufferBase::create( target->getSize(), convert( target->getPixelFormat() ) );
		auto targetExtent = makeExtent2D( m_saveBuffer->getDimensions() );
		auto bufferSize = ashes::getAlignedSize( ashes::getLevelsSize( targetExtent
			, VK_FORMAT_R32G32B32A32_SFLOAT // Reserve enough room to hold max image size
			, 0u
			, 1u
			, 1u )
			, getDevice().renderSystem.getValue( GpuMin::eBufferMapSize ) );
		m_stagingBuffer = getDevice()->createBuffer( "Snapshot"
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT );
		auto requirements = m_stagingBuffer->getMemoryRequirements();
		auto deduced = getDevice()->deduceMemoryType( requirements.memoryTypeBits
			, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
		m_stagingBuffer->bindMemory( getDevice()->allocateMemory( "Snapshot"
			, {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				nullptr,
				requirements.size,
				deduced
			} ) );
		m_stagingData = castor::makeArrayView( m_stagingBuffer->lock( 0u, bufferSize, 0u )
			, bufferSize );
		auto pass = 0u;
		m_transferCommands.resize( m_intermediates.size() );
		auto & handler = m_device.renderSystem.getEngine()->getGraphResourceHandler();
		auto & context = m_device.makeContext();

		for ( auto & transferCommands : m_transferCommands )
		{
			transferCommands = { getDevice(), queueData, "Snapshot" };
			auto & intermediate = m_intermediates[pass];
			auto & intermediateBarrierView = m_intermediateBarrierViews[pass];
			auto & intermediateSampledView = m_intermediateSampledViews[pass];
			auto & commands = *transferCommands.commandBuffer;
			commands.begin();
			commands.beginDebugBlock( { "Staging Texture Download"
				, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
			VkImage srcImage = handler.createImage( context, intermediateBarrierView.viewId.data->image );

			if ( intermediate.layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
			{
				commands.memoryBarrier( ashes::getStageMask( intermediateBarrierView.layout )
					, VK_PIPELINE_STAGE_TRANSFER_BIT
					, makeLayoutTransition( srcImage
						, intermediateBarrierView.viewId.data->info.subresourceRange
						, intermediateBarrierView.layout
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			commands.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_stagingBuffer->makeTransferDestination() );
			auto srcExtent = getExtent( intermediateBarrierView.viewId );
			auto dstExtent = targetExtent;
			VkOffset3D srcOffset{};

			if ( srcExtent.width > dstExtent.width )
			{
				srcOffset.x = int32_t( srcExtent.width - dstExtent.width ) / 2;
			}

			if ( srcExtent.height > dstExtent.height )
			{
				srcOffset.y = int32_t( srcExtent.height - dstExtent.height ) / 2;
			}

			dstExtent.width = std::min( dstExtent.width, srcExtent.width );
			dstExtent.height = std::min( dstExtent.height, srcExtent.height );

			auto subresourceRange = intermediateSampledView.viewId.data->info.subresourceRange;
			auto mipLevel = subresourceRange.baseMipLevel;
			dstExtent.width = std::max( 1u, dstExtent.width >> mipLevel );
			dstExtent.height = std::max( 1u, dstExtent.height >> mipLevel );
			srcOffset.x = srcOffset.x >> mipLevel;
			srcOffset.y = srcOffset.y >> mipLevel;
			commands.copyToBuffer( VkBufferImageCopy{ 0u
					, 0u
					, 0u
					, { subresourceRange.aspectMask
						, mipLevel
						, subresourceRange.baseArrayLayer
						, subresourceRange.layerCount }
					, srcOffset
					, makeExtent3D( dstExtent ) }
				, srcImage
				, *m_stagingBuffer );
			commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_HOST_BIT
				, m_stagingBuffer->makeHostRead() );

			if ( intermediate.layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				&& intermediate.layout != VK_IMAGE_LAYOUT_UNDEFINED )
			{
				commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, ashes::getStageMask( intermediateBarrierView.layout )
					, makeLayoutTransition( srcImage
						, intermediateBarrierView.viewId.data->info.subresourceRange
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, intermediateBarrierView.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			commands.endDebugBlock();
			commands.end();
			++pass;
		}
	}

	void RenderWindow::doDestroySaveData()
	{
		m_transferCommands.clear();
		m_stagingBuffer.reset();
		m_saveBuffer.reset();
	}

	void RenderWindow::doResetSwapChain()
	{
		doWaitFrame( {} );
		getDevice()->waitIdle();

		if ( m_initialised )
		{
			doDestroyCommandBuffers();
			doDestroyRenderQuad();
			doDestroyIntermediateViews();

#if C3D_PersistLoadingScreen
			if ( m_progressBar && m_loadingScreen )
			{
				m_progressBar->lock();
				doDestroySwapchain();
				doCreateSwapchain();
				m_loadingScreen->setRenderPass( *m_renderPass, m_size );
				m_progressBar->unlock();
			}
			else
#endif
			{
				doDestroySwapchain();
				doCreateSwapchain();
			}

			doCreateIntermediateViews( *m_queue );
			doCreateRenderQuad( *m_queue );
			doCreateCommandBuffers( *m_queue );
		}
		else
		{
#if C3D_PersistLoadingScreen
			if ( m_progressBar && m_loadingScreen )
			{
				m_progressBar->lock();
				doDestroySwapchain();
				doCreateSwapchain();
				m_loadingScreen->setRenderPass( *m_renderPass, m_size );
				m_progressBar->unlock();
			}
			else
#endif
			{
				doDestroySwapchain();
				doCreateSwapchain();
			}

		}
	}

	RenderWindow::RenderingResources * RenderWindow::doGetResources()
	{
		auto & resources = *m_renderingResources[m_resourceIndex];
		uint32_t imageIndex{ 0u };
		auto res = m_swapChain->acquireNextImage( ashes::MaxTimeout
			, *resources.imageAvailableSemaphore
			, imageIndex );

		if ( doCheckNeedReset( res
			, true
			, "Swap chain image acquisition" ) )
		{
			m_resourceIndex = ( m_resourceIndex + 1 ) % m_renderingResources.size();
			resources.imageIndex = imageIndex;
			return &resources;
		}

		return nullptr;
	}

	crg::SemaphoreWaitArray RenderWindow::doSubmitLoadingFrame( RenderingResources & resources
		, LoadingScreen & loadingScreen
		, crg::Fence *& fence
		, crg::SemaphoreWaitArray toWait )
	{
		toWait.push_back( { *resources.imageAvailableSemaphore
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } );
		return loadingScreen.render( *m_queue->queue
			, *m_frameBuffers[resources.imageIndex]
			, toWait
			, fence );
	}

	void RenderWindow::doPresentLoadingFrame( crg::Fence * fence
		, RenderingResources & resources
		, crg::SemaphoreWaitArray toWait )
	{
		try
		{
			ashes::VkSemaphoreArray semaphores;
			crg::convert( toWait, semaphores );

			if ( fence )
			{
				auto res = fence->wait( ashes::MaxTimeout );
				ashes::checkError( res, "Wait between swapchain images presentation." );
			}

			m_queue->queue->present( { *m_swapChain }
				, { resources.imageIndex }
				, semaphores );

			if ( m_toSave )
			{
				std::memcpy( m_saveBuffer->getPtr(), m_stagingData.data(), m_stagingData.size() );
				m_toSave = false;
			}
		}
		catch ( ashes::Exception & exc )
		{
			doCheckNeedReset( exc.getResult()
				, false
				, "Image presentation" );
		}

		resources.imageIndex = ~0u;
	}

	void RenderWindow::doWaitFrame( crg::SemaphoreWaitArray toWait )
	{
		auto target = getRenderTarget();

		if ( target )
		{
			ashes::VkSemaphoreArray semaphores;
			ashes::VkPipelineStageFlagsArray stages;
			crg::convert( toWait, semaphores, stages );

			if ( m_toSave )
			{
				m_savedFormat = m_intermediates[m_debugConfig.debugIndex].viewId.data->info.format;
				auto & transferCommands = m_transferCommands[m_debugConfig.debugIndex];
				m_queue->queue->submit( ashes::VkCommandBufferArray{ *transferCommands.commandBuffer }
					, semaphores
					, stages
					, ashes::VkSemaphoreArray{ *transferCommands.semaphore } );
				semaphores = { *transferCommands.semaphore };
				stages = { VK_PIPELINE_STAGE_TRANSFER_BIT };
			}

			m_queue->queue->submit( ashes::VkCommandBufferArray{}
				, semaphores
				, stages
				, ashes::VkSemaphoreArray{} );
		}
	}

	void RenderWindow::doSubmitFrame( RenderingResources * resources
		, crg::SemaphoreWaitArray toWait )
	{
		ashes::VkSemaphoreArray semaphores;
		ashes::VkPipelineStageFlagsArray stages;
		crg::convert( toWait, semaphores, stages );

#if C3D_DebugQuads
		m_texture3Dto2D->render( *m_queue->queue
			, semaphores
			, stages );
#endif
		if ( m_toSave )
		{
			m_savedFormat = m_intermediates[m_debugConfig.debugIndex].viewId.data->info.format;
			auto & transferCommands = m_transferCommands[m_debugConfig.debugIndex];
			m_queue->queue->submit( ashes::VkCommandBufferArray{ *transferCommands.commandBuffer }
				, semaphores
				, stages
				, ashes::VkSemaphoreArray{ *transferCommands.semaphore } );
			semaphores = { *transferCommands.semaphore };
			stages = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		}

		semaphores.push_back( *resources->imageAvailableSemaphore );
		stages.push_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		m_queue->queue->submit( ashes::VkCommandBufferArray{ *m_commandBuffers[m_debugConfig.debugIndex][resources->imageIndex] }
			, semaphores
			, stages
			, ashes::VkSemaphoreArray{}
			, *resources->fence );
	}

	void RenderWindow::doPresentFrame( RenderingResources * resources )
	{
		try
		{
			resources->fence->wait( ashes::MaxTimeout );
			resources->fence->reset();
			m_queue->queue->present( *m_swapChain
				, resources->imageIndex );

			if ( m_toSave )
			{
				auto target = getRenderTarget();
				auto & intermediate = m_intermediateBarrierViews[m_debugConfig.debugIndex];
				auto srcExtent = getExtent( intermediate.viewId );
				auto dstExtent = makeExtent2D( target->getSize() );
				dstExtent.width = std::min( dstExtent.width, srcExtent.width );
				dstExtent.height = std::min( dstExtent.height, srcExtent.height );
				auto subresourceRange = intermediate.viewId.data->info.subresourceRange;
				auto mipLevel = subresourceRange.baseMipLevel;
				dstExtent.width = std::max( 1u, dstExtent.width >> mipLevel );
				dstExtent.height = std::max( 1u, dstExtent.height >> mipLevel );
				m_saveBuffer = castor::PxBufferBase::create( makeSize( dstExtent )
					, convert( target->getPixelFormat() )
					, m_stagingData.data()
					, castor::PixelFormat( m_savedFormat )
					, 0u );
				m_savedFormat = {};
				m_toSave = false;
			}
		}
		catch ( ashes::Exception & exc )
		{
			doCheckNeedReset( exc.getResult()
				, false
				, "Image presentation" );
		}

		resources->imageIndex = ~0u;
	}

	bool RenderWindow::doCheckNeedReset( VkResult errCode
		, bool acquisition
		, char const * const action )
	{
		bool result{ false };

		switch ( errCode )
		{
		case VK_SUCCESS:
			result = true;
			break;

		case VK_ERROR_OUT_OF_DATE_KHR:
			if ( !acquisition )
			{
				doResetSwapChain();
			}
			else
			{
				result = true;
			}
			break;

		case VK_SUBOPTIMAL_KHR:
			doResetSwapChain();
			break;

		default:
			throw ashes::Exception{ errCode, action };
		}

		return result;
	}
}
