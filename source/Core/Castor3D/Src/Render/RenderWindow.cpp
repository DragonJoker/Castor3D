#include "RenderWindow.hpp"

#include "Render/RenderLoop.hpp"
#include "Render/RenderTarget.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
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
			result = file.print( 256, cuT( "%s\tvsync %s\n" ), m_tabs.c_str(), window.getVSync() ? cuT( "true" ) : cuT( "false" ) ) > 0;
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
		, m_pickingPass{ std::make_unique< PickingPass >( engine ) }
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
				m_swapChain = m_device->createSwapChain( Point2ui{ size.getWidth(), size.getHeight() } );
				SceneSPtr scene = getScene();
				m_swapChain->setClearColour( RgbaColour::fromRGBA( toRGBAFloat( scene->getBackgroundColour() ) ) );
				m_swapChainReset = m_swapChain->onReset.connect( [this]()
				{
					doResetSwapChain();
				} );

				RenderTargetSPtr target = getRenderTarget();

				if ( !target )
				{
					CASTOR_EXCEPTION( "No render target for render window." );
				}

				std::vector< renderer::PixelFormat > formats{ { m_swapChain->getFormat() } };
				renderer::RenderSubpassPtrArray subpasses;
				subpasses.emplace_back( m_device->createRenderSubpass( formats
					, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite } ) );
				m_renderPass = m_device->createRenderPass( formats
					, std::move( subpasses )
					, renderer::RenderPassState{ renderer::PipelineStageFlag::eBottomOfPipe
						, renderer::AccessFlag::eMemoryRead
						, { renderer::ImageLayout::ePresentSrc } }
					, renderer::RenderPassState{ renderer::PipelineStageFlag::eBottomOfPipe
						, renderer::AccessFlag::eMemoryRead
						, { renderer::ImageLayout::ePresentSrc } } );

				doCreateSwapChainDependent();
				doPrepareFrames();

				m_transferCommandBuffer = m_device->getGraphicsCommandPool().createCommandBuffer();
				target->initialise( 1 );
				m_saveBuffer = PxBufferBase::create( target->getSize(), target->getPixelFormat() );
				m_stagingBuffer = std::make_shared< renderer::StagingBuffer >( *m_device
					, renderer::BufferTarget::eTransferDst
					, m_saveBuffer->size() );
				m_pickingPass->initialise( target->getSize() );
				m_device->disable();
				m_initialised = true;
			}
		}

		return m_initialised;
	}

	void RenderWindow::cleanup()
	{
		m_initialised = false;

		if ( m_device )
		{
			if ( getEngine()->getRenderSystem()->hasCurrentDevice()
				&& getEngine()->getRenderSystem()->getCurrentDevice() != m_device.get() )
			{
				auto device = getEngine()->getRenderSystem()->getCurrentDevice();
				device->disable();
				m_device->enable();

				m_pickingPass->cleanup();
				RenderTargetSPtr target = getRenderTarget();

				if ( target )
				{
					target->cleanup();
				}

				m_device->disable();
			}
			else
			{
				m_pickingPass->cleanup();
				RenderTargetSPtr target = getRenderTarget();

				if ( target )
				{
					target->cleanup();
				}
			}

			getEngine()->getRenderSystem()->unregisterDevice( *m_device );
			m_device.reset();
		}
	}

	void RenderWindow::render( bool force )
	{
		if ( m_initialised )
		{
			Engine * engine = getEngine();
			RenderTargetSPtr target = getRenderTarget();
			m_device->enable();

			if ( target && target->isInitialised() )
			{
				if ( m_toSave )
				{
					ByteArray data;
					m_stagingBuffer->downloadTextureData( *m_transferCommandBuffer
						, m_saveBuffer->ptr()
						, m_saveBuffer->size()
						, target->getTexture().getTexture()->getView() );
					auto texture = target->getTexture().getTexture();
					m_toSave = false;
				}

				auto resources = m_swapChain->getResources();

				if ( resources )
				{
					auto before = std::chrono::high_resolution_clock::now();
					auto & queue = m_device->getGraphicsQueue();
					auto res = queue.submit( *m_commandBuffers[resources->getBackBuffer()]
						, resources->getImageAvailableSemaphore()
						, renderer::PipelineStageFlag::eColourAttachmentOutput
						, resources->getRenderingFinishedSemaphore()
						, &resources->getFence() );
					m_swapChain->present( *resources );

					auto after = std::chrono::high_resolution_clock::now();
				}
				else
				{
					std::cerr << "Can't render" << std::endl;
				}
			}

			m_device->disable();
		}
	}

	void RenderWindow::resize( uint32_t x, uint32_t y )
	{
		resize( { x, y } );
	}

	void RenderWindow::resize( Size const & size )
	{
		m_size = size;

		if ( m_initialised )
		{
			getListener()->postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_device->waitIdle();
				m_swapChain->reset( Point2ui{ m_size.getWidth(), m_size.getHeight() } );
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

	PixelFormat RenderWindow::getPixelFormat()const
	{
		PixelFormat result = PixelFormat( -1 );
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			result = target->getPixelFormat();
		}

		return result;
	}

	void RenderWindow::setPixelFormat( PixelFormat value )
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

	void RenderWindow::doCreateProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer, 0 );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_projection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		m_program = &cache.getNewProgram( false );
		m_program->createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		m_program->createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
	}

	void RenderWindow::doCreateSwapChainDependent()
	{
		RenderTargetSPtr target = getRenderTarget();
		m_renderQuad = std::make_unique< RenderQuad >( *getEngine()->getRenderSystem()
			, castor::Position{}
			, m_size
			, *m_program
			, target->getTexture().getTexture()->getView()
			, *m_renderPass
			, false
			, false );
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
				m_swapChain->preRenderCommands( i, commandBuffer );
				commandBuffer.beginRenderPass( *m_renderPass
					, frameBuffer
					, { m_swapChain->getClearColour() }
					, renderer::SubpassContents::eInline );
				m_renderQuad->registerFrame( commandBuffer );
				commandBuffer.endRenderPass();
				m_swapChain->postRenderCommands( i, commandBuffer );

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
}
