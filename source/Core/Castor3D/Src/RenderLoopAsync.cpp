#include "RenderLoopAsync.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	static const char * CALL_RENDER_SYNC_FRAME = "Can't call RenderSyncFrame in threaded render loop";

	RenderLoopAsync::RenderLoopAsync( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_renderSystem, p_wantedFPS )
		, m_mainLoopThread( nullptr )
	{
		m_ended = false;
		m_started = false;
		m_createContext = false;
		m_created = false;
		m_interrupted = false;
		m_mainLoopThread.reset( new std::thread( std::bind( &RenderLoopAsync::DoMainLoop, this ) ) );
	}

	RenderLoopAsync::~RenderLoopAsync()
	{
		Interrupt();
		// We wait for the main loop to end (it makes a final render to clean the render system)
		m_mainLoopThread->join();
		m_mainLoopThread.reset();
	}

	bool RenderLoopAsync::IsEnded()const
	{
		return m_ended;
	}

	void RenderLoopAsync::SetEnded( bool p_value )
	{
		m_ended = p_value;
	}

	bool RenderLoopAsync::IsToCreate()const
	{
		return m_createContext;
	}

	void RenderLoopAsync::SetToCreate( bool p_value )
	{
		m_createContext = p_value;
	}

	bool RenderLoopAsync::IsCreated()const
	{
		return m_created;
	}

	void RenderLoopAsync::SetCreated( bool p_value )
	{
		m_created = p_value;
	}

	bool RenderLoopAsync::IsStarted()const
	{
		return m_started;
	}

	void RenderLoopAsync::SetStarted( bool p_value )
	{
		m_started = p_value;
	}

	bool RenderLoopAsync::IsInterrupted()const
	{
		return m_interrupted;
	}

	void RenderLoopAsync::Interrupt()
	{
		m_interrupted = true;
	}

	void RenderLoopAsync::DoStartRendering()
	{
		m_ended = false;
		m_started = true;
	}

	void RenderLoopAsync::DoRenderSyncFrame()
	{
		CASTOR_ASSERT( false );
		CASTOR_EXCEPTION( CALL_RENDER_SYNC_FRAME );
	}

	void RenderLoopAsync::DoEndRendering()
	{
		m_ended = true;
		m_started = false;
	}

	ContextSPtr RenderLoopAsync::DoCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr l_return;

		if ( !IsToCreate() )
		{
			DoSetWindow( &p_window );
			SetToCreate( true );

			while ( !IsCreated() )
			{
				System::Sleep( 5 );
			}

			SetToCreate( false );
			DoSetWindow( NULL );
			l_return = m_renderSystem->GetMainContext();
		}

		return l_return;
	}

	void RenderLoopAsync::DoMainLoop()
	{
		PreciseTimer l_timer;

		try
		{
			// Tant qu'on n'est pas interrompu, on continue
			while ( !IsInterrupted() )
			{
				// Tant qu'on n'a pas de contexte principal et qu'on ne nous a pas demandé de le créer, on attend.
				while ( !IsInterrupted() && !IsToCreate() && !IsCreated() )
				{
					System::Sleep( 10 );
				}

				if ( !IsInterrupted() && IsToCreate() && !IsCreated() )
				{
					// On nous a demandé de créer le contexte principal, on le crée
					ContextSPtr l_context = DoCreateContext( *DoGetWindow() );

					if ( l_context )
					{
						m_renderSystem->SetMainContext( l_context );
						SetCreated();
					}
				}

				// Tant qu'on n'a pas demandé le début du rendu, on attend.
				while ( !IsInterrupted() && !IsStarted() )
				{
					System::Sleep( 10 );
				}

				// Le rendu est en cours
				while ( !IsInterrupted() && !IsEnded() )
				{
					double l_dFrameTime = GetFrameTime();
					l_timer.TimeS();
					DoRenderOneFrame();
					double l_dTimeDiff = l_timer.TimeS();

					if ( l_dTimeDiff < l_dFrameTime )
					{
						System::Sleep( uint32_t( ( l_dFrameTime - l_dTimeDiff ) * 1000 ) );
					}
					else
					{
						// In order to let the CPU work, we sleep at least 1ms
						System::Sleep( 1 );
					}
				}
			}
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( cuT( "RenderLoop - " ) + p_exc.GetFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( std::string( "RenderLoop - " ) + p_exc.what() );
		}

		// A final render to clean the renderers
		DoRenderOneFrame();
	}

	void RenderLoopAsync::DoSetWindow( RenderWindow * p_window )
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexWindow );
		m_window = p_window;
	}

	RenderWindow * RenderLoopAsync::DoGetWindow()const
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexWindow );
		return m_window;
	}
}
