#include "RenderWindow.hpp"

#include "BlendStateManager.hpp"
#include "CameraManager.hpp"
#include "DepthStencilStateManager.hpp"
#include "Engine.hpp"
#include "ListenerManager.hpp"
#include "RasteriserStateManager.hpp"
#include "TargetManager.hpp"
#include "SceneManager.hpp"
#include "ShaderManager.hpp"

#include "Context.hpp"
#include "Pipeline.hpp"
#include "RenderLoop.hpp"
#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "FrameBuffer/BackBuffers.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Miscellaneous/WindowHandle.hpp"
#include "Texture/TextureLayout.hpp"
#include "Shader/FrameVariable.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderWindow::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< RenderWindow >{ p_tabs }
	{
	}

	bool RenderWindow::TextWriter::operator()( RenderWindow const & p_window, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing Window " ) + p_window.GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "window \"" ) + p_window.GetName() + cuT( "\"\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tvsync %s\n" ), m_tabs.c_str(), p_window.GetVSync() ? cuT( "true" ) : cuT( "false" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tfullscreen %s\n" ), m_tabs.c_str(), p_window.IsFullscreen() ? cuT( "true" ) : cuT( "false" ) ) > 0;
		}

		if ( l_return && p_window.GetRenderTarget() )
		{
			l_return = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_window.GetRenderTarget(), p_file );
		}

		p_file.WriteText( m_tabs + cuT( "}\n" ) );
		return l_return;
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >( p_engine )
		, Named( p_name )
		, m_index( s_nbRenderWindows )
		, m_wpListener( p_engine.GetListenerManager().Create( cuT( "RenderWindow_" ) + string::to_string( s_nbRenderWindows ) ) )
		, m_initialised( false )
		, m_bVSync( false )
		, m_bFullscreen( false )
		, m_backBuffers( p_engine.GetRenderSystem()->CreateBackBuffers() )
	{
		auto l_dsstate = GetEngine()->GetDepthStencilStateManager().Create( cuT( "RenderWindowState_" ) + string::to_string( m_index ) );
		l_dsstate->SetDepthTest( false );
		m_wpDepthStencilState = l_dsstate;
		m_wpRasteriserState = GetEngine()->GetRasteriserStateManager().Create( cuT( "RenderWindowState_" ) + string::to_string( m_index ) );
		s_nbRenderWindows++;
	}

	RenderWindow::~RenderWindow()
	{
		FrameListenerSPtr l_pListener( m_wpListener.lock() );
		GetEngine()->GetListenerManager().Remove( cuT( "RenderWindow_" ) + string::to_string( m_index ) );
		auto l_target = m_renderTarget.lock();

		if ( l_target )
		{
			GetEngine()->GetTargetManager().Remove( l_target );
		}
	}

	bool RenderWindow::Initialise( Size const & p_size, WindowHandle const & p_handle )
	{
		m_size = p_size;
		m_handle = p_handle;

		if ( m_handle )
		{
			GetEngine()->GetRenderLoop().CreateContext( *this );
			m_initialised = m_context && m_context->IsInitialised();

			if ( m_initialised )
			{
				m_context->SetCurrent();
				m_backBuffers->Initialise( GetSize(), GetPixelFormat() );

				SceneSPtr l_scene = GetScene();
				RenderTargetSPtr l_target = GetRenderTarget();

				if ( l_scene )
				{
					m_backBuffers->SetClearColour( l_scene->GetBackgroundColour() );
				}
				else
				{
					m_backBuffers->SetClearColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
				}

				if ( l_target )
				{
					l_target->Initialise( 1 );
					m_saveBuffer = PxBufferBase::create( l_target->GetSize(), l_target->GetPixelFormat() );
				}

				m_context->EndCurrent();
				m_initialised = true;
			}
		}

		return m_initialised;
	}

	void RenderWindow::Cleanup()
	{
		m_initialised = false;

		if ( m_context )
		{
			auto l_context = GetEngine()->GetRenderSystem()->GetCurrentContext();

			if ( l_context != m_context.get() )
			{
				m_context->SetCurrent();
			}

			RenderTargetSPtr l_target = GetRenderTarget();

			if ( l_target )
			{
				l_target->Cleanup();
			}

			if ( l_context != m_context.get() )
			{
				m_context->EndCurrent();
			}

			if ( m_context != GetEngine()->GetRenderSystem()->GetMainContext() )
			{
				m_context->Cleanup();
			}

			if ( l_context != m_context.get() )
			{
				l_context->SetCurrent();
			}
		}
	}

	void RenderWindow::Render( bool p_bForce )
	{
		if ( m_initialised )
		{
			Engine * l_pEngine = GetEngine();
			RenderTargetSPtr l_target = GetRenderTarget();
			m_context->SetCurrent();

			if ( l_target && l_target->IsInitialised() )
			{
				l_pEngine->GetDefaultBlendState()->Apply();

				if ( IsUsingStereo() && abs( GetIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && l_pEngine->GetRenderSystem()->GetGpuInformations().IsStereoAvailable() )
				{
					DoRender( eBUFFER_BACK_LEFT, l_target->GetTextureLEye() );
					DoRender( eBUFFER_BACK_RIGHT, l_target->GetTextureREye() );
				}
				else
				{
					DoRender( eBUFFER_BACK, l_target->GetTexture() );
				}
			}

			m_context->EndCurrent();
			m_context->SwapBuffers();
		}
	}

	void RenderWindow::Resize( int x, int y )
	{
		Resize( Size( x, y ) );
	}

	void RenderWindow::Resize( Size const & p_size )
	{
		m_size = p_size;

		if ( m_initialised )
		{
			m_wpListener.lock()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this]()
			{
				DoUpdateSize();
			} ) );
		}
	}

	void RenderWindow::SetCamera( CameraSPtr p_pCamera )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetCamera( p_pCamera );
		}
	}

	bool RenderWindow::IsMultisampling()const
	{
		bool l_return = false;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->IsMultisampling();
		}

		return l_return;
	}

	int32_t RenderWindow::GetSamplesCount()const
	{
		int32_t l_return = 0;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetSamplesCount();
		}

		return l_return;
	}

	void RenderWindow::UpdateFullScreen( bool p_value )
	{
		m_bFullscreen = p_value;
	}

	void RenderWindow::SetSamplesCount( int32_t val )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetSamplesCount( val );
		}
	}

	SceneSPtr RenderWindow::GetScene()const
	{
		SceneSPtr l_return;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetScene();
		}

		return l_return;
	}

	CameraSPtr RenderWindow::GetCamera()const
	{
		CameraSPtr l_return;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetCamera();
		}

		return l_return;
	}

	eVIEWPORT_TYPE RenderWindow::GetViewportType()const
	{
		eVIEWPORT_TYPE l_return = eVIEWPORT_TYPE( -1 );
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetViewportType();
		}

		return l_return;
	}

	void RenderWindow::SetViewportType( eVIEWPORT_TYPE val )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetViewportType( val );
		}
	}

	ePIXEL_FORMAT RenderWindow::GetPixelFormat()const
	{
		ePIXEL_FORMAT l_return = ePIXEL_FORMAT( -1 );
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetPixelFormat();
		}

		return l_return;
	}

	void RenderWindow::SetPixelFormat( ePIXEL_FORMAT val )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetPixelFormat( val );
		}
	}

	void RenderWindow::SetScene( SceneSPtr p_scene )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetScene( p_scene );
		}
	}

	bool RenderWindow::IsUsingStereo()const
	{
		bool l_return = false;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->IsUsingStereo();
		}

		return l_return;
	}

	void RenderWindow::SetStereo( bool p_bStereo )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetStereo( p_bStereo );
		}
	}

	real RenderWindow::GetIntraOcularDistance()const
	{
		real l_return = 0;
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_return = l_target->GetIntraOcularDistance();
		}

		return l_return;
	}

	void RenderWindow::SetIntraOcularDistance( real p_rIao )
	{
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->SetIntraOcularDistance( p_rIao );
		}
	}

	Size RenderWindow::GetSize()const
	{
		return m_size;
	}

	void RenderWindow::DoRender( eBUFFER p_eTargetBuffer, TextureUnit const & p_texture )
	{
		auto l_texture = p_texture.GetTexture();

		if ( m_toSave )
		{
			auto l_buffer = l_texture->GetImage().Lock( eACCESS_TYPE_READ );

			if ( l_buffer )
			{
				std::memcpy( m_saveBuffer->ptr(), l_buffer, m_saveBuffer->size() );
				l_texture->GetImage().Unlock( false );
			}

			m_toSave = false;
		}

		if ( m_backBuffers->Bind( p_eTargetBuffer, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			m_backBuffers->Clear();
			m_wpDepthStencilState.lock()->Apply();
			m_wpRasteriserState.lock()->Apply();
			m_context->RenderTexture( m_size, *l_texture );
			m_backBuffers->Unbind();
		}
	}

	void RenderWindow::DoUpdateSize()
	{
		m_backBuffers->Resize( GetSize() );
	}
}
