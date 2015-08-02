#include "RenderWindow.hpp"
#include "WindowRenderer.hpp"
#include "ResizeWindowEvent.hpp"
#include "InitialiseEvent.hpp"
#include "CleanupEvent.hpp"
#include "WindowHandle.hpp"
#include "RenderTarget.hpp"
#include "Context.hpp"
#include "Buffer.hpp"
#include "FrameListener.hpp"
#include "Scene.hpp"
#include "Viewport.hpp"
#include "Pipeline.hpp"
#include "DynamicTexture.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariable.hpp"
#include "Camera.hpp"
#include "Vertex.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "BlendState.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderWindow::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< RenderWindow, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool RenderWindow::TextLoader::operator()( RenderWindow const & p_window, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "RenderWindow::Write - Window Name" ) );
		bool l_bReturn = p_file.WriteText( cuT( "window \"" ) + p_window.GetName() + cuT( "\"\n{\n" ) ) > 0;

		if ( l_bReturn && p_window.GetVSync() )
		{
			l_bReturn = p_file.WriteText( cuT( "\tvsync true\n" ) ) > 0;
		}

		if ( l_bReturn && p_window.IsFullscreen() )
		{
			l_bReturn = p_file.WriteText( cuT( "\tfullscreen true\n" ) ) > 0;
		}

		if ( l_bReturn && p_window.GetRenderTarget() )
		{
			l_bReturn = RenderTarget::TextLoader( cuT( "\t" ) )( *p_window.GetRenderTarget(), p_file );
		}

		p_file.WriteText( cuT( "}\n" ) );
		return l_bReturn;
	}

	//*************************************************************************************************

	RenderWindow::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< RenderWindow >( p_path )
	{
	}

	bool RenderWindow::BinaryParser::Fill( RenderWindow const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_WINDOW );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetVSync(), eCHUNK_TYPE_WINDOW_VSYNC, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.IsFullscreen(), eCHUNK_TYPE_WINDOW_FULLSCREEN, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = RenderTarget::BinaryParser( m_path ).Fill( *p_obj.GetRenderTarget(), l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool RenderWindow::BinaryParser::Parse( RenderWindow & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;
		bool l_bool;
		RenderTargetSPtr l_target;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_WINDOW_VSYNC:
					l_bReturn = DoParseChunk( l_bool, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetVSync( l_bool );
					}

					break;

				case eCHUNK_TYPE_WINDOW_FULLSCREEN:
					l_bReturn = DoParseChunk( l_bool, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetFullscreen( l_bool );
					}

					break;

				case eCHUNK_TYPE_TARGET:
					l_target = p_obj.GetEngine()->CreateRenderTarget( eTARGET_TYPE_WINDOW );
					l_bReturn = RenderTarget::BinaryParser( m_path ).Parse( *l_target, l_chunk );
					break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	uint32_t	RenderWindow::s_nbRenderWindows	= 0;

	RenderWindow::RenderWindow( Engine * p_pRoot )
		: Renderable< RenderWindow, WindowRenderer >( p_pRoot )
		, m_strName( DoGetName() )
		, m_index( s_nbRenderWindows )
		, m_wpListener( p_pRoot->CreateFrameListener() )
		, m_bInitialised( false )
		, m_bVSync( false )
		, m_bFullscreen( false )
		, m_bResized( true )
	{
		DoCreateRenderer( this );
		m_wpDepthStencilState	= p_pRoot->CreateDepthStencilState( cuT( "RenderWindowState_" ) + str_utils::to_string( m_index ) );
		m_wpRasteriserState		= p_pRoot->CreateRasteriserState( cuT( "RenderWindowState_" ) + str_utils::to_string( m_index ) );
	}

	RenderWindow::~RenderWindow()
	{
		FrameListenerSPtr l_pListener( m_wpListener.lock() );
		m_pEngine->DestroyFrameListener( l_pListener );

		if ( !m_pRenderTarget.expired() )
		{
			m_pEngine->RemoveRenderTarget( std::move( m_pRenderTarget.lock() ) );
		}
	}

	bool RenderWindow::Initialise( WindowHandle const & p_handle )
	{
		m_handle = p_handle;

		if ( m_handle && GetRenderer() )
		{
			m_pContext = GetEngine()->CreateContext( this );
			m_bInitialised = m_pContext && m_pContext->IsInitialised();

			if ( m_bInitialised )
			{
				m_wpListener.lock()->PostEvent( std::make_shared< InitialiseEvent< WindowRenderer > >( *GetRenderer() ) );
			}
		}

		return m_bInitialised;
	}

	void RenderWindow::Cleanup()
	{
		m_pContext->Cleanup();
		m_wpListener.lock()->PostEvent( std::make_shared< CleanupEvent< WindowRenderer > >( *GetRenderer() ) );
	}

	void RenderWindow::RenderOneFrame( bool p_bForce )
	{
		if ( !m_pRenderer.expired() && m_bInitialised )
		{
			WindowRendererSPtr l_pRenderer = m_pRenderer.lock();
			Engine * l_pEngine = GetEngine();
			RenderTargetSPtr l_pTarget = GetRenderTarget();

			if ( p_bForce )
			{
				if ( l_pRenderer->BeginScene() )
				{
					if ( l_pTarget )
					{
						if ( l_pRenderer->EndScene() && l_pTarget->IsInitialised() )
						{
							GetEngine()->GetDefaultBlendState()->Apply();

							if ( IsUsingStereo() && abs( GetIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && l_pRenderer->GetRenderSystem()->IsStereoAvailable() )
							{
								DoRender( eBUFFER_BACK_LEFT,	l_pTarget->GetTextureLEye() );
								DoRender( eBUFFER_BACK_RIGHT,	l_pTarget->GetTextureREye() );
							}
							else
							{
								DoRender( eBUFFER_BACK, l_pTarget->GetTexture() );
							}
						}
					}

					l_pRenderer->EndRender();
				}
			}
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

	void RenderWindow::SetScene( SceneSPtr p_pScene )
	{
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->SetScene( p_pScene );
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

	bool RenderWindow::IsUsingDeferredRendering()const
	{
		bool l_return = false;
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if ( l_pTarget )
		{
			l_return = l_pTarget->IsUsingDeferredRendering();
		}

		return l_return;
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
		l_strReturn += str_utils::to_string( RenderWindow::s_nbRenderWindows++ );
		return l_strReturn;
	}

	void RenderWindow::DoRender( eBUFFER p_eTargetBuffer, DynamicTextureSPtr p_pTexture )
	{
		m_pContext->Bind( p_eTargetBuffer, eFRAMEBUFFER_TARGET_DRAW );
		m_wpDepthStencilState.lock()->Apply();
		m_wpRasteriserState.lock()->Apply();
		m_pContext->BToBRender( GetRenderTarget()->GetSize(), p_pTexture, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
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
