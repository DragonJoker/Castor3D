#include "RenderWindow.hpp"

#include "FrameBuffer/BackBuffers.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderTarget.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

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
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "window \"" ) + p_window.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< RenderWindow >::CheckError( result, "RenderWindow name" );

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tvsync %s\n" ), m_tabs.c_str(), p_window.GetVSync() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			Castor::TextWriter< RenderWindow >::CheckError( result, "RenderWindow vsync" );
		}

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tfullscreen %s\n" ), m_tabs.c_str(), p_window.IsFullscreen() ? cuT( "true" ) : cuT( "false" ) ) > 0;
			Castor::TextWriter< RenderWindow >::CheckError( result, "RenderWindow fullscreen" );
		}

		if ( result && p_window.GetRenderTarget() )
		{
			result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_window.GetRenderTarget(), p_file );
		}

		p_file.WriteText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	uint32_t RenderWindow::s_nbRenderWindows = 0;

	RenderWindow::RenderWindow( String const & p_name, Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_index{ s_nbRenderWindows++ }
		, m_wpListener{ p_engine.GetFrameListenerCache().Add( cuT( "RenderWindow_" ) + string::to_string( m_index ) ) }
		, m_backBuffers{ p_engine.GetRenderSystem()->CreateBackBuffers() }
		, m_pickingPass{ std::make_unique< PickingPass >( p_engine ) }
	{
	}

	RenderWindow::~RenderWindow()
	{
		FrameListenerSPtr pListener( m_wpListener.lock() );
		GetEngine()->GetFrameListenerCache().Remove( cuT( "RenderWindow_" ) + string::to_string( m_index ) );
		auto target = m_renderTarget.lock();

		if ( target )
		{
			GetEngine()->GetRenderTargetCache().Remove( target );
		}

		m_pickingPass.reset();
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

				SceneSPtr scene = GetScene();
				RenderTargetSPtr target = GetRenderTarget();

				if ( scene )
				{
					m_backBuffers->SetClearColour( scene->GetBackgroundColour() );
				}
				else
				{
					m_backBuffers->SetClearColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
				}

				if ( target )
				{
					target->Initialise( 1 );
					m_saveBuffer = PxBufferBase::create( target->GetSize(), target->GetPixelFormat() );
				}

				m_pickingPass->Initialise( target->GetSize() );
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
			auto context = GetEngine()->GetRenderSystem()->GetCurrentContext();

			if ( context != m_context.get() )
			{
				m_context->SetCurrent();
			}

			m_pickingPass->Cleanup();
			RenderTargetSPtr target = GetRenderTarget();

			if ( target )
			{
				target->Cleanup();
			}

			if ( context != m_context.get() )
			{
				m_context->EndCurrent();
			}

			if ( m_context != GetEngine()->GetRenderSystem()->GetMainContext() )
			{
				m_context->Cleanup();
			}

			if ( context && context != m_context.get() )
			{
				context->SetCurrent();
			}
		}
	}

	void RenderWindow::Render( bool p_bForce )
	{
		if ( m_initialised )
		{
			Engine * engine = GetEngine();
			RenderTargetSPtr target = GetRenderTarget();
			m_context->SetCurrent();

			if ( target && target->IsInitialised() )
			{
				if ( IsUsingStereo() && abs( GetIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && engine->GetRenderSystem()->GetGpuInformations().IsStereoAvailable() )
				{
					//DoRender( WindowBuffer::eBackLeft, target->GetTextureLEye() );
					//DoRender( WindowBuffer::eBackRight, target->GetTextureREye() );
				}
				else
				{
					DoRender( WindowBuffer::eBack, target->GetTexture() );
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
			m_wpListener.lock()->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				DoUpdateSize();
			} ) );
		}
	}

	void RenderWindow::SetCamera( CameraSPtr p_pCamera )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			target->SetCamera( p_pCamera );
		}
	}

	void RenderWindow::UpdateFullScreen( bool p_value )
	{
		m_bFullscreen = p_value;
	}

	SceneSPtr RenderWindow::GetScene()const
	{
		SceneSPtr result;
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			result = target->GetScene();
		}

		return result;
	}

	CameraSPtr RenderWindow::GetCamera()const
	{
		CameraSPtr result;
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			result = target->GetCamera();
		}

		return result;
	}

	ViewportType RenderWindow::GetViewportType()const
	{
		ViewportType result = ViewportType( -1 );
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			result = target->GetViewportType();
		}

		return result;
	}

	void RenderWindow::SetViewportType( ViewportType val )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			target->SetViewportType( val );
		}
	}

	PixelFormat RenderWindow::GetPixelFormat()const
	{
		PixelFormat result = PixelFormat( -1 );
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			result = target->GetPixelFormat();
		}

		return result;
	}

	void RenderWindow::SetPixelFormat( PixelFormat val )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			target->SetPixelFormat( val );
		}
	}

	void RenderWindow::SetScene( SceneSPtr p_scene )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			target->SetScene( p_scene );
		}
	}

	bool RenderWindow::IsUsingStereo()const
	{
		bool result = false;
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			//l_result = target->IsUsingStereo();
		}

		return result;
	}

	void RenderWindow::SetStereo( bool p_bStereo )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			//l_target->SetStereo( p_bStereo );
		}
	}

	real RenderWindow::GetIntraOcularDistance()const
	{
		real result = 0;
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			//l_result = target->GetIntraOcularDistance();
		}

		return result;
	}

	void RenderWindow::SetIntraOcularDistance( real p_rIao )
	{
		RenderTargetSPtr target = GetRenderTarget();

		if ( target )
		{
			//l_target->SetIntraOcularDistance( p_rIao );
		}
	}

	Size RenderWindow::GetSize()const
	{
		return m_size;
	}

	void RenderWindow::DoRender( WindowBuffer p_eTargetBuffer, TextureUnit const & p_texture )
	{
		auto texture = p_texture.GetTexture();

		if ( m_toSave )
		{
			auto buffer = texture->Lock( AccessType::eRead );

			if ( buffer )
			{
				std::memcpy( m_saveBuffer->ptr(), buffer, m_saveBuffer->size() );
			}

			texture->Unlock( false );
			m_toSave = false;
		}

		m_backBuffers->Bind( p_eTargetBuffer, FrameBufferTarget::eDraw );
		m_backBuffers->Clear( BufferComponent::eColour );
		m_context->RenderTexture( m_size, *texture );
		m_backBuffers->Unbind();
	}

	void RenderWindow::DoUpdateSize()
	{
		m_backBuffers->Resize( GetSize() );
	}
}
