#include "RenderWindow.hpp"

#include "BlendState.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "CleanupEvent.hpp"
#include "Context.hpp"
#include "DepthStencilState.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "FrameListener.hpp"
#include "FrameVariable.hpp"
#include "InitialiseEvent.hpp"
#include "Pipeline.hpp"
#include "RasteriserState.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "ResizeWindowEvent.hpp"
#include "Scene.hpp"
#include "ShaderProgram.hpp"
#include "Vertex.hpp"
#include "Viewport.hpp"
#include "WindowHandle.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderWindow::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< RenderWindow, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
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
		:	Castor3D::BinaryParser< RenderWindow >( p_path )
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
					l_target = p_obj.GetEngine()->CreateRenderTarget( eTARGET_TYPE_WINDOW );
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

	RenderWindow::RenderWindow( Engine * p_pRoot )
		: m_engine( p_pRoot )
		, m_strName( DoGetName() )
		, m_index( s_nbRenderWindows )
		, m_wpListener( p_pRoot->CreateFrameListener( cuT( "RenderWindow_" ) + string::to_string( s_nbRenderWindows ) ) )
		, m_bInitialised( false )
		, m_bVSync( false )
		, m_bFullscreen( false )
		, m_bResized( true )
	{
		m_wpDepthStencilState = m_engine->CreateDepthStencilState( cuT( "RenderWindowState_" ) + string::to_string( m_index ) );
		m_wpRasteriserState = m_engine->CreateRasteriserState( cuT( "RenderWindowState_" ) + string::to_string( m_index ) );
		s_nbRenderWindows++;
	}

	RenderWindow::~RenderWindow()
	{
		FrameListenerSPtr l_pListener( m_wpListener.lock() );
		m_engine->DestroyFrameListener( cuT( "RenderWindow_" ) + string::to_string( m_index ) );

		if ( !m_pRenderTarget.expired() )
		{
			m_engine->RemoveRenderTarget( std::move( m_pRenderTarget.lock() ) );
		}
	}

	bool RenderWindow::Initialise( WindowHandle const & p_handle )
	{
		m_handle = p_handle;

		if ( m_handle )
		{
			m_pContext = GetEngine()->CreateContext( this );
			m_bInitialised = m_pContext && m_pContext->IsInitialised();
			m_pContext->SetCurrent();
			SceneSPtr l_pScene = GetScene();
			RenderTargetSPtr l_pTarget = GetRenderTarget();

			if ( l_pScene )
			{
				m_pContext->SetClearColour( l_pScene->GetBackgroundColour() );
			}
			else
			{
				m_pContext->SetClearColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
			}

			if ( l_pTarget )
			{
				l_pTarget->Initialise( 1 );
			}

			SetInitialised();
			m_pContext->EndCurrent();
		}

		return m_bInitialised;
	}

	void RenderWindow::Cleanup()
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->Cleanup();
		}

		if ( m_pContext != m_engine->GetRenderSystem()->GetMainContext() )
		{
			m_pContext->Cleanup();
		}
	}

	void RenderWindow::RenderOneFrame( bool p_bForce )
	{
		if ( m_bInitialised )
		{
			Engine * l_pEngine = GetEngine();
			RenderTargetSPtr l_pTarget = GetRenderTarget();
			m_pContext->SetCurrent();

			if ( l_pTarget && l_pTarget->IsInitialised() )
			{
				l_pEngine->GetDefaultBlendState()->Apply();

				if ( IsUsingStereo() && abs( GetIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && l_pEngine->GetRenderSystem()->IsStereoAvailable() )
				{
					DoRender( eBUFFER_BACK_LEFT, l_pTarget->GetTextureLEye() );
					DoRender( eBUFFER_BACK_RIGHT, l_pTarget->GetTextureREye() );
				}
				else
				{
					DoRender( eBUFFER_BACK, l_pTarget->GetTexture() );
				}
			}

			m_pContext->EndCurrent();
			m_pContext->SwapBuffers();
		}
	}

	void RenderWindow::Resize( int x, int y )
	{
		Resize( Size( x, y ) );
	}

	void RenderWindow::Resize( Size const & p_size )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetSize( m_pContext->GetMaxSize( p_size ) );
		}

		m_bResized = true;
		m_wpListener.lock()->PostEvent( std::make_shared< ResizeWindowEvent >( *this ) );
	}

	void RenderWindow::SetCamera( CameraSPtr p_pCamera )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetCamera( p_pCamera );
		}
	}

	bool RenderWindow::IsMultisampling()const
	{
		bool l_return = false;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->IsMultisampling();
		}

		return l_return;
	}

	int32_t RenderWindow::GetSamplesCount()const
	{
		int32_t l_return = 0;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetSamplesCount();
		}

		return l_return;
	}

	void RenderWindow::UpdateFullScreen( bool val )
	{
		if ( m_bFullscreen != val )
		{
			Logger::LogDebug( cuT( "RenderWindow::UpdateFullScreen" ) );
			m_pContext->UpdateFullScreen( val );
			Logger::LogDebug( cuT( "Context switched fullscreen" ) );
			m_bFullscreen = val;
		}
	}

	void RenderWindow::SetSamplesCount( int32_t val )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetSamplesCount( val );
		}
	}

	SceneSPtr RenderWindow::GetScene()const
	{
		SceneSPtr l_return;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetScene();
		}

		return l_return;
	}

	CameraSPtr RenderWindow::GetCamera()const
	{
		CameraSPtr l_return;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetCamera();
		}

		return l_return;
	}

	eTOPOLOGY RenderWindow::GetPrimitiveType()const
	{
		eTOPOLOGY l_return = eTOPOLOGY( -1 );
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetPrimitiveType();
		}

		return l_return;
	}

	void RenderWindow::SetPrimitiveType( eTOPOLOGY val )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetPrimitiveType( val );
		}
	}

	eVIEWPORT_TYPE RenderWindow::GetViewportType()const
	{
		eVIEWPORT_TYPE l_return = eVIEWPORT_TYPE( -1 );
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetViewportType();
		}

		return l_return;
	}

	void RenderWindow::SetViewportType( eVIEWPORT_TYPE val )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetViewportType( val );
		}
	}

	ePIXEL_FORMAT RenderWindow::GetPixelFormat()const
	{
		ePIXEL_FORMAT l_return = ePIXEL_FORMAT( -1 );
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetPixelFormat();
		}

		return l_return;
	}

	void RenderWindow::SetPixelFormat( ePIXEL_FORMAT val )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetPixelFormat( val );
		}
	}

	ePIXEL_FORMAT RenderWindow::GetDepthFormat()const
	{
		ePIXEL_FORMAT l_return = ePIXEL_FORMAT( -1 );
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetDepthFormat();
		}

		return l_return;
	}

	void RenderWindow::SetDepthFormat( ePIXEL_FORMAT val )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetDepthFormat( val );
		}
	}

	void RenderWindow::SetScene( SceneSPtr p_scene )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetScene( p_scene );
		}
	}

	bool RenderWindow::IsUsingStereo()const
	{
		bool l_return = false;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->IsUsingStereo();
		}

		return l_return;
	}

	void RenderWindow::SetStereo( bool p_bStereo )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetStereo( p_bStereo );
		}
	}

	real RenderWindow::GetIntraOcularDistance()const
	{
		real l_return = 0;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetIntraOcularDistance();
		}

		return l_return;
	}

	void RenderWindow::SetIntraOcularDistance( real p_rIao )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetIntraOcularDistance( p_rIao );
		}
	}

	Size RenderWindow::GetSize()const
	{
		Size l_return;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->GetSize();
		}

		return l_return;
	}

	String RenderWindow::DoGetName()
	{
		String l_strReturn = cuT( "RenderWindow_" );
		l_strReturn += string::to_string( m_index );
		return l_strReturn;
	}

	void RenderWindow::DoRender( eBUFFER p_eTargetBuffer, DynamicTextureSPtr p_pTexture )
	{
		m_pContext->Bind( p_eTargetBuffer, eFRAMEBUFFER_TARGET_DRAW );
		m_wpDepthStencilState.lock()->Apply();
		m_wpRasteriserState.lock()->Apply();
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();

		if ( l_renderSystem->GetRendererType() != eRENDERER_TYPE_DIRECT3D )
		{
#if !defined( NDEBUG )
			Colour l_save = m_pContext->GetClearColour();
			m_pContext->SetClearColour( Colour::from_predef( Colour::ePREDEFINED_FULLALPHA_RED ) );
#endif
			m_pContext->BToBRender( GetRenderTarget()->GetSize(), p_pTexture, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
#if !defined( NDEBUG )
			m_pContext->SetClearColour( l_save );
#endif
		}
		else
		{
			GetRenderTarget()->Render( 0.0 );
		}
	}

	void RenderWindow::DoUpdateSize()
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->Resize();
		}
	}
}
