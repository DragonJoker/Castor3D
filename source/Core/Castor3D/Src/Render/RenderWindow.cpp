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
	RenderWindow::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< RenderWindow, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool RenderWindow::TextLoader::operator()( RenderWindow const & p_window, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "RenderWindow::Write - Window Name" ) );
		bool l_return = p_file.WriteText( cuT( "window \"" ) + p_window.GetName() + cuT( "\"\n{\n" ) ) > 0;

		if ( l_return && p_window.GetVSync() )
		{
			l_return = p_file.WriteText( cuT( "\tvsync true\n" ) ) > 0;
		}

		if ( l_return && p_window.IsFullscreen() )
		{
			l_return = p_file.WriteText( cuT( "\tfullscreen true\n" ) ) > 0;
		}

		if ( l_return && p_window.GetRenderTarget() )
		{
			l_return = RenderTarget::TextLoader( cuT( "\t" ) )( *p_window.GetRenderTarget(), p_file );
		}

		p_file.WriteText( cuT( "}\n" ) );
		return l_return;
	}

	//*************************************************************************************************

	RenderWindow::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< RenderWindow >( p_path )
	{
	}

	bool RenderWindow::BinaryParser::Fill( RenderWindow const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_WINDOW );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetVSync(), eCHUNK_TYPE_WINDOW_VSYNC, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.IsFullscreen(), eCHUNK_TYPE_WINDOW_FULLSCREEN, l_chunk );
		}

		if ( l_return )
		{
			l_return = RenderTarget::BinaryParser( m_path ).Fill( *p_obj.GetRenderTarget(), l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool RenderWindow::BinaryParser::Parse( RenderWindow & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;
		bool l_bool;
		RenderTargetSPtr l_target;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_WINDOW_VSYNC:
					l_return = DoParseChunk( l_bool, l_chunk );

					if ( l_return )
					{
						p_obj.SetVSync( l_bool );
					}

					break;

				case eCHUNK_TYPE_WINDOW_FULLSCREEN:
					l_return = DoParseChunk( l_bool, l_chunk );

					if ( l_return )
					{
						p_obj.SetFullscreen( l_bool );
					}

					break;

				case eCHUNK_TYPE_TARGET:
					l_target = p_obj.GetEngine()->GetTargetManager().Create( eTARGET_TYPE_WINDOW );
					l_return = RenderTarget::BinaryParser( m_path ).Parse( *l_target, l_chunk );
					break;

				default:
					l_return = false;
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

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

		if ( !m_renderTarget.expired() )
		{
			GetEngine()->GetTargetManager().Remove( std::move( m_renderTarget.lock() ) );
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
		m_context->SetCurrent();
		RenderTargetSPtr l_target = GetRenderTarget();

		if ( l_target )
		{
			l_target->Cleanup();
		}

		m_context->EndCurrent();

		if ( m_context != GetEngine()->GetRenderSystem()->GetMainContext() )
		{
			m_context->Cleanup();
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
