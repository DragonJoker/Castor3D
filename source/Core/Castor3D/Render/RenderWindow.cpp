#include "Castor3D/Render/RenderWindow.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlProgress.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Picking.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Core/SwapChainCreateInfo.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

CU_ImplementSmartPtr( c3d, RenderWindow )

#define C3D_SupportDeviceFault 0

namespace c3d
{
	namespace rendwndw
	{
		static bool constexpr C3D_PersistLoadingScreen = true;

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
				throw ashes::Exception{ VK_ERROR_INITIALIZATION_FAILED
					, "Couldn't find a queue supporting presentation." };

			return &( *it );
		}

		static uint32_t getImageCount( ashes::Surface const & surface )
		{
			auto surfaceCaps = surface.getCapabilities();
			uint32_t desiredNumberOfSwapChainImages{ surfaceCaps.minImageCount + 1 };
			if ( ( surfaceCaps.maxImageCount > 0 ) && ( desiredNumberOfSwapChainImages > surfaceCaps.maxImageCount ) )
				desiredNumberOfSwapChainImages = surfaceCaps.maxImageCount;
			return desiredNumberOfSwapChainImages;
		}

		static VkSurfaceFormatKHR selectFormat( ashes::Surface const & surface
			, bool allowHdr )
		{
			VkSurfaceFormatKHR result{};

			if ( auto formats = surface.getFormats();
				formats.size() == 1u && formats[0].format == VK_FORMAT_UNDEFINED )
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
					it = std::find_if( formats.begin()
						, formats.end()
						, []( VkSurfaceFormatKHR const & lookup )
						{
							return lookup.format == VK_FORMAT_R8G8B8A8_UNORM;
						} );

				if ( it != formats.end() )
					result = *it;
				else
					result = formats.front();
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
					result = mode;
			}

			return result;
		}

		static ashes::SwapChainCreateInfo getSwapChainCreateInfo( ashes::Surface const & surface
			, VkExtent2D const & size
			, bool allowHdr )
		{
			auto surfaceCaps = surface.getCapabilities();
			VkExtent2D swapChainExtent{};
			if ( surfaceCaps.currentExtent.width == uint32_t( -1 ) )
				swapChainExtent = size;
			else
				swapChainExtent = surfaceCaps.currentExtent;

			VkSurfaceTransformFlagBitsKHR preTransform{};
			if ( ashes::checkFlag( surfaceCaps.supportedTransforms, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) )
				preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			else
				preTransform = surfaceCaps.currentTransform;

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

		static IntermediateView doCreateBarrierView( crg::ResourceHandler & handler
			, IntermediateView const & view )
		{
			auto imageId = view.viewId.data->image;
			auto info = view.viewId.data->info;
			crg::ImageViewId viewId{ handler.createViewId( crg::ImageViewData{ toUtf8( view.name ) + "Barrier"
				, imageId
				, info.flags
				, info.viewType
				, info.format
				, { crg::getAspectMask( info.format )
					, info.subresourceRange.baseMipLevel
					, info.subresourceRange.levelCount
					, info.subresourceRange.baseArrayLayer
					, info.subresourceRange.layerCount } } ) };
			return { view.name
				, viewId
				, view.layout
				, view.factors };
		}

		static IntermediateView doCreateBarrierView( crg::ResourceHandler & handler
			, IntermediateView const * tex3DResult
			, IntermediateView const & view )
		{
			if ( view.viewId.data->info.viewType == crg::ImageViewType::e3D )
			{
				if ( tex3DResult )
					return doCreateBarrierView( handler, *tex3DResult );
				return {};
			}
			return doCreateBarrierView( handler, view );
		}

		static IntermediateView doCreateSampledView( crg::ResourceHandler & handler
			, IntermediateView const & view )
		{
			auto imageId = view.viewId.data->image;
			auto info = view.viewId.data->info;
			crg::ImageViewId viewId{ handler.createViewId( crg::ImageViewData{ toUtf8( view.name ) + "Sampled"
				, imageId
				, info.flags
				, info.viewType
				, info.format
				, { crg::getAspectMask( info.format )
					, info.subresourceRange.baseMipLevel
					, info.subresourceRange.levelCount
					, info.subresourceRange.baseArrayLayer
					, info.subresourceRange.layerCount } } ) };
			return { view.name
				, viewId
				, view.layout
				, view.factors };
		}

		static IntermediateView doCreateSampledView( crg::ResourceHandler & handler
			, IntermediateView const * tex3DResult
			, IntermediateView const & view )
		{
			if ( view.viewId.data->info.viewType == crg::ImageViewType::e3D )
			{
				if ( tex3DResult )
					return doCreateSampledView( handler, *tex3DResult );
				return {};
			}
			return doCreateSampledView( handler, view );
		}

#endif

		static Size getScreenSize()
		{
			Size result;
			system::getScreenSize( 0u, result );
			return result;
		}

#if defined( VK_EXT_device_fault ) && C3D_SupportDeviceFault
		static String getAddressTypeName( VkDeviceFaultAddressTypeEXT v )
		{
			switch ( v )
			{
			case VK_DEVICE_FAULT_ADDRESS_TYPE_NONE_EXT:
				return cuT( "None" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_READ_INVALID_EXT:
				return cuT( "Read Invalid" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_WRITE_INVALID_EXT:
				return cuT( "Write Invalid" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_EXECUTE_INVALID_EXT:
				return cuT( "Execute Invalid" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_UNKNOWN_EXT:
				return cuT( "Instruction Pointer Unknown" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_INVALID_EXT:
				return cuT( "Instruction Pointer  Invalid" );
			case VK_DEVICE_FAULT_ADDRESS_TYPE_INSTRUCTION_POINTER_FAULT_EXT:
				return cuT( "Instruction Pointer Fault" );
			default:
				return cuT( "Unknown" );
			}
		}
#endif

		static CU_ImplementAttributeParserNewBlock( parserRenderTarget, WindowContext, TargetContext )
		{
			newBlockContext->window = blockContext;
			newBlockContext->targetType = TargetType::eWindow;
			newBlockContext->size = { 1u, 1u };
			newBlockContext->srgbPixelFormat = PixelFormat::eUNDEFINED;
			newBlockContext->hdrPixelFormat = PixelFormat::eUNDEFINED;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eRenderTarget )

		static CU_ImplementAttributeParserBlock( parserVSync, WindowContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->window.enableVSync );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFullscreen, WindowContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->window.fullscreen );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAllowHdr, WindowContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->window.allowHdr );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, WindowContext )
		{
			log::info << "Loaded window [" << blockContext->window.name
				<< ", HDR(" << blockContext->window.allowHdr << ")"
				<< ", VSYNC(" << blockContext->window.enableVSync << ")"
				<< ", FS(" << blockContext->window.fullscreen << ")]" << std::endl;
			blockContext->root->window = c3d::move( blockContext->window );
		}
		CU_EndAttributePop()
	}

	//*************************************************************************************************

	void RenderWindow::EvtHandler::doProcessMouseEvent( MouseEventSPtr event )
	{
		m_window->m_mousePosition = event->getPosition();
	}

	//*************************************************************************************************

	RenderWindow::IntermediateCommand::IntermediateCommand( crg::ResourceHandler & handler
		, String const & baseName
		, IntermediateView const * tex3DResult
		, IntermediateView intermediate
		, uint32_t swapchainImageCount
		, ashes::CommandPool const & commandBufferPool )
		: intermediateView{ c3d::move( intermediate ) }
		, intermediateBarrierView{ rendwndw::doCreateBarrierView( handler, tex3DResult, intermediateView ) }
		, intermediateSampledView{ rendwndw::doCreateSampledView( handler, tex3DResult, intermediateView ) }
	{
		commandBuffers.resize( swapchainImageCount );
		uint32_t index{};
		for ( auto & commandBuffer : commandBuffers )
		{
			auto name = baseName + string::toString( index );
			commandBuffer = commandBufferPool.createCommandBuffer( name );
			commandBuffer->begin();
			commandBuffer->end();

			++index;
		}
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( String const & name
		, Engine & engine
		, Size const & size
		, ashes::WindowHandle handle )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_evtHandler{ makeRawUnique< EvtHandler >( *this ) }
		, m_index{ s_nbRenderWindows++ }
		, m_device{ engine.getRenderSystem()->getRenderDevice() }
		, m_surface{ m_device.renderSystem.getInstance().createSurface( m_device.renderSystem.getPhysicalDevice()
			, c3d::move( handle ) ) }
		, m_queues{ rendwndw::getQueueFamily( *m_surface, m_device.queueFamilies ) }
		, m_reservedQueue{ m_queues->getQueueSize() > 1 ? m_queues->reserveQueue() : nullptr }
		, m_commandBufferPool{ m_device->createCommandPool( m_device.getGraphicsQueueFamilyIndex()
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) }
		, m_resources{ engine.getGraphResourceHandler() }
		, m_listener{ getEngine()->addNewFrameListener( getName() + string::toString( m_index ) ) }
		, m_size{ size }
		, m_loading{ engine.isThreaded() }
		, m_configUbo{ m_device }
	{
		log::debug << "Created RenderWindow, size: " << size << std::endl;
		if ( !m_surface )
			CU_Exception( "Could not create Vulkan surface." );

		getEngine()->getMaterialCache().initialise( m_device );
		doCreateProgram();
		auto queueData = m_reservedQueue;
		if ( !queueData )
			queueData = m_queues->getQueue();

		doCreateSwapchain();

		if ( engine.isThreaded() )
		{
			if constexpr ( rendwndw::C3D_PersistLoadingScreen )
				doCreateLoadingScreen();
			getEngine()->registerWindow( *this );
		}

		if ( !m_reservedQueue )
			m_queues->putQueue( queueData );
		log::debug << "Created render window " << m_index << std::endl;
	}

	RenderWindow::~RenderWindow()noexcept
	{
		log::debug << "Destroyed render window " << m_index << std::endl;
		auto & engine = *getEngine();
		auto listener = engine.removeFrameListener( getName() + string::toString( m_index ) );

		if ( rendwndw::C3D_PersistLoadingScreen
			&& engine.isThreaded() )
		{
			doDestroyLoadingScreen();
		}

		doDestroySwapchain();
		doDestroyRenderPass();
		doDestroyProgram();

		if ( m_reservedQueue )
			m_queues->unreserveQueue( m_reservedQueue );
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
				m_loadingScreen->enable();

			auto progress = m_progressBar.get();
			incProgressBarGlobalRange( progress
				, 1u + m_renderTarget->countInitialisationSteps() );
			getEngine()->postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
				, [this]()
				{
					if ( m_renderTarget )
					{
						m_renderTarget->initialise( [this]( RenderTarget const &, QueueData const & queue )
							{
								auto prgrss = m_progressBar.get();
								stepProgressBarGlobalStartLocal( prgrss
									, cuT( "Initialising: Render Window" )
									, 6u );
								stepProgressBarLocal( prgrss, cuT( "Loading picking" ) );
								doCreatePickingPass( queue );
								stepProgressBarLocal( prgrss, cuT( "Loading intermediate views" ) );
								doCreateIntermediateViews( queue );
								stepProgressBarLocal( prgrss, cuT( "Loading save data" ) );
								doCreateSaveData();
								stepProgressBarLocal( prgrss, cuT( "Finalising..." ) );

								getListener()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
									, [this]()
									{
										if ( m_loadingScreen )
											m_loadingScreen->disable();
										m_loading = false;
										m_initialised = true;
									} ) );
							}
							, m_progressBar.get() );
					}
					else
					{
						getListener()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
							, [this]()
							{
								if ( m_loadingScreen )
									m_loadingScreen->disable();
								m_loading = false;
								m_initialised = true;
							} ) );
					}
				} ) );
		}
		else
		{
			auto queueData = m_device.graphicsData();
			m_renderTarget->initialise( m_device, nullptr );
			doCreatePickingPass( *queueData );
			doCreateIntermediateViews( *queueData );
			doCreateSaveData();

			if ( m_loadingScreen )
				m_loadingScreen->disable();
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
			queueData = m_queues->getQueue();

		auto lock = makeUniqueLock( m_renderMutex );
		doWaitFrame( *queueData, {}, ~0u );
		getDevice()->waitIdle();

		doDestroySaveData();
		if ( engine.isThreaded() )
			doDestroyLoadingScreen();
		doDestroyIntermediateViews();
		doDestroyPickingPass();

		if ( auto target = getRenderTarget() )
			target->cleanup( m_device );
		if ( !m_reservedQueue )
			m_queues->putQueue( queueData );
	}

	void RenderWindow::update( CpuUpdater & updater )
	{
		if ( m_skip )
			return;

		auto lock( makeUniqueLock( m_renderMutex ) );

		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			m_loadingScreen->update( updater );
			auto extent = m_loadingScreen->getResult().getExtent();
			auto config = m_configUbo.getData();
			config.multiply = Point4f{ 1.0f, 1.0f, 1.0f, 1.0f };
			config.add = Point4f{};
			config.uvMultiplyAdd = Point4f{ float( m_size->x ) / float( extent.width )
				, float( m_size->y ) / float( extent.height )
				, 0.0f, 0.0f };
			m_configUbo.setData( config );
		}
		else if ( auto target = getRenderTarget() )
		{
			target->update( updater );

			if ( m_initialised )
			{
#if C3D_DebugPicking == 0 && C3D_DebugBackgroundPicking == 0
				if ( getEngine()->areDebugTargetsEnabled() )
				{
					auto const & targetDebugConfig = target->getDebugConfig();
					auto const & debugConfig = target->getScene()->getDebugConfig();
					updater.combineIndex = targetDebugConfig.intermediateImageIndex;
					updater.debugIndex = debugConfig.intermediateShaderValueIndex;
					auto const & intermediate = m_intermediates[updater.combineIndex + 1u].intermediateView;

					if ( intermediate.factors.grid )
					{
						updater.cellSize = ( *intermediate.factors.grid )->w;
						updater.gridCenter = Point3f{ *intermediate.factors.grid };
					}
					else
					{
						updater.cellSize = 0.0f;
						updater.gridCenter = {};
					}

					if (m_texture3Dto2D )
						m_texture3Dto2D->update( updater );

					auto config = m_configUbo.getData();
					config.multiply = Point4f{ intermediate.factors.multiply };
					config.add = Point4f{ intermediate.factors.add };
					config.data = Point4f{ intermediate.factors.isDepth ? 1.0f : 0.0f
						, 0.0f, 0.0f, 0.0f };
					config.uvMultiplyAdd = Point4f{ 1.0f, 1.0f, 0.0f, 0.0f };
					m_configUbo.setData( config );
				}
				else
#endif
				{
					updater.combineIndex = 0u;
					updater.cellSize = 0.0f;
					updater.gridCenter = {};
					auto config = m_configUbo.getData();
					config.multiply = Point4f{ 1.0f, 1.0f, 1.0f, 1.0f };
					config.add = Point4f{};
					config.uvMultiplyAdd = Point4f{ 1.0f, 1.0f, 0.0f, 0.0f };
					m_configUbo.setData( config );
				}
			}
		}
	}

	void RenderWindow::update( GpuUpdater & updater )
	{
		if ( m_skip )
			return;

		auto lock( makeUniqueLock( m_renderMutex ) );
		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
			m_loadingScreen->update( updater );
		else if ( auto target = getRenderTarget() )
			target->update( updater );
	}

	void RenderWindow::upload( UploadData & uploader )
	{
		if ( m_skip )
			return;

		auto lock( makeUniqueLock( m_renderMutex ) );
		if ( m_loadingScreen && m_loadingScreen->isEnabled() )
			m_loadingScreen->upload( uploader );
		else if ( auto target = getRenderTarget() )
			target->upload( uploader );
	}

	void RenderWindow::render( bool waitOnly
		, SemaphoreWaitArray & baseToWait )
	{
		if ( m_skip )
			return;

		auto queueData = m_reservedQueue;
		if ( !queueData )
			queueData = m_queues->getQueue();

		auto lock = makeUniqueLock( m_renderMutex );
		auto target = getRenderTarget();
		SemaphoreWaitArray toWait;
		uint32_t intermediateImageIndex{ ~0u };
		Size displaySize{};

		if ( auto needLoadingScreen = ( !m_initialised || !target
				|| target->isInitialising() || !target->isInitialised() );
			needLoadingScreen && m_loadingScreen && m_loadingScreen->isEnabled() )
		{
			intermediateImageIndex = 0u;
			m_loadingScreen->record();
			displaySize = makeSize( m_loadingScreen->getResult().getExtent() );
			toWait = m_loadingScreen->render( *queueData->queue
				, baseToWait );
			baseToWait.clear();
		}
		else if ( !needLoadingScreen )
		{
			intermediateImageIndex = target->getDebugConfig().intermediateImageIndex
				+ ( m_loadingScreen ? 1u : 0u );
			displaySize = target->getDisplaySize();
			toWait = target->render( *queueData->queue
				, baseToWait );
			baseToWait.clear();

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			m_picking->pick( m_device
				, m_mousePosition
				, *target->getCamera() );
			auto pickingToWait = m_picking->getSemaphoreWait();
			toWait.insert( toWait.end(), pickingToWait.begin(), pickingToWait.end() );
#endif
		}

		if ( waitOnly || !m_renderQuad )
		{
			doWaitFrame( *queueData, toWait, intermediateImageIndex );
		}
		else if ( auto resources = doGetResources() )
		{
			if ( intermediateImageIndex != ~0u )
			{
				try
				{
					doSubmitFrame( *queueData, resources, toWait, intermediateImageIndex );
					doPresentFrame( *queueData, resources, intermediateImageIndex, displaySize );
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
		}
		else
		{
			std::cerr << "Can't render" << std::endl;
		}

		if ( !m_reservedQueue )
			m_queues->putQueue( queueData );
	}

	void RenderWindow::resize( uint32_t x, uint32_t y )
	{
		resize( { x, y } );
	}

	void RenderWindow::resize( Size const & size )
	{
		m_size = size;
		log::debug << "Resizing RenderWindow to " << size << std::endl;
		doResetSwapChainAndCommands();
	}

	void RenderWindow::setCamera( Camera & camera )const
	{
		if ( auto target = getRenderTarget() )
			target->setCamera( camera );
	}

	void RenderWindow::enableFullScreen( bool value )
	{
		m_fullscreen = value;
	}

	SceneRPtr RenderWindow::getScene()const
	{
		SceneRPtr result{};
		if ( auto target = getRenderTarget() )
			result = target->getScene();
		return result;
	}

	CameraRPtr RenderWindow::getCamera()const
	{
		CameraRPtr result{};
		if ( auto target = getRenderTarget() )
			result = target->getCamera();
		return result;
	}

	ViewportType RenderWindow::getViewportType()const
	{
		ViewportType result{};
		if ( auto target = getRenderTarget() )
			result = target->getViewportType();
		return result;
	}

	void RenderWindow::setViewportType( ViewportType value )const
	{
		if ( auto target = getRenderTarget() )
			target->setViewportType( value );
	}

	PixelFormat RenderWindow::getPixelFormat()const
	{
		PixelFormat result = PixelFormat::eUNDEFINED;
		if ( auto target = getRenderTarget() )
			result = target->getPixelFormat();
		return result;
	}

	void RenderWindow::setScene( Scene & value )const
	{
		if ( auto target = getRenderTarget() )
			target->setScene( value );
	}

	bool RenderWindow::isUsingStereo()const
	{
		bool result = false;
		if ( auto target = getRenderTarget() )
			result = target->isUsingStereo();
		return result;
	}

	void RenderWindow::setStereo( bool value )const
	{
		if ( auto target = getRenderTarget() )
			target->setStereo( value );
	}

	float RenderWindow::getIntraOcularDistance()const
	{
		float result = 0;
		if ( auto target = getRenderTarget() )
			result = target->getIntraOcularDistance();
		return result;
	}

	void RenderWindow::setIntraOcularDistance( float value )const
	{
		if ( auto target = getRenderTarget() )
			target->setIntraOcularDistance( value );
	}

	Size RenderWindow::getSize()const
	{
		return m_size;
	}

	PickNodeType RenderWindow::pick( Position const & position )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		return m_picking->getPickedNodeType();

#else
		PickNodeType result = PickNodeType::eNone;
		if ( auto camera = getCamera();
			camera && !m_picking->isPicking() )
			result = m_picking->pick( position );
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

	void RenderWindow::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< WindowContext > context{ result, CSCNSection::eWindow, CSCNSection::eRoot };

		context.addParser( cuT( "vsync" ), rendwndw::parserVSync, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		context.addParser( cuT( "fullscreen" ), rendwndw::parserFullscreen, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		context.addParser( cuT( "allow_hdr" ), rendwndw::parserAllowHdr, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		context.addPushParser( cuT( "render_target" ), CSCNSection::eRenderTarget, rendwndw::parserRenderTarget );
		context.addPopParser( cuT( "}" ), rendwndw::parserEnd );
	}

	GeometryRPtr RenderWindow::getPickedGeometry()const
	{
		auto sel = m_picking->getPickedGeometry();
		if ( !sel )
			return nullptr;

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
			return target->getShadowMaps();
		return {};
	}

	ShadowBuffer * RenderWindow::getShadowBuffer()const
	{
		if ( auto target = getRenderTarget() )
			return target->getShadowBuffer();
		return nullptr;
	}

	void RenderWindow::enableLoading()
	{
		m_loading = getEngine()->isThreaded();
	}

	void RenderWindow::allowHdrSwapchain( bool value )
	{
		if ( value == m_allowHdrSwapchain )
			return;

		m_allowHdrSwapchain = value;
		if ( m_hasHdrSupport && m_swapChain )
			doResetSwapChainAndCommands();
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
			, c3d::move( attaches )
			, c3d::move( subpasses )
			, c3d::move( dependencies ) };
		m_renderPass = getDevice()->createRenderPass( toUtf8( getName() )
			, c3d::move( createInfo ) );
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
			auto c3d_mapResult = writer.declCombinedImg< FImg2DRgba8Unorm >( "c3d_mapResult", 0u, 0u );
#endif
			auto c3d_config = writer.declUniformBuffer( "c3d_config", 1u, 0u );
			auto c3d_multiply = c3d_config.declMember< sdw::Vec4 >( "c3d_multiply" );
			auto c3d_add = c3d_config.declMember< sdw::Vec4 >( "c3d_add" );
			auto c3d_uvMultiplyAdd = c3d_config.declMember< sdw::Vec4 >( "c3d_uvMultiplyAdd" );
			auto c3d_data = c3d_config.declMember< sdw::Vec4 >( "c3d_data" );
			c3d_config.end();

			// Shader inputs
			auto inUv = writer.declInput< sdw::Vec2 >( "inUv", sdw::EntryPoint::eVertex, 1u );

			// Shader outputs
			auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0 );

			writer.implementEntryPointT< shader::PosUv2FT, shader::Uv2FT >( []( sdw::VertexInT< shader::PosUv2FT > const & in
				, sdw::VertexOutT< shader::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< shader::Uv2FT, shader::Colour4FT >( [&c3d_mapResult, c3d_uvMultiplyAdd, &c3d_data, &c3d_multiply, &c3d_add
					, &writer]( sdw::FragmentInT< shader::Uv2FT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					auto sampled = writer.declLocale( "sampled"
						, c3d_mapResult.sample( sdw::fma( in.uv(), c3d_uvMultiplyAdd.xy(), c3d_uvMultiplyAdd.zw() ) ) );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
					out.colour() = vec4( vec3( sampled.xyz() ), 1.0_f );
#else
					sdwIF( writer, c3d_data.x() == 1.0_f )
					{
						out.colour() = vec4( fma( sampled.xxx(), c3d_multiply.xyz(), c3d_add.xyz() )
							, 1.0_f );
					}
					sdwELSE
					{
						out.colour() = vec4( fma( sampled.xyz(), c3d_multiply.xyz(), c3d_add.xyz() )
							, 1.0_f );
					}
					sdwFI
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
		log::info << cuT( "Created SwapChain [" ) << getName()
			<< cuT( ", FMT(" ) << makeString( ashes::getName( m_swapChain->getFormat() ) ) << cuT( ")" )
			<< cuT( ", IMGS(" ) << m_swapChain->getImageCount() << cuT( ")" )
			<< cuT( ", DIM(" ) << makeSize( m_swapChain->getDimensions() ) << cuT( ")" )
			<< cuT( ", MODE(" ) << makeString( ashes::getName( m_swapChain->getPresentMode() ) ) << cuT( ")]" ) << std::endl;

		if ( !m_renderPass
			|| m_swapChain->getFormat() != convert( m_swapchainFormat ) )
			doCreateRenderPass();

		m_swapchainFormat = c3d::convert( m_swapChain->getFormat() );
		doCreateRenderingResources();
		doCreateFrameBuffers();
		if constexpr ( !rendwndw::C3D_PersistLoadingScreen )
			doCreateLoadingScreen();
		doCreateRenderQuad();
	}

	void RenderWindow::doDestroySwapchain()noexcept
	{
		doDestroyRenderQuad();
		if constexpr ( !rendwndw::C3D_PersistLoadingScreen  )
			doDestroyLoadingScreen();
		doDestroyFrameBuffers();
		doDestroyRenderingResources();
		m_swapChain.reset();
		log::info << "Destroyed SwapChain [" << getName() << "]" << std::endl;
	}

	void RenderWindow::doCreateRenderingResources()
	{
		for ( uint32_t i = 0u; i < uint32_t( m_swapChain->getImageCount() ); ++i )
		{
			auto mbName = toUtf8( getName() + string::toString( i ) );
			m_renderingResources.emplace_back( makeRawUnique< RenderingResources >( getDevice()->createSemaphore( mbName + "ImageAvailable" )
				, getDevice()->createSemaphore( mbName + "FinishedRendering" )
				, getDevice()->createFence( mbName, VkFenceCreateFlags{ 0u } )
				, m_commandBufferPool->createCommandBuffer( mbName )
				, 0u ) );
		}
	}

	void RenderWindow::doDestroyRenderingResources()noexcept
	{
		m_renderingResources.clear();
	}

	void RenderWindow::doCreateFrameBuffers()
	{
		m_swapchainBuffers.reserve( m_swapChain->getImageCount() );

		for ( uint32_t i = 0u; i < m_swapChain->getImageCount(); ++i )
		{
			ashes::ImageViewCRefArray attaches;
			auto & image = *m_swapChain->getImages()[i];
			auto & swapchainBuffer = m_swapchainBuffers.emplace_back( image.createView( makeVkStruct< VkImageViewCreateInfo >( 0u
				, image
				, VK_IMAGE_VIEW_TYPE_2D
				, m_swapChain->getFormat()
				, VkComponentMapping{}
				, VkImageSubresourceRange{ ashes::getAspectMask( m_swapChain->getFormat() ), 0u, 1u, 0u, 1u } ) ) );
			attaches.emplace_back( swapchainBuffer.view );

			auto mbName = toUtf8( getName() + string::toString( i ) );
			swapchainBuffer.frameBuffer = m_renderPass->createFrameBuffer( mbName
				, m_swapChain->getDimensions()
				, c3d::move( attaches ) );
		}
	}

	void RenderWindow::doDestroyFrameBuffers()noexcept
	{
		m_swapchainBuffers.clear();
	}

	void RenderWindow::doCreateLoadingScreen()
	{
		auto scene = getEngine()->getLoadingScene();
		if ( !scene )
			return;

		auto const & manager = static_cast< ControlsManager & >( *getEngine()->getUserInputListener() );
		auto global = manager.findControl( cuT( "C3D_LoadingScreen/GlobalProgress" ), scene );
		auto local = manager.findControl( cuT( "C3D_LoadingScreen/LocalProgress" ), scene );

		if ( !m_progressBar )
			m_progressBar = makeUnique< ProgressBar >( *getEngine()
				, static_cast< ProgressCtrl * >( global )
				, static_cast< ProgressCtrl * >(  local ) );
		else
			m_progressBar->update( static_cast< ProgressCtrl * >( global )
				, static_cast< ProgressCtrl * >( local ) );

		if ( rendwndw::C3D_PersistLoadingScreen && getEngine()->isThreaded() )
			m_loadingScreen = makeUnique< LoadingScreen >( *m_progressBar
				, m_device
				, m_resources
				, scene
				, *m_renderPass
				, rendwndw::getScreenSize() );
		else
			m_loadingScreen = makeUnique< LoadingScreen >( *m_progressBar
				, m_device
				, m_resources
				, scene
				, *m_renderPass
				, m_size );

		if ( m_loadingScreen )
		{
			if ( m_loading )
				m_loadingScreen->enable();

			//
			m_intermediates.emplace( m_intermediates.begin()
				, m_resources.getHandler()
				, getName()
				, m_texture3Dto2D ? &m_tex3DTo2DIntermediate : nullptr
				, IntermediateView{ cuT( "LoadingScreen Result" ), m_loadingScreen->getResult(), ImageLayout::eColorAttachment }
				, m_swapChain->getImageCount()
				, *m_commandBufferPool );
		}
	}

	void RenderWindow::doDestroyLoadingScreen()noexcept
	{
		if ( m_loadingScreen )
		{
			if ( m_loading )
				m_loadingScreen->disable();
			m_loadingScreen.reset();

			//
			m_intermediates.erase( m_intermediates.begin() );
		}
	}

	void RenderWindow::doCreatePickingPass( QueueData const & queueData )
	{
		auto target = getRenderTarget();
		if ( !target )
			return;

		m_picking = makeUnique< Picking >( m_resources
			, m_device
			, queueData
			, target->getDisplaySize()
			, target->getCameraUbo()
			, target->getRenderUbo()
			, target->getSceneUbo()
			, target->getCuller() );
	}

	void RenderWindow::doDestroyPickingPass()noexcept
	{
		m_picking.reset();
	}

	void RenderWindow::doCreateRenderQuad()
	{
		m_renderQuad = RenderQuadBuilder{}
			.binding( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ImageViewType::e2D )
			.binding( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER )
			.texcoordConfig( rq::Texcoord{} )
			.build( m_device
				, getName()
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
				, FilterMode::eNearest );
#else
				, FilterMode::eLinear );
#endif
		m_renderQuad->createPipeline( Extent2D{ m_size[0], m_size[1] }
			, Position{}
			, m_program
			, *m_renderPass );
		auto & context = m_device.makeContext();

#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		m_renderQuad->registerPassInputs( { makeImageViewDescriptorWrite( m_resources.createImageView( context, m_picking->getImageView() )
				, m_renderQuad->getSampler().getSampler(), 0u )
			, RenderQuad::makeDescriptorWrite( m_configUbo, 1u ) }
			, true );
		m_renderQuad->initialisePass( 0u );

#else

		if ( !m_intermediates.empty() )
		{
			for ( auto const & intermediate : m_intermediates )
			{
				auto const & intermediateView = intermediate.intermediateSampledView;
				m_renderQuad->registerPassInputs( { makeImageViewDescriptorWrite( m_resources.createImageView( context, intermediateView.viewId ), m_renderQuad->getSampler().getSampler(), 0u )
						, m_configUbo.getDescriptorWrite( 1u ) }
					, intermediateView.factors.invertY );
			}

			m_renderQuad->initialisePass( 0u );
		}

#endif
	}

	void RenderWindow::doDestroyRenderQuad()noexcept
	{
		m_renderQuad.reset();
	}

	void RenderWindow::doRecordCommandBuffer( uint32_t passIndex )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		passIndex = 0u;
#else
		auto const & intermediateCommands = m_intermediates[passIndex];
		auto const & intermediate = intermediateCommands.intermediateView;
		auto const & intermediateBarrierView = intermediateCommands.intermediateSampledView;
		auto & context = m_device.makeContext();
#endif
		auto & commandBuffers = intermediateCommands.commandBuffers;
		uint32_t index = 0u;
		m_renderQuad->initialisePass( passIndex );

		for ( auto & commandBuffer : commandBuffers )
		{
			auto const & frameBuffer = *m_swapchainBuffers[index].frameBuffer;
			commandBuffer->reset();
			commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
			commandBuffer->beginDebugBlock( { "RenderWindow " + getName() + string::toString( index )
				, makeFloatArray( getEngine()->getNextRainbowColour() ) } );

#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
			if ( intermediate.layout != ImageLayout::eShaderReadOnly )
				commandBuffer->memoryBarrier( ashes::getStageMask( convert( intermediateBarrierView.layout ) )
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, makeLayoutTransition( m_resources.createImage( context, intermediateBarrierView.viewId.data->image )
						, intermediateBarrierView.viewId.data->info.subresourceRange
						, intermediateBarrierView.layout
						, ImageLayout::eShaderReadOnly
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );

			commandBuffer->beginRenderPass( *m_renderPass
				, frameBuffer
				, { convert( ClearValue{ opaqueWhiteClearColor } ) }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_renderQuad->registerPass( *commandBuffer, passIndex );
			commandBuffer->endRenderPass();

			if ( intermediate.layout != ImageLayout::eShaderReadOnly && intermediate.layout != ImageLayout::eUndefined )
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, ashes::getStageMask( convert( intermediateBarrierView.layout ) )
					, makeLayoutTransition( m_resources.createImage( context, intermediateBarrierView.viewId.data->image )
						, intermediateBarrierView.viewId.data->info.subresourceRange
						, ImageLayout::eShaderReadOnly
						, intermediateBarrierView.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
#else
			commandBuffer->beginRenderPass( *m_renderPass
				, frameBuffer
				, { convert( ClearValue{ opaqueWhiteClearColor } ) }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_renderQuad->registerPass( *commandBuffer, passIndex );
			commandBuffer->endRenderPass();
#endif

			commandBuffer->endDebugBlock();
			commandBuffer->end();
			index++;
		}
	}

	void RenderWindow::doCreateIntermediateViews( QueueData const & queueData )
	{
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		auto target = getRenderTarget();

		if ( !m_loadingScreen
			&& ( !target || !target->hasTechnique() ) )
			return;

		auto lock( makeUniqueLock( m_renderMutex ) );

		if ( target )
		{
			if ( m_device.hasGeometryShader() )
			{
				Extent2D extent{ m_size.getWidth(), m_size.getHeight() };
				m_texture3Dto2D = makeUnique< Texture3DTo2D >( m_device
					, m_resources
					, extent
					, target->getCameraUbo() );
				m_tex3DTo2DIntermediate = { cuT( "Texture3DTo2DResult" )
					, m_texture3Dto2D->getTarget().getSampledViewId()
					, ImageLayout::eShaderReadOnly
					, TextureFactors{}.invert( true ) };
			}

			IntermediateViewArray intermediates;
			for ( auto & intermediate : m_intermediates )
				intermediates.emplace_back( intermediate.intermediateView );

			auto targetIntermediates = target->getIntermediateViews();
			auto current = targetIntermediates.begin();
			do
			{
				intermediates.emplace_back( *current );
				m_intermediates.emplace_back( m_resources.getHandler()
					, getName()
					, m_texture3Dto2D ? &m_tex3DTo2DIntermediate : nullptr
					, *current
					, m_swapChain->getImageCount()
					, *m_commandBufferPool );
				++current;
			}
			while ( getEngine()->areDebugTargetsEnabled()
				&& current != targetIntermediates.end() );

			if ( m_texture3Dto2D )
				m_texture3Dto2D->createPasses( queueData, intermediates );
		}

		if ( m_renderQuad )
		{
			auto & context = m_device.makeContext();
			auto begin = m_loadingScreen ? std::next( m_intermediates.begin() ) : m_intermediates.begin();
			for ( auto const & intermediate : makeArrayView( begin, m_intermediates.end() ) )
			{
				m_renderQuad->registerPassInputs( { makeImageViewDescriptorWrite( m_resources.createImageView( context, intermediate.intermediateSampledView.viewId ), m_renderQuad->getSampler().getSampler(), 0u )
						, m_configUbo.getDescriptorWrite( 1u ) }
				, intermediate.intermediateSampledView.factors.invertY );
			}

			if ( target )
				m_renderQuad->initialisePass( target->getDebugConfig().intermediateImageIndex );
			else
				m_renderQuad->initialisePass( 0u );
		}
#endif
	}

	void RenderWindow::doDestroyIntermediateViews()noexcept
	{
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking

		auto lock = makeUniqueLock( m_renderMutex );

		m_texture3Dto2D.reset();
		m_tex3DTo2DIntermediate = {};

		if ( m_renderQuad )
		{
			if ( m_loadingScreen )
				m_renderQuad->unregisterPasses( 1u, ~0u );
			else
				m_renderQuad->unregisterPasses( 0u, ~0u );
		}

		if ( !m_intermediates.empty() )
		{
			if ( m_loadingScreen )
				m_intermediates.erase( std::next( m_intermediates.begin() ), m_intermediates.end() );
			else
				m_intermediates.clear();
		}

#endif
	}

	void RenderWindow::doCreateSaveData()
	{
		auto target = getRenderTarget();
		m_saveBuffer = PxBufferBase::create( target->getDisplaySize(), target->getPixelFormat() );
		auto targetExtent = makeExtent2D( m_saveBuffer->getDimensions() );
		auto bufferSize = ashes::getAlignedSize( ashes::getLevelsSize( convert( targetExtent )
			, VK_FORMAT_R32G32B32A32_SFLOAT // Reserve enough room to hold max image size
			, 0u
			, 1u
			, 1u )
			, getDevice().renderSystem.getValue( GpuMin::eBufferMapSize ) );
		m_snapshotBuffer = makeBufferBase( m_device
			, target->getResources()
			, bufferSize
			, BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc
			, MemoryPropertyFlags::eHostVisible
			, cuT( "Snapshot" ) );
		m_snapshotData = makeArrayView( m_snapshotBuffer->lock()
			, bufferSize );
	}

	void RenderWindow::doDestroySaveData()noexcept
	{
		m_snapshotData = {};

		if ( m_snapshotBuffer )
		{
			m_snapshotBuffer->unlock();
			m_snapshotBuffer->destroy();
			m_snapshotBuffer.reset();
		}

		m_saveBuffer.reset();
	}

	void RenderWindow::doResetSwapChain()
	{
		if ( rendwndw::C3D_PersistLoadingScreen
			&& getEngine()->isThreaded()
			&& m_progressBar
			&& m_loadingScreen )
		{
			m_progressBar->lock();
			doDestroySwapchain();
			doCreateSwapchain();
			m_loadingScreen->setRenderPass( *m_renderPass, m_size, m_swapchainFormat );
			m_progressBar->unlock();
		}
		else
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
				, [this]( RenderDevice const &
					, QueueData const & queueData )
				{
					doWaitFrame( queueData, {}, ~0u );
					getDevice()->waitIdle();

					if ( !m_initialised || m_loading )
					{
						doResetSwapChain();
					}
					else
					{
						doDestroyIntermediateViews();
						doResetSwapChain();
						doCreateIntermediateViews( queueData );
					}

					m_skip = false;
				} ) );
		}
	}

	RenderWindow::RenderingResources * RenderWindow::doGetResources()
	{
		auto & resources = *m_renderingResources[m_resourceIndex];
		uint32_t imageIndex{ 0u };

		if ( auto res = m_swapChain->acquireNextImage( ashes::MaxTimeout
				, *resources.imageAvailableSemaphore
				, imageIndex );
			doCheckNeedReset( res
				, true
				, "Swap chain image acquisition" ) )
		{
			m_resourceIndex = ( m_resourceIndex + 1 ) % m_renderingResources.size();
			resources.imageIndex = imageIndex;
			return &resources;
		}

		return nullptr;
	}

	void RenderWindow::doInitialiseTransferCommands( QueueData const & queueData
		, uint32_t index )
	{
		auto & intermediateCommands = m_intermediates[index];
		auto & transferCommands = intermediateCommands.transferCommands;
		if ( transferCommands.commandBuffer )
			return;

		auto lock = makeUniqueLock( m_renderMutex );
		auto & context = m_device.makeContext();
		auto targetExtent = makeExtent2D( m_saveBuffer->getDimensions() );

		transferCommands = { getDevice(), queueData, cuT( "Snapshot" ) };
#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		auto const & intermediate = intermediateCommands.intermediateView;
		auto const & intermediateBarrierView = intermediateCommands.intermediateBarrierView;
		auto const & intermediateSampledView = intermediateCommands.intermediateSampledView;
#endif
		auto const & commands = *transferCommands.commandBuffer;
		commands.begin();
		commands.beginDebugBlock( { "Staging Texture Download"
			, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		VkImage srcImage = m_resources.createImage( context, m_picking->getImageView().data->image );
		commands.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, makeLayoutTransition( srcImage
				, m_picking->getImageView().data->info.subresourceRange
				, ImageLayout::eShaderReadOnly
				, ImageLayout::eTransferSrc
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );
#else
		VkImage srcImage = m_resources.createImage( context, intermediateBarrierView.viewId.data->image );

		if ( intermediate.layout != ImageLayout::eTransferSrc )
		{
			commands.memoryBarrier( getPipelineStageFlags( getStageMask( intermediateBarrierView.layout ) )
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, makeLayoutTransition( srcImage
					, intermediateBarrierView.viewId.data->info.subresourceRange
					, intermediateBarrierView.layout
					, ImageLayout::eTransferSrc
					, VK_QUEUE_FAMILY_IGNORED
					, VK_QUEUE_FAMILY_IGNORED ) );
		}
#endif

		commands.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_snapshotBuffer->getBuffer().makeTransferDestination() );
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		auto srcExtent = getExtent( m_picking->getImageView() );
#else
		auto srcExtent = getExtent( intermediateBarrierView.viewId );
#endif
		auto dstExtent = targetExtent;
		VkOffset3D srcOffset{};
		if ( srcExtent.width > dstExtent.width )
			srcOffset.x = int32_t( srcExtent.width - dstExtent.width ) / 2;
		if ( srcExtent.height > dstExtent.height )
			srcOffset.y = int32_t( srcExtent.height - dstExtent.height ) / 2;

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
				, { getImageAspectFlags( subresourceRange.aspectMask )
					, mipLevel
					, subresourceRange.baseArrayLayer
					, subresourceRange.layerCount }
				, srcOffset
				, makeVkExtent3D( dstExtent ) }
			, srcImage
			, m_snapshotBuffer->getBuffer() );
		commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_snapshotBuffer->getBuffer().makeHostRead() );

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
		commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, makeLayoutTransition( srcImage
				, m_picking->getImageView().data->info.subresourceRange
				, ImageLayout::eShaderReadOnly
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );
#else
		if ( intermediate.layout != ImageLayout::eShaderReadOnly
			&& intermediate.layout != ImageLayout::eUndefined )
		{
			commands.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, getPipelineStageFlags( getStageMask( intermediateBarrierView.layout ) )
				, makeLayoutTransition( srcImage
					, intermediateBarrierView.viewId.data->info.subresourceRange
					, ImageLayout::eShaderReadOnly
					, intermediateBarrierView.layout
					, VK_QUEUE_FAMILY_IGNORED
					, VK_QUEUE_FAMILY_IGNORED ) );
		}
#endif

		commands.endDebugBlock();
		commands.end();
	}

	void RenderWindow::doWaitFrame( QueueData const & queueData
		, SemaphoreWaitArray const & toWait
		, uint32_t intermediateImageIndex )
	{
		ashes::VkSemaphoreArray semaphores;
		ashes::VkPipelineStageFlagsArray stages;
		crg::convert( toWait, semaphores, stages );

		if ( m_toSave && intermediateImageIndex != ~0u )
		{
			auto const & intermediateCommands = m_intermediates[intermediateImageIndex];
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			m_savedFormat = m_picking->getImageView().data->info.format;
#else
			m_savedFormat = getFormat( intermediateCommands.intermediateView.viewId );
#endif
			doInitialiseTransferCommands( queueData, intermediateImageIndex );
			auto const & transferCommands = intermediateCommands.transferCommands;
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

	void RenderWindow::doSubmitFrame( QueueData const & queueData
		, RenderingResources const * resources
		, SemaphoreWaitArray const & toWait
		, uint32_t intermediateImageIndex )
	{
		ashes::VkSemaphoreArray semaphores;
		ashes::VkPipelineStageFlagsArray stages;
		crg::convert( toWait, semaphores, stages );
		doRecordCommandBuffer( intermediateImageIndex );

#if !C3D_DebugPicking && !C3D_DebugBackgroundPicking
		if ( getEngine()->areDebugTargetsEnabled() && m_texture3Dto2D )
		{
			m_texture3Dto2D->render( *queueData.queue
				, semaphores
				, stages );
		}
#endif
		auto const & intermediateCommands = m_intermediates[intermediateImageIndex];

		if ( m_toSave )
		{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking
			m_savedFormat = m_picking->getImageView().data->info.format;
#else
			m_savedFormat = getFormat( intermediateCommands.intermediateView.viewId );
#endif
			doInitialiseTransferCommands( queueData, intermediateImageIndex );
			auto const & transferCommands = intermediateCommands.transferCommands;
			queueData.queue->submit( ashes::VkCommandBufferArray{ *transferCommands.commandBuffer }
				, semaphores
				, stages
				, ashes::VkSemaphoreArray{ *transferCommands.semaphore } );
			semaphores = { *transferCommands.semaphore };
			stages = { VK_PIPELINE_STAGE_TRANSFER_BIT };
		}

		semaphores.push_back( *resources->imageAvailableSemaphore );
		stages.push_back( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		queueData.queue->submit( ashes::VkCommandBufferArray{ *intermediateCommands.commandBuffers[resources->imageIndex] }
			, semaphores
			, stages
			, ashes::VkSemaphoreArray{}
			, *resources->fence );
	}

	void RenderWindow::doPresentFrame( QueueData const & queueData
		, RenderingResources * resources
		, uint32_t intermediateImageIndex
		, Size const & displaySize )
	{
		try
		{
			resources->fence->wait( ashes::MaxTimeout );
			resources->fence->reset();
			queueData.queue->present( *m_swapChain
				, resources->imageIndex );

			if ( m_toSave )
			{
				auto const & intermediateCommands = m_intermediates[intermediateImageIndex];
				auto const & intermediate = intermediateCommands.intermediateBarrierView;
				auto srcExtent = getExtent( intermediate.viewId );
				auto dstExtent = makeExtent2D( displaySize );
				dstExtent.width = std::min( dstExtent.width, srcExtent.width );
				dstExtent.height = std::min( dstExtent.height, srcExtent.height );
				auto subresourceRange = intermediate.viewId.data->info.subresourceRange;
				auto mipLevel = subresourceRange.baseMipLevel;
				dstExtent.width = std::max( 1u, dstExtent.width >> mipLevel );
				dstExtent.height = std::max( 1u, dstExtent.height >> mipLevel );
				m_saveBuffer = PxBufferBase::create( makeSize( dstExtent )
					, getFormat( intermediate.viewId )
					, m_snapshotData.data()
					, PixelFormat( m_savedFormat )
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
#if defined( VK_EXT_device_fault ) && C3D_SupportDeviceFault
		auto faultInfo = m_device->getDeviceFaultInfo();
		log::error << "Device lost error: " << faultInfo.description << "\n";

		if ( !faultInfo.addressInfos.empty() )
		{
			log::error << "  Addresses: \n";

			for ( auto const & info : faultInfo.addressInfos )
				log::error << cuT( "    From 0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << ( info.reportedAddress & ~( info.addressPrecision - 1 ) )
					<< cuT( " to 0x" ) << std::hex << std::setw( 8u ) << ( info.reportedAddress | ( info.addressPrecision - 1 ) )
					<< cuT( ": " ) << rendwndw::getAddressTypeName( info.addressType ) << cuT( "\n" );
		}

		if ( !faultInfo.vendorInfos.empty() )
		{
			log::error << "  Vendor Infos: \n";
			for ( auto const & info : faultInfo.vendorInfos )
				log::error << cuT( "    " ) << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << info.vendorFaultCode
					<< cuT( ": " ) << info.description << "\n";
		}
#endif
	}

	String getPrefix( WindowContext const & context )
	{
		return getPrefix( *context.root );
	}

	Engine * getEngine( WindowContext const & context )
	{
		return getEngine( *context.root );
	}
}
