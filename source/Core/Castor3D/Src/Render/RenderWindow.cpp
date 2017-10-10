#include "RenderWindow.hpp"

#include "FrameBuffer/BackBuffers.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderTarget.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	RenderWindow::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< RenderWindow >{ p_tabs }
	{
	}

	bool RenderWindow::TextWriter::operator()( RenderWindow const & p_window, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Window " ) + p_window.getName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "window \"" ) + p_window.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow name" );

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tvsync %s\n" ), m_tabs.c_str(), p_window.getVSync() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow vsync" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tfullscreen %s\n" ), m_tabs.c_str(), p_window.isFullscreen() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			castor::TextWriter< RenderWindow >::checkError( result, "RenderWindow fullscreen" );
		}

		if ( result && p_window.getRenderTarget() )
		{
			result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_window.getRenderTarget(), p_file );
		}

		p_file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( String const & p_name, Engine & engine )
		: OwnedBy< Engine >{ engine }
		, Named{ p_name }
		, m_index{ s_nbRenderWindows++ }
		, m_wpListener{ engine.getFrameListenerCache().add( cuT( "RenderWindow_" ) + string::toString( m_index ) ) }
		, m_backBuffers{ engine.getRenderSystem()->createBackBuffers() }
		, m_pickingPass{ std::make_unique< PickingPass >( engine ) }
	{
	}

	RenderWindow::~RenderWindow()
	{
		FrameListenerSPtr pListener( m_wpListener.lock() );
		getEngine()->getFrameListenerCache().remove( cuT( "RenderWindow_" ) + string::toString( m_index ) );
		auto target = m_renderTarget.lock();

		if ( target )
		{
			getEngine()->getRenderTargetCache().remove( target );
		}

		m_pickingPass.reset();
	}

	bool RenderWindow::initialise( Size const & p_size, WindowHandle const & p_handle )
	{
		m_size = p_size;
		m_handle = p_handle;

		if ( m_handle )
		{
			getEngine()->getRenderLoop().createContext( *this );
			m_initialised = m_context && m_context->isInitialised();

			if ( m_initialised )
			{
				m_context->setCurrent();
				m_backBuffers->initialise( getSize(), getPixelFormat() );

				SceneSPtr scene = getScene();
				RenderTargetSPtr target = getRenderTarget();

				if ( scene )
				{
					m_backBuffers->setClearColour( scene->getBackgroundColour() );
				}
				else
				{
					m_backBuffers->setClearColour( Colour::fromComponents( 0.5, 0.5, 0.5, 1.0 ) );
				}

				if ( target )
				{
					target->initialise( 1 );
					m_saveBuffer = PxBufferBase::create( target->getSize(), target->getPixelFormat() );
				}

				m_pickingPass->initialise( target->getSize() );
				m_context->endCurrent();
				m_initialised = true;
			}
		}

		return m_initialised;
	}

	void RenderWindow::cleanup()
	{
		m_initialised = false;

		if ( m_context )
		{
			auto context = getEngine()->getRenderSystem()->getCurrentContext();

			if ( context != m_context.get() )
			{
				m_context->setCurrent();
			}

			m_pickingPass->cleanup();
			RenderTargetSPtr target = getRenderTarget();

			if ( target )
			{
				target->cleanup();
			}

			if ( context != m_context.get() )
			{
				m_context->endCurrent();
			}

			if ( m_context != getEngine()->getRenderSystem()->getMainContext() )
			{
				m_context->cleanup();
			}

			if ( context && context != m_context.get() )
			{
				context->setCurrent();
			}

			m_context.reset();
		}
	}

	void RenderWindow::render( bool p_bForce )
	{
		if ( m_initialised )
		{
			Engine * engine = getEngine();
			RenderTargetSPtr target = getRenderTarget();
			m_context->setCurrent();

			if ( target && target->isInitialised() )
			{
				if ( m_toSave )
				{
					auto texture = target->getTexture().getTexture();
					auto buffer = texture->lock( AccessType::eRead );

					if ( buffer )
					{
						std::memcpy( m_saveBuffer->ptr(), buffer, m_saveBuffer->size() );
					}

					texture->unlock( false );
					m_toSave = false;
				}

				if ( isUsingStereo() && abs( getIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && engine->getRenderSystem()->getGpuInformations().isStereoAvailable() )
				{
					//doRender( WindowBuffer::eBackLeft, target->getTextureLEye() );
					//doRender( WindowBuffer::eBackRight, target->getTextureREye() );
				}
				else
				{
					doRender( WindowBuffer::eBack, target->getTexture() );
				}
			}

			m_context->endCurrent();
			m_context->swapBuffers();
		}
	}

	void RenderWindow::resize( int x, int y )
	{
		resize( Size( x, y ) );
	}

	void RenderWindow::resize( Size const & p_size )
	{
		m_size = p_size;

		if ( m_initialised )
		{
			m_wpListener.lock()->postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				doUpdateSize();
			} ) );
		}
	}

	void RenderWindow::setCamera( CameraSPtr p_pCamera )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setCamera( p_pCamera );
		}
	}

	void RenderWindow::updateFullScreen( bool p_value )
	{
		m_bFullscreen = p_value;
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

	void RenderWindow::setViewportType( ViewportType val )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setViewportType( val );
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

	void RenderWindow::setPixelFormat( PixelFormat val )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setPixelFormat( val );
		}
	}

	void RenderWindow::setScene( SceneSPtr p_scene )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			target->setScene( p_scene );
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

	void RenderWindow::setStereo( bool p_bStereo )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_target->setStereo( p_bStereo );
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

	void RenderWindow::setIntraOcularDistance( real p_rIao )
	{
		RenderTargetSPtr target = getRenderTarget();

		if ( target )
		{
			//l_target->setIntraOcularDistance( p_rIao );
		}
	}

	Size RenderWindow::getSize()const
	{
		return m_size;
	}

	void RenderWindow::doRender( WindowBuffer p_eTargetBuffer, TextureUnit const & p_texture )
	{
		auto texture = p_texture.getTexture();
		m_backBuffers->bind( p_eTargetBuffer, FrameBufferTarget::eDraw );
		m_backBuffers->clear( BufferComponent::eColour );
		m_context->renderTexture( m_size, *texture );
		m_backBuffers->unbind();
	}

	void RenderWindow::doUpdateSize()
	{
		m_backBuffers->resize( getSize() );
	}
}
