#include "RenderLoopAsync.hpp"

#include "Engine.hpp"

#include <Miscellaneous/PreciseTimer.hpp>
#include <Design/ScopeGuard.hpp>

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	static const char * CALL_RENDER_SYNC_FRAME = "Can't call renderSyncFrame in threaded render loop";
	static const char * CALL_PAUSE_RENDERING = "Can't call Pause on a paused render loop";
	static const char * CALL_RESUME_RENDERING = "Can't call Resume on a non paused render loop";

	RenderLoopAsync::RenderLoopAsync( Engine & engine, uint32_t p_wantedFPS )
		: RenderLoop( engine, p_wantedFPS, true )
		, m_mainLoopThread( nullptr )
		, m_paused( false )
		, m_ended( false )
		, m_rendering( false )
		, m_createContext( false )
		, m_created( false )
		, m_interrupted( false )
	{
		m_mainLoopThread.reset( new std::thread( std::bind( &RenderLoopAsync::doMainLoop, this ) ) );
	}

	RenderLoopAsync::~RenderLoopAsync()
	{
		// We wait for the main loop to end (it makes a final render to clean the render system)
		m_interrupted = true;
		m_mainLoopThread->join();
		m_mainLoopThread.reset();
	}

	bool RenderLoopAsync::isCreated()const
	{
		return m_created;
	}

	bool RenderLoopAsync::isRendering()const
	{
		return m_rendering;
	}

	bool RenderLoopAsync::isInterrupted()const
	{
		return m_interrupted;
	}

	bool RenderLoopAsync::isEnded()const
	{
		return m_ended;
	}

	bool RenderLoopAsync::isPaused()const
	{
		return m_paused;
	}

	void RenderLoopAsync::updateVSync( bool p_enable )
	{
		using type = decltype( m_savedTime );
		static auto zero = type::zero();

		if ( p_enable && m_savedTime != zero )
		{
			m_frameTime = m_savedTime;
			m_savedTime = zero;
		}
		else if ( m_savedTime == zero )
		{
			m_savedTime = m_frameTime;
			m_frameTime = type{ 1 };
		}
	}

	void RenderLoopAsync::beginRendering()
	{
		m_rendering = true;
	}

	void RenderLoopAsync::renderSyncFrame()
	{
		if ( !m_paused )
		{
			CASTOR_EXCEPTION( CALL_RENDER_SYNC_FRAME );
		}

		if ( m_rendering )
		{
			// trick pour éviter les problèmes de contexte dans des threads différents:
			// On enlève la pause le temps de dessiner une frame.
			m_frameEnded = false;
			m_paused = false;

			while ( !m_frameEnded )
			{
				System::sleep( 1 );
			}

			m_paused = true;
		}
	}

	void RenderLoopAsync::pause()
	{
		if ( m_paused )
		{
			CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
		}

		m_paused = true;

		while ( !m_frameEnded )
		{
			System::sleep( 5 );
		}
	}

	void RenderLoopAsync::resume()
	{
		if ( !m_paused )
		{
			CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
		}

		m_paused = false;
	}

	void RenderLoopAsync::endRendering()
	{
		m_rendering = false;

		while ( !isEnded() )
		{
			System::sleep( 5 );
		}
	}

	ContextSPtr RenderLoopAsync::doCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr result;

		if ( !m_createContext )
		{
			doSetWindow( &p_window );
			m_createContext = true;

			while ( !isInterrupted() && !isCreated() )
			{
				System::sleep( 5 );
			}

			m_createContext = false;
			doSetWindow( nullptr );
			result = m_renderSystem.getMainContext();
		}

		return result;
	}

	void RenderLoopAsync::doMainLoop()
	{
		PreciseTimer timer;
		m_frameEnded = true;
		auto scopeGuard{ makeScopeGuard( [this]()
		{
			cleanup();
			m_renderSystem.cleanup();
		} ) };

		try
		{
			// Tant qu'on n'est pas interrompu, on continue
			while ( !isInterrupted() )
			{
				// Tant qu'on n'a pas de contexte principal et qu'on ne nous a pas demandé de le créer, on attend.
				while ( !isInterrupted() && !m_createContext && !isCreated() )
				{
					System::sleep( 10 );
				}

				if ( !isInterrupted() && m_createContext && !isCreated() )
				{
					// On nous a demandé de créer le contexte principal, on le crée
					ContextSPtr context = doCreateContext( *doGetWindow() );

					if ( context )
					{
						m_renderSystem.setMainContext( context );
						m_created = true;
					}
					else
					{
						m_interrupted = true;
					}
				}

				// Tant qu'on n'a pas demandé le début du rendu, on attend.
				while ( !isInterrupted() && !isRendering() )
				{
					System::sleep( 10 );
				}

				// Le rendu est en cours
				while ( !isInterrupted() && isRendering() && !isPaused() )
				{
					m_frameEnded = false;
					timer.getElapsed();
					doRenderFrame();
					auto endTime = std::chrono::duration_cast< Milliseconds >( timer.getElapsed() );
					m_frameEnded = true;
					std::this_thread::sleep_for( std::max( 0_ms, getFrameTime() - endTime ) );
				}

				m_ended = true;
			}
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( cuT( "RenderLoop - " ) + p_exc.getFullDescription() );
			m_frameEnded = true;
			m_ended = true;
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( std::string( "RenderLoop - " ) + p_exc.what() );
			m_frameEnded = true;
			m_ended = true;
		}
	}

	void RenderLoopAsync::doSetWindow( RenderWindow * p_window )
	{
		auto lock = makeUniqueLock( m_mutexWindow );
		m_window = p_window;
	}

	RenderWindow * RenderLoopAsync::doGetWindow()const
	{
		auto lock = makeUniqueLock( m_mutexWindow );
		return m_window;
	}
}
