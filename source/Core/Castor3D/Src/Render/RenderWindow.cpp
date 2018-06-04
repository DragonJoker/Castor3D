#include "RenderWindow.hpp"

#include "Miscellaneous/PickingPass.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderTarget.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderWindow::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< RenderWindow >{ tabs }
	{
	}

	bool RenderWindow::TextWriter::operator()( RenderWindow const & window, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Window " ) + window.getName() );
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

	RenderWindow::RenderWindow( String const & name
		, Engine & engine )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_index{ s_nbRenderWindows++ }
		, m_listener{ engine.getFrameListenerCache().add( cuT( "RenderWindow_" ) + string::toString( m_index ) ) }
		, m_pickingPass{ std::make_shared< PickingPass >( engine ) }
	{
	}

	RenderWindow::~RenderWindow()
	{
		auto listener = getListener();
		getEngine()->getFrameListenerCache().remove( cuT( "RenderWindow_" ) + string::toString( m_index ) );
		auto target = m_renderTarget.lock();

		if ( target )
		{
			getEngine()->getRenderTargetCache().remove( target );
		}

		m_pickingPass.reset();
	}

	bool RenderWindow::initialise( Size const & size
		, renderer::WindowHandle && handle )
	{
		m_size = size;

		if ( handle )
		{
			getEngine()->getRenderLoop().createDevice( std::move( handle ), *this );
			m_initialised = m_device != nullptr;

			if ( m_initialised )
			{
				m_device->enable();
				getEngine()->getMaterialCache().initialise( getEngine()->getMaterialsType() );
				m_swapChain = m_device->createSwapChain( { size.getWidth(), size.getHeight() } );
				SceneSPtr scene = getScene();
				static renderer::ClearColorValue clear{ 1.0f, 1.0f, 0.0f, 1.0f };
				m_swapChain->setClearColour( clear );
				m_swapChainReset = m_swapChain->onReset.connect( [this]()
				{
					doResetSwapChain();
				} );

				RenderTargetSPtr target = getRenderTarget();

				if ( !target )
				{
					CASTOR_EXCEPTION( "No render target for render window." );
				}

				// Create the render pass.
				renderer::RenderPassCreateInfo renderPass;
				renderPass.flags = 0u;

				renderPass.attachments.resize( 1u );
				renderPass.attachments[0].format = m_swapChain->getFormat();
				renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
				renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
				renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
				renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
				renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
				renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
				renderPass.attachments[0].finalLayout = renderer::ImageLayout::ePresentSrc;

				renderPass.subpasses.resize( 1u );
				renderPass.subpasses[0].flags = 0u;
				renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
				renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

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

				m_renderPass = m_device->createRenderPass( renderPass );

				target->initialise();
				m_pickingPass->initialise( target->getSize() );

				doCreateSwapChainDependent();
				doPrepareFrames();

				m_transferCommandBuffer = m_device->getGraphicsCommandPool().createCommandBuffer();
				m_saveBuffer = PxBufferBase::create( target->getSize(), convert( target->getPixelFormat() ) );
				m_stagingBuffer = std::make_unique< renderer::StagingBuffer >( *m_device
					, renderer::BufferTarget::eTransferDst
					, m_saveBuffer->size() );
				m_device->disable();
				m_initialised = true;
				m_dirty = false;
			}
		}

		return m_initialised;
	}

	void RenderWindow::cleanup()
	{
		m_initialised = false;

		if ( m_device )
		{
			bool hasCurrent = getEngine()->getRenderSystem()->hasCurrentDevice();

			if ( hasCurrent
				&& &getCurrentDevice( *this ) != m_device.get() )
			{
				auto & device = getCurrentDevice( *this );
				device.disable();
				doCleanup( true );
				device.enable();
			}
			else
			{
				doCleanup( !hasCurrent );
			}

			getEngine()->getRenderSystem()->unregisterDevice( *m_device );
			m_device.reset();
		}
	}

	void RenderWindow::render( bool force )
	{
		if ( m_initialised )
		{
			RenderTargetSPtr target = getRenderTarget();

			if ( target && target->isInitialised() )
			{
				m_device->enable();

				if ( m_toSave )
				{
					ByteArray data;
					m_stagingBuffer->downloadTextureData( *m_transferCommandBuffer
						, m_saveBuffer->ptr()
						, m_saveBuffer->size()
						, target->getTexture().getTexture()->getDefaultView() );
					auto texture = target->getTexture().getTexture();
					m_toSave = false;
				}

				auto resources = m_swapChain->getResources();
#if C3D_DebugPicking
				m_pickingPass->pick( Position{}, *target->getCamera() );
#endif

				if ( resources )
				{
					auto res = m_device->getGraphicsQueue().submit( { *m_commandBuffers[resources->getBackBuffer()] }
						, { resources->getImageAvailableSemaphore(), target->getSemaphore() }
						, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::PipelineStageFlag::eColourAttachmentOutput }
						, { resources->getRenderingFinishedSemaphore() }
						, &resources->getFence() );
					m_device->waitIdle();
					m_swapChain->present( *resources );
				}
				else
				{
					std::cerr << "Can't render" << std::endl;
				}

				m_device->disable();
			}
		}
	}

	void RenderWindow::resize( uint32_t x, uint32_t y )
	{
		resize( { x, y } );
	}

	void RenderWindow::resize( Size const & size )
	{
		m_size = size;

		if ( m_initialised && !m_dirty )
		{
			m_dirty = true;
			getListener()->postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_device->waitIdle();
				m_swapChain->reset( { m_size.getWidth(), m_size.getHeight() } );
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

	renderer::Format RenderWindow::getPixelFormat()const
	{
		renderer::Format result = renderer::Format::eUndefined;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getPixelFormat();
		}

		return result;
	}

	void RenderWindow::setPixelFormat( renderer::Format value )
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

	real RenderWindow::getIntraOcularDistance()const
	{
		real result = 0;
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_result = target->getIntraOcularDistance();
		}

		return result;
	}

	void RenderWindow::setIntraOcularDistance( real value )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_target->setIntraOcularDistance( value );
		}
	}

	Size RenderWindow::getSize()const
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
		PickNodeType result = PickNodeType::eNone;
		auto camera = getCamera();

		if ( camera )
		{
			result = m_pickingPass->pick( position, *camera );
		}

		return result;
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

	void RenderWindow::doCreateProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
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
				vtx_texture = texcoord;
				out.gl_Position() = writer.rendererScalePosition( vec4( position, 0.0, 1.0 ) );
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

		m_program.clear();
		m_program.push_back( { m_device->createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		m_program.push_back( { m_device->createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		m_program[0].module->loadShader( vtx.getSource() );
		m_program[1].module->loadShader( pxl.getSource() );
	}

	void RenderWindow::doCreateSwapChainDependent()
	{
		RenderTargetSPtr target = getRenderTarget();
		m_renderQuad = std::make_unique< RenderQuad >( *getEngine()->getRenderSystem()
			, false
			, false );
		doCreateProgram();
		m_renderQuad->createPipeline( renderer::Extent2D{ m_size[0], m_size[1] }
			, castor::Position{}
			, m_program
#if C3D_DebugPicking
			, m_pickingPass->getResult()
#else
			, target->getTexture().getTexture()->getDefaultView()
#endif
			, *m_renderPass
			, {}
			, {} );
	}

	bool RenderWindow::doPrepareFrames()
	{
		bool result{ true };
		m_commandBuffers = m_swapChain->createCommandBuffers();
		m_frameBuffers = m_swapChain->createFrameBuffers( *m_renderPass );

		for ( uint32_t i = 0u; i < m_commandBuffers.size() && result; ++i )
		{
			auto & frameBuffer = *m_frameBuffers[i];
			auto & commandBuffer = *m_commandBuffers[i];

			if ( commandBuffer.begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
			{
				commandBuffer.beginRenderPass( *m_renderPass
					, frameBuffer
					, { m_swapChain->getClearColour() }
					, renderer::SubpassContents::eInline );
				m_renderQuad->registerFrame( commandBuffer );
				commandBuffer.endRenderPass();

				result = commandBuffer.end();

				if ( !result )
				{
					std::cerr << "Command buffers recording failed" << std::endl;
				}
			}
		}

		return result;
	}

	void RenderWindow::doResetSwapChain()
	{
		doCreateSwapChainDependent();
		doPrepareFrames();
	}

	void RenderWindow::doCleanup( bool enableDevice )
	{
		if ( enableDevice )
		{
			m_device->enable();
		}

		m_device->waitIdle();
		m_pickingPass->cleanup();
		m_overlayRenderer.reset();
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->cleanup();
		}

		m_renderQuad.reset();
		m_program.clear();
		m_swapChainReset.disconnect();
		m_commandBuffers.clear();
		m_frameBuffers.clear();
		m_transferCommandBuffer.reset();
		m_stagingBuffer.reset();
		m_renderPass.reset();
		m_swapChain.reset();

		if ( enableDevice )
		{
			m_device->disable();
		}
	}
}
