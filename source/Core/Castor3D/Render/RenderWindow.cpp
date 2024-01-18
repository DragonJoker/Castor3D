#include "Castor3D/Render/RenderWindow.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlProgress.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Picking.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Core/SwapChainCreateInfo.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <CastorUtils/Design/BlockGuard.hpp>

CU_ImplementSmartPtr( castor3d, RenderWindow )

#define C3D_PersistLoadingScreen 1

namespace castor3d
{
	namespace rendwndw
	{
		static QueuesData * getQueueFamily( ashes::Surface const & surface
			, QueueFamilies & queues )
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

		static uint32_t getImageCount( ashes::Surface const & surface )
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

		static VkSurfaceFormatKHR selectFormat( ashes::Surface const & surface
			, bool allowHdr )
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
					, [allowHdr]( VkSurfaceFormatKHR const & lookup )
					{
						return allowHdr
							? lookup.format == VK_FORMAT_R16G16B16A16_SFLOAT
							: lookup.format == VK_FORMAT_R8G8B8A8_UNORM;
					} );

				if ( allowHdr && it == formats.end() )
				{
					it = std::find_if( formats.begin()
						, formats.end()
						, []( VkSurfaceFormatKHR const & lookup )
						{
							return lookup.format == VK_FORMAT_R8G8B8A8_UNORM;
						} );
				}

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

		static VkPresentModeKHR selectPresentMode( ashes::Surface const & surface )
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

		static ashes::SwapChainCreateInfo getSwapChainCreateInfo( ashes::Surface const & surface
			, VkExtent2D const & size
			, bool allowHdr )
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
			auto surfaceFormat = selectFormat( surface, allowHdr );
			return ashes::SwapChainCreateInfo{ 0u
				, surface
				, getImageCount( surface )
				, surfaceFormat.format
				, surfaceFormat.colorSpace
				, swapChainExtent
				, 1u
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				, VK_SHARING_MODE_EXCLUSIVE
				, {}
				, preTransform
				, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				, presentMode
				, VK_TRUE
				, VkSwapchainKHR{} };
		}

#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking

		static IntermediateView doCreateBarrierView( RenderDevice const & device
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

		static IntermediateViewArray doCreateBarrierViews( RenderDevice const & device
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

		static IntermediateView doCreateSampledView( RenderDevice const & device
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

		static IntermediateViewArray doCreateSampledViews( RenderDevice const & device
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

#endif

		static castor::Size getScreenSize()
		{
			castor::Size result;
			castor::system::getScreenSize( 0u, result );
			return result;
		}

#ifdef VK_EXT_device_fault
		static castor::StringView getAddressTypeName( VkDeviceFaultAddressTypeEXT v )
		{
			switch ( v )
			{
			case VK_DEVICE_FAULT_ADDRESS_TYPE_NONE_EXT:
				return "None";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_READ_INVALID_EXT:
				return "Read Invalid";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_WRITE_INVALID_EXT:
				return "Write Invalid";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_EXECUTE_INVALID_EXT:
				return "Execute Invalid";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_UNKNOWN_EXT:
				return "Instruction Pointer Unknown";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_INVALID_EXT:
				return "Instruction Pointer  Invalid";
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_FAULT_EXT:
				return "Instruction Pointer Fault";
			default:
				return "Unknown";
			};
		}
#endif
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
		, m_evtHandler{ std::make_unique< EvtHandler >( *this ) }
		, m_index{ s_nbRenderWindows++ }
		, m_device{ engine.getRenderSystem()->getRenderDevice() }
		, m_surface{ m_device.renderSystem.getInstance().createSurface( m_device.renderSystem.getPhysicalDevice()
			, std::move( handle ) ) }
		, m_queues{ rendwndw::getQueueFamily( *m_surface, m_device.queueFamilies ) }
		, m_reservedQueue{ m_queues->getQueueSize() > 1 ? m_queues->reserveQueue() : nullptr }
		, m_commandBufferPool{ m_device->createCommandPool( m_device.getGraphicsQueueFamilyIndex()
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) }
		, m_resources{ engine.getGraphResourceHandler() }
		, m_listener{ getEngine()->addNewFrameListener( getName() + castor::string::toString( m_index ) ) }
		, m_size{ size }
		, m_loading{ engine.isThreaded() }
		, m_configUbo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
	{
		log::debug << "Created RenderWindow, size: " << size << std::endl;

		if ( !m_surface )
		{
			CU_Exception( "Could not create Vulkan surface." );
		}

		getEngine()->getMaterialCache().initialise( m_device );
		doCreateProgram();
		auto queueData = m_reservedQueue;

		if ( !queueData )
		{
			queueData = m_queues->getQueue();
		}

		doCreateSwapchain();

		if ( engine.isThreaded() )
		{
#if C3D_PersistLoadingScreen
			doCreateLoadingScreen();
#endif
			getEngine()->registerWindow( *this );
		}

		if ( !m_reservedQueue )
		{
			m_queues->putQueue( queueData );
		}

		log::debug << "Created render window " << m_index << std::endl;
	}

	RenderWindow::~RenderWindow()noexcept
	{
		log::debug << "Destroyed render window " << m_index << std::endl;
		auto & engine = *getEngine();
		auto listener = engine.removeFrameListener( getName() + castor::string::toString( m_index ) );

#if C3D_PersistLoadingScreen
		if ( engine.isThreaded() )
		{
			doDestroyLoadingScreen();
		}
#endif

		doDestroySwapchain();
		doDestroyRenderPass();
		doDestroyProgram();
		m_device.uboPool->putBuffer( m_configUbo );

		if ( m_reservedQueue )
		{
			m_queues->unreserveQueue( m_reservedQueue );
		}
	}

	void RenderWindow::initialise( RenderWindowDesc const & desc )
	{
		enableFullScreen( desc.fullscreen );
		enableVSync( desc.enableVSync );
		allowHdrSwapchain( desc.allowHdr );
		m_renderTarget = desc.renderTarget;

		if ( m_loadingScreen )
		{
			if ( !m_loading.exchange( true ) )
			{
				m_loadingScreen->enable();
			}

			auto progress = m_progressBar.get();
			incProgressBarGlobalRange( progress
				, 1u + m_renderTarget->countInitialisationSteps() );
			getEngine()->postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
				, [this]()
				{
					if ( m_renderTarget )
					{
						auto progress = m_progressBar.get();
						m_renderTarget->initialise( [this]( RenderTarget const & rt, QueueData const & queue )
							{
								auto progress = m_progressBar.get();
								stepProgressBarGlobalStartLocal( progress
									, "Initialising: Render Window"
									, 6u );
								stepProgressBarLocal( progress, "Loading picking" );
								doCreatePickingPass( queue );
								stepProgressBarLocal( progress, "Loading intermediate views" );
								doCreateIntermediateViews( queue );
								stepProgressBarLocal( progress, "Loading combine quad" );
								doCreateRenderQuad( queue );
								stepProgressBarLocal( progress, "Loading command buffers" );
								doCreateCommandBuffers( queue );
								stepProgressBarLocal( progress, "Loading save data" );
								doCreateSaveData( queue );
								stepProgressBarLocal( progress, "Finalising..." );

								getListener()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
									, [this]()
									{
										if ( m_loadingScreen )
										{
											m_loadingScreen->disable();
										}

										m_loading = false;
										m_initialised = true;
									} ) );
							}
							, progress );
					}
					else
					{
						getListener()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
							, [this]()
							{
								if ( m_loadingScreen )
								{
									m_loadingScreen->disable();
								}

								m_loading = false;
								m_initialised = true;
							} ) );
					}
				} ) );
		}
		else
		{
			auto queueData = m_device.graphicsData();
			m_renderTarget->initialise( m_device, *queueData, nullptr );
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
		auto queueData = m_reservedQueue;

		if ( !queueData )
		{
			queueData = m_queues->getQueue();
		}

		doWaitFrame( *queueData, {} );
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

		if ( !m_reservedQueue )
		{
			m_queues->putQueue( queueData );
		}

	}

	void RenderWindow::update( CpuUpdater & updater )
	{
		if ( m_skip )
		{
			return;
		}

		auto lock( castor::makeUniqueLock( m_renderMutex ) );

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->update( updater );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->update( updater );

			if ( m_initialised )
			{
#if C3D_DebugPicking == 0 && C3D_DebugBackgroundPicking == 0
				if ( getEngine()->areDebugTargetsEnabled() )
				{
					auto & intermediates = target->getIntermediateViews();
					auto & targetDebugConfig = target->getDebugConfig();
					auto & debugConfig = target->getScene()->getDebugConfig();
					updater.combineIndex = targetDebugConfig.intermediateImageIndex;
					updater.debugIndex = debugConfig.intermediateShaderValueIndex;
					auto & intermediate = intermediates[updater.combineIndex];

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
					config.data = castor::Point4f{ intermediate.factors.isDepth ? 1.0f : 0.0f
						, 0.0f, 0.0f, 0.0f };
				}
				else
#endif
				{
					updater.combineIndex = 0u;
					updater.cellSize = 0.0f;
					updater.gridCenter = {};
					auto & config = m_configUbo.getData();
					config.multiply = castor::Point4f{ 1.0f, 1.0f, 1.0f, 1.0f };
					config.add = castor::Point4f{};
				}
			}
		}
	}

	void RenderWindow::update( GpuUpdater & updater )
	{
		if ( m_skip )
		{
			return;
		}

		auto lock( castor::makeUniqueLock( m_renderMutex ) );

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->update( updater );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->update( updater );
		}
	}

	void RenderWindow::upload( UploadData & uploader )
	{
		if ( m_skip )
		{
			return;
		}

		auto lock( castor::makeUniqueLock( m_renderMutex ) );

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->upload( uploader );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->upload( uploader );
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

		auto queueData = m_reservedQueue;

		if ( !queueData )
		{
			queueData = m_queues->getQueue();
		}

		auto target = getRenderTarget();
		auto needLoadingScreen = ( !m_initialised
			|| !target
			|| target->isInitialising()
			|| !target->isInitialised() );
		auto useLoadingScreen = needLoadingScreen
			&& m_loadingScreen
			&& m_loadingScreen->isEnabled();

		if ( useLoadingScreen )
		{
			if ( auto resources = doGetResources() )
			{
				crg::Fence * fence{};
				auto toWait = doSubmitLoadingFrame( *queueData 
					, *resources
					, *m_loadingScreen
					, fence
					, std::move( baseToWait ) );
				doPresentLoadingFrame( *queueData
					, fence
					, *resources
					, std::move( toWait ) );
			}
		}
		else if ( !needLoadingScreen )
		{
			auto toWait = target->render( m_device
				, info
				, *queueData->queue
				, baseToWait );
			baseToWait.clear();

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			m_picking->pick( m_device
				, m_mousePosition
				, *target->getCamera() );
			auto pickingToWait = m_picking->getSemaphoreWait();
			toWait.insert( toWait.end(), pickingToWait.begin(), pickingToWait.end() );
#endif

			if ( waitOnly )
			{
				doWaitFrame( *queueData, toWait );
			}
			else if ( auto resources = doGetResources() )
			{
				try
				{
					doSubmitFrame( *queueData, resources, toWait );
					doPresentFrame( *queueData, resources );
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

		if ( !m_reservedQueue )
		{
			m_queues->putQueue( queueData );
		}
	}

	void RenderWindow::resize( uint32_t x, uint32_t y )
	{
		resize( { x, y } );
	}

	void RenderWindow::resize( castor::Size const & size )
	{
		m_size = size;
		log::debug << "Resizing RenderWindow to " << size << std::endl;
		doResetSwapChainAndCommands();
	}

	void RenderWindow::setCamera( Camera & camera )
	{
		if ( auto target = getRenderTarget() )
		{
			target->setCamera( camera );
		}
	}

	void RenderWindow::enableFullScreen( bool value )
	{
		m_fullscreen = value;
	}

	SceneRPtr RenderWindow::getScene()const
	{
		SceneRPtr result{};

		if ( auto target = getRenderTarget() )
		{
			result = target->getScene();
		}

		return result;
	}

	CameraRPtr RenderWindow::getCamera()const
	{
		CameraRPtr result{};

		if ( auto target = getRenderTarget() )
		{
			result = target->getCamera();
		}

		return result;
	}

	ViewportType RenderWindow::getViewportType()const
	{
		ViewportType result{};

		if ( auto target = getRenderTarget() )
		{
			result = target->getViewportType();
		}

		return result;
	}

	void RenderWindow::setViewportType( ViewportType value )
	{
		if ( auto target = getRenderTarget() )
		{
			target->setViewportType( value );
		}
	}

	VkFormat RenderWindow::getPixelFormat()const
	{
		VkFormat result = VK_FORMAT_UNDEFINED;

		if ( auto target = getRenderTarget() )
		{
			result = target->getPixelFormat();
		}

		return result;
	}

	void RenderWindow::setScene( Scene & value )
	{
		if ( auto target = getRenderTarget() )
		{
			target->setScene( value );
		}
	}

	bool RenderWindow::isUsingStereo()const
	{
		bool result = false;

		if ( auto target = getRenderTarget() )
		{
			result = target->isUsingStereo();
		}

		return result;
	}

	void RenderWindow::setStereo( bool value )
	{
		if ( auto target = getRenderTarget() )
		{
			target->setStereo( value );
		}
	}

	float RenderWindow::getIntraOcularDistance()const
	{
		float result = 0;

		if ( auto target = getRenderTarget() )
		{
			result = target->getIntraOcularDistance();
		}

		return result;
	}

	void RenderWindow::setIntraOcularDistance( float value )
	{
		if ( auto target = getRenderTarget() )
		{
			target->setIntraOcularDistance( value );
		}
	}

	castor::Size RenderWindow::getSize()const
	{
		return m_size;
	}

	PickNodeType RenderWindow::pick( castor::Position const & position )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		return m_picking->getPickedNodeType();

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

	void RenderWindow::destroyLoadingScreen()
	{
		m_renderMutex.lock();
		doDestroyLoadingScreen();
	}

	void RenderWindow::createLoadingScreen()
	{
		doCreateLoadingScreen();
		m_loadingScreen->setRenderPass( *m_renderPass, m_size, m_swapchainFormat );
		m_renderMutex.unlock();
	}

	GeometryRPtr RenderWindow::getPickedGeometry()const
	{
		auto sel = m_picking->getPickedGeometry();

		if ( !sel )
		{
			return nullptr;
		}

		auto geometry = sel->getScene()->findGeometry( sel->getName() );
		return geometry;
	}

	Submesh const * RenderWindow::getPickedSubmesh()const
	{
		return m_picking->getPickedSubmesh();
	}

	uint32_t RenderWindow::getPickedFace()const
	{
		return m_picking->getPickedFace();
	}

	ShadowMapLightTypeArray RenderWindow::getShadowMaps()const
	{
		if ( auto target = getRenderTarget() )
		{
			return target->getShadowMaps();
		}

		return {};
	}

	ShadowBuffer * RenderWindow::getShadowBuffer()const
	{
		if ( auto target = getRenderTarget() )
		{
			return target->getShadowBuffer();
		}

		return nullptr;
	}

	void RenderWindow::enableLoading()
	{
		m_loading = getEngine()->isThreaded();
	}

	void RenderWindow::allowHdrSwapchain( bool value )
	{
		if ( value == m_allowHdrSwapchain )
		{
			return;
		}

		m_allowHdrSwapchain = value;

		if ( m_hasHdrSupport && m_swapChain )
		{
			doResetSwapChainAndCommands();
		}
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
		m_renderPass = getDevice()->createRenderPass( getName()
			, std::move( createInfo ) );
	}

	void RenderWindow::doDestroyRenderPass()noexcept
	{
		m_renderPass.reset();
	}

	void RenderWindow::doCreateProgram()
	{
		ProgramModule programModule{ getName() };
		{
			sdw::TraditionalGraphicsWriter writer{ &getEngine()->getShaderAllocator() };

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			auto c3d_mapResult = writer.declCombinedImg< UImg2DRgba32 >( "c3d_mapResult", 0u, 0u );
#else
			auto c3d_mapResult = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapResult", 0u, 0u );
#endif
			auto c3d_config = writer.declUniformBuffer( "c3d_config", 1u, 0u );
			auto c3d_multiply = c3d_config.declMember< sdw::Vec4 >( "c3d_multiply" );
			auto c3d_add = c3d_config.declMember< sdw::Vec4 >( "c3d_add" );
			auto c3d_data = c3d_config.declMember< sdw::Vec4 >( "c3d_data" );
			c3d_config.end();

			// Shader inputs
			auto inPosition = writer.declInput< sdw::Vec2 >( "inPosition", sdw::EntryPoint::eVertex, 0u );
			auto inUv = writer.declInput< sdw::Vec2 >( "inUv", sdw::EntryPoint::eVertex, 1u );
			auto inTexture = writer.declInput< sdw::Vec2 >( "inTexture", sdw::EntryPoint::eFragment, 0u );

			// Shader outputs
			auto outTexture = writer.declOutput< sdw::Vec2 >( "outTexture", sdw::EntryPoint::eVertex, 0u );
			auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0 );

			writer.implementEntryPoint( [&]( sdw::VertexIn in
				, sdw::VertexOut out )
				{
					outTexture = inUv;
					out.vtx.position = vec4( inPosition, 0.0_f, 1.0_f );
				} );

			writer.implementEntryPoint( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
					outColour = vec4( vec3( c3d_mapResult.sample( vtx_texture ).xyz() ), 1.0_f );
#else
					auto sampled = writer.declLocale( "sampled"
						, c3d_mapResult.sample( inTexture ) );

					IF( writer, c3d_data.x() == 1.0_f )
					{
						outColour = vec4( fma( sampled.xxx(), c3d_multiply.xyz(), c3d_add.xyz() )
							, 1.0_f );
					}
					ELSE
					{
						outColour = vec4( fma( sampled.xyz(), c3d_multiply.xyz(), c3d_add.xyz() )
							, 1.0_f );
					}
					FI;
#endif
				} );
			programModule.shader = writer.getBuilder().releaseShader();
		}

		m_program = makeProgramStates( getDevice(), programModule );
	}

	void RenderWindow::doDestroyProgram()noexcept
	{
		m_program.clear();
	}

	void RenderWindow::doCreateSwapchain()
	{
		m_swapChain = getDevice()->createSwapChain( rendwndw::getSwapChainCreateInfo( *m_surface
			, { m_size.getWidth(), m_size.getHeight() }
			, m_allowHdrSwapchain ) );
		log::info << "Created SwapChain [" << getName()
			<< ", FMT(" << ashes::getName( m_swapChain->getFormat() ) << ")"
			<< ", IMGS(" << m_swapChain->getImageCount() << ")"
			<< ", DIM(" << makeSize( m_swapChain->getDimensions() ) << ")"
			<< ", MODE(" << ashes::getName( m_swapChain->getPresentMode() ) << ")]" << std::endl;

		if ( !m_renderPass
			|| m_swapChain->getFormat() != m_swapchainFormat )
		{
			doCreateRenderPass();
		}

		m_swapchainFormat = m_swapChain->getFormat();
		doCreateRenderingResources();
		doCreateFrameBuffers();

#if !C3D_PersistLoadingScreen
		if ( getEngine()->isThreaded() )
		{
			doCreateLoadingScreen();
		}
#endif
	}

	void RenderWindow::doDestroySwapchain()noexcept
	{
#if !C3D_PersistLoadingScreen
		if ( getEngine()->isThreaded() )
		{
			doDestroyLoadingScreen();
		}
#endif

		doDestroyFrameBuffers();
		doDestroyRenderingResources();
		m_swapChain.reset();
		log::info << "Destroyed SwapChain [" << getName() << "]" << std::endl;
	}

	void RenderWindow::doCreateRenderingResources()
	{
		for ( uint32_t i = 0u; i < uint32_t( m_swapChain->getImageCount() ); ++i )
		{
			m_renderingResources.emplace_back( std::make_unique< RenderingResources >( getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "ImageAvailable" )
				, getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "FinishedRendering" )
				, getDevice()->createFence( getName() + castor::string::toString( i ), VkFenceCreateFlags{ 0u } )
				, m_commandBufferPool->createCommandBuffer( getName() + castor::string::toString( i ) )
				, 0u ) );
		}
	}

	void RenderWindow::doDestroyRenderingResources()noexcept
	{
		m_renderingResources.clear();
	}

	ashes::ImageViewCRefArray RenderWindow::doPrepareAttaches( size_t index )
	{
		ashes::ImageViewCRefArray attaches;
		auto & image = *m_swapChain->getImages()[index];

		for ( size_t i = 0u; i < m_renderPass->getAttachments().size(); ++i )
		{
			m_swapchainViews[index].push_back( image.createView( makeVkStruct< VkImageViewCreateInfo >( 0u
				, image
				, VK_IMAGE_VIEW_TYPE_2D
				, m_swapChain->getFormat()
				, VkComponentMapping{}
				, VkImageSubresourceRange
				{ ashes::getAspectMask( m_swapChain->getFormat() )
				, 0u
				, 1u
				, 0u
				, 1u } ) ) );
			attaches.emplace_back( m_swapchainViews[index].back() );
		}

		return attaches;
	}

	void RenderWindow::doCreateFrameBuffers()
	{
		m_swapchainViews.resize( m_swapChain->getImageCount() );
		m_frameBuffers.resize( m_swapChain->getImageCount() );

		for ( size_t i = 0u; i < m_frameBuffers.size(); ++i )
		{
			auto attaches = doPrepareAttaches( uint32_t( i ) );
			m_frameBuffers[i] = m_renderPass->createFrameBuffer( getName() + std::to_string( i )
				, m_swapChain->getDimensions()
				, std::move( attaches ) );
		}
	}

	void RenderWindow::doDestroyFrameBuffers()noexcept
	{
		m_frameBuffers.clear();
		m_swapchainViews.clear();
	}

	void RenderWindow::doCreateLoadingScreen()
	{
		auto scene = getEngine()->getLoadingScene();

		if ( !scene )
		{
			return;
		}

		auto const & manager = static_cast< ControlsManager & >( *getEngine()->getUserInputListener() );
		auto global = manager.findControl( cuT( "C3D_LoadingScreen/GlobalProgress" ) );
		auto local = manager.findControl( cuT( "C3D_LoadingScreen/LocalProgress" ) );

		if ( !m_progressBar )
		{
			m_progressBar = castor::makeUnique< ProgressBar >( *getEngine()
				, static_cast< ProgressCtrl * >( global )
				, static_cast< ProgressCtrl * >(  local ) );
		}
		else
		{
			m_progressBar->update( static_cast< ProgressCtrl * >( global )
				, static_cast< ProgressCtrl * >( local ) );
		}

		m_loadingScreen = castor::makeUnique< LoadingScreen >( *m_progressBar
			, m_device
			, m_resources
			, scene
			, *m_renderPass
#if C3D_PersistLoadingScreen
			, rendwndw::getScreenSize() );
#else
			, m_size );
#endif

		if ( m_loading && m_loadingScreen )
		{
			m_loadingScreen->enable();
		}
	}

	void RenderWindow::doDestroyLoadingScreen()noexcept
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

		if ( !target )
		{
			return;
		}

		m_picking = castor::makeUnique< Picking >( m_resources
			, m_device
			, queueData
			, target->getSize()
			, target->getCameraUbo()
			, target->getSceneUbo()
			, target->getCuller() );
	}

	void RenderWindow::doDestroyPickingPass()noexcept
	{
		m_picking.reset();
	}

	void RenderWindow::doCreateRenderQuad( QueueData const & queueData )
	{
		auto target = getRenderTarget();

		if ( !target )
		{
			return;
		}

		m_renderQuad = RenderQuadBuilder{}
			.binding( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D )
			.binding( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER )
			.texcoordConfig( rq::Texcoord{} )
			.tex3DResult( m_tex3DTo2DIntermediate )
			.build( m_device
				, getName()
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
				, VK_FILTER_NEAREST );
#else
				, VK_FILTER_LINEAR );
#endif
		m_renderQuad->createPipeline( VkExtent2D{ m_size[0], m_size[1] }
			, castor::Position{}
			, m_program
			, *m_renderPass );
		auto & context = m_device.makeContext();

#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		m_renderQuad->registerPassInputs( { makeImageViewDescriptorWrite( m_resources.createImageView( context, m_picking->getImageView() )
				, m_renderQuad->getSampler().getSampler(), 0u )
			, RenderQuad::makeDescriptorWrite( m_configUbo, 1u ) }
			, true );

#else

		for ( auto const & intermediate : m_intermediateSampledViews )
		{
			m_renderQuad->registerPassInputs( { makeImageViewDescriptorWrite( m_resources.createImageView( context, intermediate.viewId ), m_renderQuad->getSampler().getSampler(), 0u )
					, makeDescriptorWrite( m_configUbo, 1u ) }
				, intermediate.factors.invertY );
		}

#endif

		auto const & debugConfig = target->getDebugConfig();
		m_renderQuad->initialisePass( debugConfig.intermediateImageIndex );
	}

	void RenderWindow::doDestroyRenderQuad()noexcept
	{
		m_renderQuad.reset();
	}

	void RenderWindow::doRecordCommandBuffer( QueueData const & queueData
		, uint32_t passIndex )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		passIndex = 0u;
#else
		auto target = getRenderTarget();
		CU_Require( target );
		auto & intermediates = target->getIntermediateViews();
		auto & intermediate = intermediates[passIndex];
		auto const & intermediateBarrierView = m_intermediateBarrierViews[passIndex];
		auto & context = m_device.makeContext();
#endif
		auto & commandBuffers = m_commandBuffers[passIndex];
		uint32_t index = 0u;
		m_renderQuad->initialisePass( passIndex );

		for ( auto & commandBuffer : commandBuffers )
		{
			auto const & frameBuffer = *m_frameBuffers[index];
			auto name = getName() + castor::string::toString( index );

			if ( !commandBuffer )
			{
				commandBuffer = m_commandBufferPool->createCommandBuffer( name );
			}
			else
			{
				commandBuffer->reset();
			}

			commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
			commandBuffer->beginDebugBlock( { "RenderWindow " + name
				, makeFloatArray( getEngine()->getNextRainbowColour() ) } );

#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
			if ( intermediate.layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
			{
				commandBuffer->memoryBarrier( ashes::getStageMask( intermediateBarrierView.layout )
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, makeLayoutTransition( m_resources.createImage( context, intermediateBarrierView.viewId.data->image )
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
			m_renderQuad->registerPass( *commandBuffer, passIndex );
			commandBuffer->endRenderPass();

			if ( intermediate.layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				&& intermediate.layout != VK_IMAGE_LAYOUT_UNDEFINED )
			{
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, ashes::getStageMask( intermediateBarrierView.layout )
					, makeLayoutTransition( m_resources.createImage( context, intermediateBarrierView.viewId.data->image )
						, intermediateBarrierView.viewId.data->info.subresourceRange
						, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						, intermediateBarrierView.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}
#else
			commandBuffer->beginRenderPass( *m_renderPass
				, frameBuffer
				, { opaqueWhiteClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
			m_renderQuad->registerPass( *commandBuffer, passIndex );
			commandBuffer->endRenderPass();
#endif

			commandBuffer->endDebugBlock();
			commandBuffer->end();
			index++;
		}
	}

	void RenderWindow::doCreateCommandBuffers( QueueData const & queueData )
	{
		auto target = getRenderTarget();

		if ( !target )
		{
			return;
		}

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		m_commandBuffers.resize( 1u );
#else
		m_commandBuffers.resize( target->getIntermediateViews().size() );
#endif

		for ( auto & commandBuffers : m_commandBuffers )
		{
			commandBuffers.resize( m_swapChain->getImageCount() );
		}
	}

	void RenderWindow::doDestroyCommandBuffers()noexcept
	{
		m_commandBuffers.clear();
	}

	void RenderWindow::doCreateIntermediateViews( QueueData const & queueData )
	{
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		auto target = getRenderTarget();

		if ( !target || !target->hasTechnique() )
		{
			return;
		}

		VkExtent2D extent{ m_size.getWidth(), m_size.getHeight() };
		m_texture3Dto2D = castor::makeUnique< Texture3DTo2D >( m_device
			, m_resources
			, extent
			, target->getCameraUbo() );
		m_tex3DTo2DIntermediate = { "Texture3DTo2DResult"
			, m_texture3Dto2D->getTarget().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) };
		auto intermediates = getEngine()->areDebugTargetsEnabled()
			? target->getIntermediateViews()
			: IntermediateViewArray{ target->getIntermediateViews()[0] };
		m_texture3Dto2D->createPasses( queueData, intermediates );

		m_intermediateBarrierViews = rendwndw::doCreateBarrierViews( m_device
			, m_tex3DTo2DIntermediate
			, intermediates );
		m_intermediateSampledViews = rendwndw::doCreateSampledViews( m_device
			, m_tex3DTo2DIntermediate
			, intermediates );
#endif
	}

	void RenderWindow::doDestroyIntermediateViews()noexcept
	{
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		m_texture3Dto2D.reset();
		m_intermediateSampledViews.clear();
		m_intermediateBarrierViews.clear();
#endif
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
		m_snapshotBuffer = makeBufferBase( m_device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "Snapshot" );
		m_snapshotData = castor::makeArrayView( m_snapshotBuffer->lock( 0u, bufferSize, 0u )
			, bufferSize );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		m_transferCommands.resize( 1u );
#else
		auto & intermediates = target->getIntermediateViews();
		m_transferCommands.resize( intermediates.size() );
#endif
	}

	void RenderWindow::doDestroySaveData()noexcept
	{
		m_transferCommands.clear();
		m_snapshotData = {};

		if ( m_snapshotBuffer )
		{
			m_snapshotBuffer->unlock();
			m_snapshotBuffer.reset();
		}

		m_saveBuffer.reset();
	}

	void RenderWindow::doResetSwapChain()
	{
#if C3D_PersistLoadingScreen
		if ( m_progressBar && m_loadingScreen )
		{
			m_progressBar->lock();
			doDestroySwapchain();
			doCreateSwapchain();
			m_loadingScreen->setRenderPass( *m_renderPass, m_size, m_swapchainFormat );
			m_progressBar->unlock();
		}
		else
#endif
		{
			doDestroySwapchain();
			doCreateSwapchain();
		}
	}

	void RenderWindow::doResetSwapChainAndCommands()
	{
		if ( !m_skip.exchange( true ) )
		{
			getListener()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					doWaitFrame( queueData, {} );
					getDevice()->waitIdle();

					if ( !m_initialised || m_loading )
					{
						doResetSwapChain();

						if ( m_loading )
						{
							getListener()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
								, [this]( RenderDevice const & pdevice
									, QueueData const & pqueueData )
								{
									doDestroyCommandBuffers();
									doCreateCommandBuffers( pqueueData );
								} ) );
						}
					}
					else
					{
						doDestroyCommandBuffers();
						doDestroyRenderQuad();
						doDestroyIntermediateViews();
						doResetSwapChain();
						doCreateIntermediateViews( queueData );
						doCreateRenderQuad( queueData );
						doCreateCommandBuffers( queueData );
					}

					m_skip = false;
				} ) );
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

	crg::SemaphoreWaitArray RenderWindow::doSubmitLoadingFrame( QueueData const & queue
		, RenderingResources & resources
		, LoadingScreen & loadingScreen
		, crg::Fence *& fence
		, crg::SemaphoreWaitArray toWait )
	{
		toWait.push_back( { *resources.imageAvailableSemaphore
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } );
		return loadingScreen.render( *queue.queue
			, *m_frameBuffers[resources.imageIndex]
			, toWait
			, fence );
	}

	void RenderWindow::doPresentLoadingFrame( QueueData const & queueData
		, crg::Fence * fence
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

			queueData.queue->present( { *m_swapChain }
				, { resources.imageIndex }
				, semaphores );

			if ( m_toSave )
			{
				std::memcpy( m_saveBuffer->getPtr(), m_snapshotData.data(), m_snapshotData.size() );
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

	void RenderWindow::doInitialiseTransferCommands( QueueData const & queueData
		, CommandsSemaphore & transferCommands
		, uint32_t index )
	{
		if ( transferCommands.commandBuffer )
		{
			return;
		}

		auto & context = m_device.makeContext();
		auto targetExtent = makeExtent2D( m_saveBuffer->getDimensions() );

		transferCommands = { getDevice(), queueData, "Snapshot" };
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		auto target = getRenderTarget();
		CU_Require( target );
		auto intermediates = target->getIntermediateViews();
		auto & intermediate = intermediates[index];
		auto & intermediateBarrierView = m_intermediateBarrierViews[index];
		auto & intermediateSampledView = m_intermediateSampledViews[index];
#endif
		auto & commands = *transferCommands.commandBuffer;
		commands.begin();
		commands.beginDebugBlock( { "Staging Texture Download"
			, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		VkImage srcImage = m_resources.createImage( context, m_picking->getImageView().data->image );
		commands.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, makeLayoutTransition( srcImage
				, m_picking->getImageView().data->info.subresourceRange
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );
#else
		VkImage srcImage = m_resources.createImage( context, intermediateBarrierView.viewId.data->image );

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
#endif

		commands.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_snapshotBuffer->makeTransferDestination() );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		auto srcExtent = getExtent( m_picking->getImageView() );
#else
		auto srcExtent = getExtent( intermediateBarrierView.viewId );
#endif
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

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		auto subresourceRange = m_picking->getImageView().data->info.subresourceRange;
#else
		auto subresourceRange = intermediateSampledView.viewId.data->info.subresourceRange;
#endif
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
			, *m_snapshotBuffer );
		commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_snapshotBuffer->makeHostRead() );

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, makeLayoutTransition( srcImage
				, m_picking->getImageView().data->info.subresourceRange
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );
#else
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
#endif

		commands.endDebugBlock();
		commands.end();
	}

	void RenderWindow::doWaitFrame( QueueData const & queueData
		, crg::SemaphoreWaitArray toWait )
	{
		auto target = getRenderTarget();

		if ( target )
		{
			ashes::VkSemaphoreArray semaphores;
			ashes::VkPipelineStageFlagsArray stages;
			crg::convert( toWait, semaphores, stages );

			if ( m_toSave )
			{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
				m_savedFormat = m_picking->getImageView().data->info.format;
#else
				auto intermediates = target->getIntermediateViews();
				auto & debugConfig = target->getDebugConfig();
				m_savedFormat = intermediates[debugConfig.intermediateImageIndex].viewId.data->info.format;
#endif
				auto & transferCommands = m_transferCommands[debugConfig.intermediateImageIndex];
				doInitialiseTransferCommands( queueData, transferCommands, debugConfig.intermediateImageIndex );
				queueData.queue->submit( ashes::VkCommandBufferArray{ *transferCommands.commandBuffer }
					, semaphores
					, stages
					, ashes::VkSemaphoreArray{ *transferCommands.semaphore } );
				semaphores = { *transferCommands.semaphore };
				stages = { VK_PIPELINE_STAGE_TRANSFER_BIT };
			}

			queueData.queue->submit( ashes::VkCommandBufferArray{}
				, semaphores
				, stages
				, ashes::VkSemaphoreArray{} );
		}
	}

	void RenderWindow::doSubmitFrame( QueueData const & queueData
		, RenderingResources * resources
		, crg::SemaphoreWaitArray toWait )
	{
		ashes::VkSemaphoreArray semaphores;
		ashes::VkPipelineStageFlagsArray stages;
		crg::convert( toWait, semaphores, stages );
		auto target = getRenderTarget();
		CU_Require( target );
		auto & debugConfig = target->getDebugConfig();
		auto passIndex = debugConfig.intermediateImageIndex;
		doRecordCommandBuffer( queueData, passIndex );

#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		if ( getEngine()->areDebugTargetsEnabled() )
		{
			m_texture3Dto2D->render( *queueData.queue
				, semaphores
				, stages );
		}
#endif
		if ( m_toSave )
		{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			m_savedFormat = m_picking->getImageView().data->info.format;
#else
			auto intermediates = target->getIntermediateViews();
			m_savedFormat = intermediates[passIndex].viewId.data->info.format;
#endif
			auto & transferCommands = m_transferCommands[passIndex];
			doInitialiseTransferCommands( queueData, transferCommands, passIndex );
			queueData.queue->submit( ashes::VkCommandBufferArray{ *transferCommands.commandBuffer }
				, semaphores
				, stages
				, ashes::VkSemaphoreArray{ *transferCommands.semaphore } );
			semaphores = { *transferCommands.semaphore };
			stages = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		}

		semaphores.push_back( *resources->imageAvailableSemaphore );
		stages.push_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		queueData.queue->submit( ashes::VkCommandBufferArray{ *m_commandBuffers[passIndex][resources->imageIndex] }
			, semaphores
			, stages
			, ashes::VkSemaphoreArray{}
			, *resources->fence );
	}

	void RenderWindow::doPresentFrame( QueueData const & queueData
		, RenderingResources * resources )
	{
		try
		{
			resources->fence->wait( ashes::MaxTimeout );
			resources->fence->reset();
			queueData.queue->present( *m_swapChain
				, resources->imageIndex );

			if ( m_toSave )
			{
				auto target = getRenderTarget();
				CU_Require( target );
				auto & debugConfig = target->getDebugConfig();
				auto & intermediate = m_intermediateBarrierViews[debugConfig.intermediateImageIndex];
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
					, m_snapshotData.data()
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
				doResetSwapChainAndCommands();
			}
			else
			{
				result = true;
			}
			break;
		case VK_SUBOPTIMAL_KHR:
			doResetSwapChainAndCommands();
			break;
		case VK_ERROR_DEVICE_LOST:
			doProcessDeviceLost();
			break;

		default:
			throw ashes::Exception{ errCode, action };
		}

		return result;
	}

	void RenderWindow::doProcessDeviceLost()
	{
#ifdef VK_EXT_device_fault
		auto [faultCounts, faultInfo] = m_device->getDeviceFaultInfo();
		log::error << "Device lost error: " << faultInfo.description << "\n";

		if ( faultInfo.pAddressInfos )
		{
			log::error << "  Addresses: \n";

			for ( uint32_t i = 0u; i < faultCounts.addressInfoCount; ++i )
			{
				auto & info = faultInfo.pAddressInfos[i];
				log::error << "    From 0x" << std::hex << std::setw( 8u ) << std::setfill( '0' ) << ( info.reportedAddress & ~( info.addressPrecision - 1 ) )
					<< " to 0x" << std::hex << std::setw( 8u ) << ( info.reportedAddress | ( info.addressPrecision - 1 ) )
					<< ": " << rendwndw::getAddressTypeName( info.addressType ) << "\n";
			}
		}

		if ( faultInfo.pVendorInfos )
		{
			log::error << "  Vendor Infos: \n";

			for ( uint32_t i = 0u; i < faultCounts.vendorInfoCount; ++i )
			{
				auto & info = faultInfo.pVendorInfos[i];
				log::error << "    " << std::setw( 8u ) << std::setfill( '0' ) << info.vendorFaultCode
					<< ": " << info.description << "\n";
			}
		}
#endif
	}

	Engine * getEngine( WindowContext const & context )
	{
		return getEngine( *context.root );
	}
}
