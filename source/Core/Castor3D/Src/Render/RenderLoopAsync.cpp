#include "RenderLoopAsync.hpp"

#include "Engine.hpp"

#include <Miscellaneous/PreciseTimer.hpp>
#include <Design/ScopeGuard.hpp>

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	static const char * CALL_RENDER_SYNC_FRAME = "Can't call RenderSyncFrame in threaded render loop";
	static const char * CALL_PAUSE_RENDERING = "Can't call Pause on a paused render loop";
	static const char * CALL_RESUME_RENDERING = "Can't call Resume on a non paused render loop";

	RenderLoopAsync::RenderLoopAsync( Engine & p_engine, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_wantedFPS, true )
		, m_mainLoopThread( nullptr )
		, m_paused( false )
		, m_ended( false )
		, m_rendering( false )
		, m_createContext( false )
		, m_created( false )
		, m_interrupted( false )
	{
		m_mainLoopThread.reset( new std::thread( std::bind( &RenderLoopAsync::DoMainLoop, this ) ) );
	}

	RenderLoopAsync::~RenderLoopAsync()
	{
		// We wait for the main loop to end (it makes a final render to clean the render system)
		m_interrupted = true;
		m_mainLoopThread->join();
		m_mainLoopThread.reset();
	}

	bool RenderLoopAsync::IsCreated()const
	{
		return m_created;
	}

	bool RenderLoopAsync::IsRendering()const
	{
		return m_rendering;
	}

	bool RenderLoopAsync::IsInterrupted()const
	{
		return m_interrupted;
	}

	bool RenderLoopAsync::IsEnded()const
	{
		return m_ended;
	}

	bool RenderLoopAsync::IsPaused()const
	{
		return m_paused;
	}

	void RenderLoopAsync::UpdateVSync( bool p_enable )
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

	void RenderLoopAsync::StartRendering()
	{
		m_rendering = true;
	}

	void RenderLoopAsync::RenderSyncFrame()
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
				System::Sleep( 1 );
			}

			m_paused = true;
		}
	}

	void RenderLoopAsync::Pause()
	{
		if ( m_paused )
		{
			CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
		}

		m_paused = true;

		while ( !m_frameEnded )
		{
			System::Sleep( 5 );
		}
	}

	void RenderLoopAsync::Resume()
	{
		if ( !m_paused )
		{
			CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
		}

		m_paused = false;
	}

	void RenderLoopAsync::EndRendering()
	{
		m_rendering = false;

		while ( !IsEnded() )
		{
			System::Sleep( 5 );
		}
	}

	ContextSPtr RenderLoopAsync::DoCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr result;

		if ( !m_createContext )
		{
			DoSetWindow( &p_window );
			m_createContext = true;

			while ( !IsInterrupted() && !IsCreated() )
			{
				System::Sleep( 5 );
			}

			m_createContext = false;
			DoSetWindow( nullptr );
			result = m_renderSystem.GetMainContext();
		}

		return result;
	}

	void RenderLoopAsync::DoMainLoop()
	{
		PreciseTimer timer;
		m_frameEnded = true;
		auto scopeGuard{ make_scope_guard( [this]()
		{
			Cleanup();
			m_renderSystem.Cleanup();
		} ) };

		try
		{
			// Tant qu'on n'est pas interrompu, on continue
			while ( !IsInterrupted() )
			{
				// Tant qu'on n'a pas de contexte principal et qu'on ne nous a pas demandé de le créer, on attend.
				while ( !IsInterrupted() && !m_createContext && !IsCreated() )
				{
					System::Sleep( 10 );
				}

				if ( !IsInterrupted() && m_createContext && !IsCreated() )
				{
					// On nous a demandé de créer le contexte principal, on le crée
					ContextSPtr context = DoCreateContext( *DoGetWindow() );

					if ( context )
					{
						m_renderSystem.SetMainContext( context );
						m_created = true;
					}
					else
					{
						m_interrupted = true;
					}
				}

				// Tant qu'on n'a pas demandé le début du rendu, on attend.
				while ( !IsInterrupted() && !IsRendering() )
				{
					System::Sleep( 10 );
				}

				// Le rendu est en cours
				while ( !IsInterrupted() && IsRendering() && !IsPaused() )
				{
					m_frameEnded = false;
					timer.Time();
					DoRenderFrame();
					auto endTime = std::chrono::duration_cast< Milliseconds >( timer.Time() );
					m_frameEnded = true;
					std::this_thread::sleep_for( std::max( 0_ms, GetFrameTime() - endTime ) );
				}

				m_ended = true;
			}
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( cuT( "RenderLoop - " ) + p_exc.GetFullDescription() );
			m_frameEnded = true;
			m_ended = true;
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( std::string( "RenderLoop - " ) + p_exc.what() );
			m_frameEnded = true;
			m_ended = true;
		}
	}

	void RenderLoopAsync::DoSetWindow( RenderWindow * p_window )
	{
		auto lock = make_unique_lock( m_mutexWindow );
		m_window = p_window;
	}

	RenderWindow * RenderLoopAsync::DoGetWindow()const
	{
		auto lock = make_unique_lock( m_mutexWindow );
		return m_window;
	}
}
