#include "Castor3D/Render/RenderWindow.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/ListenerCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/PickingPass.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Core/SwapChainCreateInfo.hpp>
#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Design/BlockGuard.hpp>

#define C3D_DebugBackgroundPicking 0

namespace castor3d
{
	namespace
	{
		castor::Position convertToTopDown( castor::Position const & position
			, castor::Size const & size )
		{
			return
			{
				position.x(),
				int32_t( size.getHeight() - position.y() )
			};
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
					result = formats[0];
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
	}

	RenderWindow::TextWriter::TextWriter( castor::String const & tabs )
		: castor::TextWriter< RenderWindow >{ tabs }
	{
	}

	bool RenderWindow::TextWriter::operator()( RenderWindow const & window, castor::TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing Window " ) << window.getName() << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "window \"" ) + window.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow name" );

		if ( result )
		{
			result = file.print( 256, cuT( "%s\tvsync %s\n" ), m_tabs.c_str(), window.isVSyncEnabled() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow vsync" );
		}

		if ( result )
		{
			result = file.print( 256, cuT( "%s\tfullscreen %s\n" ), m_tabs.c_str(), window.isFullscreen() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow fullscreen" );
		}

		if ( result && window.getRenderTarget() )
		{
			result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *window.getRenderTarget(), file );
		}

		file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( castor::String const & name
		, Engine & engine )
		: OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, MouseEventHandler{}
		, m_index{ s_nbRenderWindows++ }
		, m_listener{ engine.getFrameListenerCache().add( getName() + castor::string::toString( m_index ) ) }
	{
		log::debug << "Created render window " << m_index << std::endl;
	}

	RenderWindow::~RenderWindow()
	{
		log::debug << "Destroyed render window " << m_index << std::endl;
		auto & engine = *getEngine();
		auto listener = getListener();
		engine.getFrameListenerCache().remove( getName() + castor::string::toString( m_index ) );
		auto target = m_renderTarget.lock();

		if ( target )
		{
			engine.getRenderTargetCache().remove( target );
		}

		m_device.reset();
	}

	void RenderWindow::doCreateSwapchain()
	{
		m_swapChain = getDevice()->createSwapChain( getSwapChainCreateInfo( *m_surface
			, { m_size.getWidth(), m_size.getHeight() } ) );
		m_swapChainImages = m_swapChain->getImages();
		m_commandPool = getDevice()->createCommandPool( getDevice().getGraphicsQueueFamilyIndex()
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT );
	}

	bool RenderWindow::initialise( castor::Size const & size
		, ashes::WindowHandle handle )
	{
		m_size = size;

		if ( handle )
		{
			auto & engine = *getEngine();
			engine.getRenderLoop().createDevice( std::move( handle ), *this );
			m_initialised = m_device != nullptr;

			if ( m_initialised )
			{
				m_surface = m_device->surface.get();
				auto guard = castor::makeBlockGuard(
					[this, &engine]()
					{
						engine.getRenderSystem()->setCurrentRenderDevice( m_device.get() );
					},
					[this, &engine]()
					{
						engine.getRenderSystem()->setCurrentRenderDevice( nullptr );
					} );
				engine.getMaterialCache().initialise( *m_device, engine.getMaterialsType() );
				doCreateSwapchain();
				doCreateRenderPass();
				RenderTargetSPtr target = getRenderTarget();

				if ( !target )
				{
					CU_Exception( "No render target for render window." );
				}

				target->initialise( *m_device );
				m_pickingPass = std::make_shared< PickingPass >( *getEngine()
					, target->getTechnique()->getMatrixUbo()
					, target->getCuller() );
				m_pickingPass->initialise( *m_device, target->getSize() );

				doCreateSwapChainDependent();
				doPrepareFrames();

				m_saveBuffer = castor::PxBufferBase::create( target->getSize(), convert( target->getPixelFormat() ) );
				m_stagingTexture = getDevice()->createStagingTexture( target->getPixelFormat()
					, { m_saveBuffer->getWidth(), m_saveBuffer->getHeight() } );
				m_initialised = true;
				m_dirty = false;
				engine.registerWindow( *this );
			}
		}

		return m_initialised;
	}

	void RenderWindow::cleanup()
	{
		m_initialised = false;

		if ( m_device )
		{
			auto & engine = *getEngine();
			engine.unregisterWindow( *this );
			bool hasCurrent = engine.getRenderSystem()->hasCurrentRenderDevice();

			if ( hasCurrent
				&& &engine.getRenderSystem()->getCurrentRenderDevice() != m_device.get() )
			{
				auto & device = engine.getRenderSystem()->getCurrentRenderDevice();
				auto guard = castor::makeBlockGuard(
					[this, &engine]()
					{
						engine.getRenderSystem()->setCurrentRenderDevice( nullptr );
					},
					[this, &engine, &device]()
					{
						engine.getRenderSystem()->setCurrentRenderDevice( &device );
					} );
				doCleanup( true );
			}
			else
			{
				doCleanup( !hasCurrent );
			}
		}
	}

	void RenderWindow::render( bool waitOnly )
	{
		if ( m_initialised )
		{
			RenderTargetSPtr target = getRenderTarget();

			if ( target && target->isInitialised() )
			{
				auto & engine = *getEngine();
				auto & renderSystem = *engine.getRenderSystem();
				auto guard = castor::makeBlockGuard(
					[this, &renderSystem]()
					{
						renderSystem.setCurrentRenderDevice( m_device.get() );
					},
					[this, &renderSystem]()
					{
						renderSystem.setCurrentRenderDevice( nullptr );
					} );

				if ( m_toSave )
				{
					castor::ByteArray data;
					m_stagingTexture->downloadTextureData( *getDevice().transferQueue
						, *getDevice().graphicsCommandPool
						, target->getPixelFormat()
						, m_saveBuffer->getPtr()
						, target->getTexture().getTexture()->getDefaultView().getTargetView() );
					auto texture = target->getTexture().getTexture();
					m_toSave = false;
				}

#if C3D_DebugPicking || C3D_DebugBackgroundPicking
				m_pickingPass->pick( *m_device
					, convertToTopDown( m_mousePosition, m_size )
					, *target->getCamera() );
#endif

				if ( waitOnly )
				{
					waitFrame();
				}
				else if ( auto resources = getResources() )
				{
					try
					{
						submitFrame( resources );
						presentFrame( resources );
					}
					catch ( ashes::Exception & exc )
					{
						std::cerr << "Can't render: " << exc.what() << std::endl;

						if ( exc.getResult() == VK_ERROR_DEVICE_LOST )
						{
							m_initialised = false;
						}
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

		if ( m_initialised && !m_dirty )
		{
			m_dirty = true;
			getListener()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this]( RenderDevice const & device )
				{
					doResetSwapChain();
					m_dirty = false;
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
		ViewportType result = ViewportType( -1 );
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

	void RenderWindow::setPixelFormat( VkFormat value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setPixelFormat( value );
		}
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
			m_pickingPass->addScene( scene, *camera );
		}
	}

	PickNodeType RenderWindow::pick( castor::Position const & position )
	{
#if C3D_DebugPicking || C3D_DebugBackgroundPicking

		return m_pickingPass->getPickedNodeType();


#else
		PickNodeType result = PickNodeType::eNone;
		auto camera = getCamera();

		if ( camera )
		{
			result = m_pickingPass->pick( *m_device
				, convertToTopDown( position, m_size )
				, *camera );
		}

		return result;

#endif
	}

	GeometrySPtr RenderWindow::getPickedGeometry()const
	{
		return m_pickingPass->getPickedGeometry();
	}

	BillboardBaseSPtr RenderWindow::getPickedBillboard()const
	{
		return m_pickingPass->getPickedBillboard();
	}

	SubmeshSPtr RenderWindow::getPickedSubmesh()const
	{
		return m_pickingPass->getPickedSubmesh();
	}

	uint32_t RenderWindow::getPickedFace()const
	{
		return m_pickingPass->getPickedFace();
	}

	void RenderWindow::doCreateRenderingResources()
	{
		for ( uint32_t i = 0u; i < uint32_t( m_swapChainImages.size() ); ++i )
		{
			m_renderingResources.emplace_back( std::make_unique< RenderingResources >( getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "ImageAvailable" )
				, getDevice()->createSemaphore( getName() + castor::string::toString( i ) + "FinishedRendering" )
				, getDevice()->createFence( getName() + castor::string::toString( i ), VK_FENCE_CREATE_SIGNALED_BIT )
				, getDevice().graphicsCommandPool->createCommandBuffer( getName() + castor::string::toString( i ) )
				, 0u ) );
		}
	}

	void RenderWindow::doCreateRenderPass()
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_swapChain->getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
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
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = getDevice()->createRenderPass( "RenderPass"
			, std::move( createInfo ) );
	}

	void RenderWindow::doCreateFrameBuffers()
	{
		m_frameBuffers.resize( m_swapChainImages.size() );

		for ( size_t i = 0u; i < m_frameBuffers.size(); ++i )
		{
			auto attaches = doPrepareAttaches( uint32_t( i ) );
			m_frameBuffers[i] = m_renderPass->createFrameBuffer( "RenderPass"
				, m_swapChain->getDimensions()
				, std::move( attaches ) );
		}
	}

	void RenderWindow::doCreateCommandBuffers()
	{
		m_commandBuffers.resize( m_swapChainImages.size() );
		uint32_t index = 0u;

		for ( auto & commandBuffer : m_commandBuffers )
		{
			commandBuffer = m_commandPool->createCommandBuffer( getName() + castor::string::toString( index++ ) );
		}
	}

	ashes::ImageViewCRefArray RenderWindow::doPrepareAttaches( uint32_t backBuffer )
	{
		ashes::ImageViewCRefArray attaches;
		m_views.clear();

		for ( auto & attach : m_renderPass->getAttachments() )
		{
			m_views.push_back( m_swapChainImages[backBuffer].createView( makeVkType< VkImageViewCreateInfo >( 0u
				, m_swapChainImages[backBuffer]
				, VK_IMAGE_VIEW_TYPE_2D
				, m_swapChain->getFormat()
				, VkComponentMapping{}
				, VkImageSubresourceRange
				{
					ashes::getAspectMask( m_swapChain->getFormat() ),
					0u,
					1u,
					0u,
					1u,
				} ) ) );
			attaches.emplace_back( m_views.back() );
		}

		return attaches;
	}

	void RenderWindow::doCreateProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "RenderWindow" };
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

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "RenderWindow" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapResult = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapResult", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = vec4( texture( c3d_mapResult, vtx_texture ).xyz(), 1.0_f );
				} );
			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_program = ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( getDevice(), vtx ),
			makeShaderState( getDevice(), pxl ),
		};
	}

	void RenderWindow::doCreateSwapChainDependent()
	{
		doCreateRenderingResources();
		m_renderQuad = RenderQuadBuilder{}
			.texcoordConfig( rq::Texcoord{} )
			.build( *m_device
				, "RenderWindow" + getName()
				, VK_FILTER_LINEAR );
		doCreateProgram();
		m_renderQuad->createPipelineAndPass( VkExtent2D{ m_size[0], m_size[1] }
			, castor::Position{}
			, m_program
			, *m_renderPass
			, {
#if C3D_DebugPicking
				RenderQuad::makeDescriptorWrite( m_pickingPass->getResult()
#else
				RenderQuad::makeDescriptorWrite( getRenderTarget()->getTexture().getTexture()->getDefaultView().getSampledView()
#endif
					, m_renderQuad->getSampler().getSampler()
					, 0u ),
			} );
	}

	bool RenderWindow::doPrepareFrames()
	{
		bool result{ true };
		doCreateCommandBuffers();
		doCreateFrameBuffers();

		for ( uint32_t i = 0u; i < m_commandBuffers.size() && result; ++i )
		{
			auto & frameBuffer = *m_frameBuffers[i];
			auto & commandBuffer = *m_commandBuffers[i];

			commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
			commandBuffer.beginDebugBlock(
				{
					"RenderWindow Render",
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			commandBuffer.beginRenderPass( *m_renderPass
				, frameBuffer
				, { opaqueWhiteClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_renderQuad->registerPass( commandBuffer );
			commandBuffer.endRenderPass();
			commandBuffer.endDebugBlock();
			commandBuffer.end();
		}

		return result;
	}

	void RenderWindow::doResetSwapChain()
	{
		getDevice()->waitIdle();
		m_renderQuad.reset();
		m_frameBuffers.clear();
		m_commandBuffers.clear();
		m_renderingResources.clear();
		m_swapChainImages.clear();
		m_swapChain.reset();
		doCreateSwapchain();
		doCreateSwapChainDependent();
		doPrepareFrames();
	}

	RenderWindow::RenderingResources * RenderWindow::getResources()
	{
		auto & resources = *m_renderingResources[m_resourceIndex];
		uint32_t imageIndex{ 0u };
		auto res = m_swapChain->acquireNextImage( ashes::MaxTimeout
			, *resources.imageAvailableSemaphore
			, imageIndex );

		if ( doCheckNeedReset( VkResult( res )
			, true
			, "Swap chain image acquisition" ) )
		{
			m_resourceIndex = ( m_resourceIndex + 1 ) % m_renderingResources.size();
			resources.imageIndex = imageIndex;
			return &resources;
		}

		return nullptr;
	}

	void RenderWindow::waitFrame()
	{
		getDevice().graphicsQueue->submit( {}
			, { getRenderTarget()->getSemaphore() }
			, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
			, {}
			, nullptr );
	}

	void RenderWindow::submitFrame( RenderingResources * resources )
	{
		RenderTargetSPtr target = getRenderTarget();
		resources->fence->reset();
		getDevice().graphicsQueue->submit( { *m_commandBuffers[resources->imageIndex] }
			, { *resources->imageAvailableSemaphore, target->getSemaphore() }
			, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
			, {}
			, resources->fence.get() );
		resources->fence->wait( ashes::MaxTimeout );
	}

	void RenderWindow::presentFrame( RenderingResources * resources )
	{
		try
		{
			getDevice().graphicsQueue->present( *m_swapChain
				, resources->imageIndex );
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
			break;
		}

		return result;
	}

	void RenderWindow::doCleanup( bool enableDevice )
	{
		auto & engine = *getEngine();

		if ( enableDevice )
		{
			engine.getRenderSystem()->setCurrentRenderDevice( m_device.get() );
		}

		getDevice()->waitIdle();

		if ( m_pickingPass )
		{
			m_pickingPass->cleanup( *m_device );
			m_pickingPass.reset();
		}

		m_overlayRenderer.reset();
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->cleanup( *m_device );
		}

		m_renderQuad.reset();
		m_program.clear();
		m_commandBuffers.clear();
		m_renderingResources.clear();
		m_commandPool.reset();
		m_frameBuffers.clear();
		m_stagingTexture.reset();
		m_renderPass.reset();
		m_swapChainImages.clear();
		m_swapChain.reset();

		if ( enableDevice )
		{
			engine.getRenderSystem()->setCurrentRenderDevice( nullptr );
		}
	}

	void RenderWindow::doProcessMouseEvent( MouseEventSPtr event )
	{
		m_mousePosition = event->getPosition();
	}
}
