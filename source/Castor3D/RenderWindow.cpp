#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderWindow.hpp"
#include "Castor3D/WindowHandle.hpp"
#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/Context.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/FrameListener.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Viewport.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/DepthStencilState.hpp"
#include "Castor3D/RasterState.hpp"
#include "Castor3D/BlendState.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

ResizeWindowEvent :: ResizeWindowEvent( RenderWindow & p_window )
	:	FrameEvent	( eEVENT_TYPE_PRE_RENDER	)
	,	m_window	( p_window					)
{
}

ResizeWindowEvent :: ResizeWindowEvent( ResizeWindowEvent const & p_copy )
	:	FrameEvent	( p_copy			)
	,	m_window	( p_copy.m_window	)
{
}

ResizeWindowEvent :: ~ResizeWindowEvent()
{
}

ResizeWindowEvent & ResizeWindowEvent :: operator=( ResizeWindowEvent const & p_copy )
{
	ResizeWindowEvent l_evt( p_copy );
	std::swap( this->m_window, l_evt.m_window );
	std::swap( this->m_eType, l_evt.m_eType );
	return *this;
}

bool ResizeWindowEvent :: Apply()
{
	RenderTargetSPtr l_pTarget = m_window.GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->Resize();
	}

	return true;
}

//*************************************************************************************************

WindowHandle :: WindowHandle()
	:	m_pHandle	()
{
}

WindowHandle :: WindowHandle( IWindowHandleSPtr p_pHandle )
	:	m_pHandle	( p_pHandle	)
{
}

WindowHandle :: WindowHandle( WindowHandle const & p_handle )
	:	m_pHandle	( p_handle.m_pHandle	)
{
}

WindowHandle :: WindowHandle( WindowHandle && p_handle )
	:	m_pHandle	( std::move( p_handle.m_pHandle ) )
{
	p_handle.m_pHandle.reset();
}

WindowHandle :: ~WindowHandle()
{
}

WindowHandle & WindowHandle :: operator =( WindowHandle const & p_handle )
{
	m_pHandle = p_handle.m_pHandle;
	return *this;
}

WindowHandle & WindowHandle :: operator =( WindowHandle && p_handle )
{
	if( this != &p_handle )
	{
		m_pHandle = std::move( p_handle.m_pHandle );

		p_handle.m_pHandle.reset();
	}

	return *this;
}

WindowHandle :: operator bool()
{
	return m_pHandle && m_pHandle->operator bool();
}

//*************************************************************************************************

uint32_t	RenderWindow :: s_nbRenderWindows	= 0;

RenderWindow :: RenderWindow( Engine * p_pRoot )
	:   Renderable< RenderWindow, WindowRenderer >	( p_pRoot												)
	,	m_strName									( DoGetName()											)
	,	m_index										( s_nbRenderWindows										)
	,	m_wpListener								( p_pRoot->CreateFrameListener()						)
	,	m_bInitialised								( false													)
	,	m_bVSync									( false													)
	,	m_bFullscreen								( false													)
	,	m_bResized									( true													)
{
	m_wpDepthStencilState	= p_pRoot->CreateDepthStencilState( cuT( "RenderWindowState_" ) + str_utils::to_string( m_index ) );
	m_wpRasteriserState		= p_pRoot->CreateRasteriserState( cuT( "RenderWindowState_" ) + str_utils::to_string( m_index ) );
}

RenderWindow :: ~RenderWindow()
{
	FrameListenerSPtr l_pListener( m_wpListener.lock() );
	m_pEngine->DestroyFrameListener( l_pListener );

	if( !m_pRenderTarget.expired() )
	{
		m_pEngine->RemoveRenderTarget( std::move( m_pRenderTarget.lock() ) );
	}
}

bool RenderWindow :: Initialise( WindowHandle const & p_handle )
{
	m_handle = p_handle;

	if( m_handle && GetRenderer() )
	{
		m_pContext = GetEngine()->CreateContext( this );

		m_bInitialised = m_pContext && m_pContext->IsInitialised();

		if( m_bInitialised )
		{
			m_wpListener.lock()->PostEvent( std::make_shared< InitialiseEvent< WindowRenderer > >( *GetRenderer() ) );
		}
	}

	return m_bInitialised;
}

void RenderWindow :: Cleanup()
{
	m_pContext->Cleanup();
	m_wpListener.lock()->PostEvent( std::make_shared< CleanupEvent< WindowRenderer > >( *GetRenderer() ) );
}

void RenderWindow :: RenderOneFrame( bool p_bForce )
{
	if( !m_pRenderer.expired() && m_bInitialised )
	{
		WindowRendererSPtr l_pRenderer = m_pRenderer.lock();
		Engine * l_pEngine = GetEngine();
		RenderTargetSPtr l_pTarget = GetRenderTarget();

		if( p_bForce )
		{
			if( l_pRenderer->BeginScene() )
			{
				if( l_pTarget )
				{
#if DX_DEBUG
					m_pContext->Bind( eBUFFER_BACK, eFRAMEBUFFER_TARGET_DRAW );
					m_wpDepthStencilState.lock( )->Apply( );
					m_wpRasteriserState.lock( )->Apply( );
					GetEngine( )->GetDefaultBlendState( )->Apply( );
					l_pTarget->Render( 0 );
					l_pRenderer->EndScene();
#else
					if( l_pRenderer->EndScene() && l_pTarget->IsInitialised() )
					{
						GetEngine()->GetDefaultBlendState()->Apply();

						if( IsUsingStereo() && abs( GetIntraOcularDistance() ) > std::numeric_limits< real >::epsilon() && l_pRenderer->GetRenderSystem()->IsStereoAvailable() )
						{
							DoRender( eBUFFER_BACK_LEFT,	l_pTarget->GetTextureLEye() );
							DoRender( eBUFFER_BACK_RIGHT,	l_pTarget->GetTextureREye() );
						}
						else
						{
							DoRender( eBUFFER_BACK, l_pTarget->GetTexture() );
						}
					}

					m_pContext->Bind( eBUFFER_BACK, eFRAMEBUFFER_TARGET_DRAW );
					m_wpDepthStencilState.lock()->Apply();
					m_wpRasteriserState.lock()->Apply();
					l_pEngine->RenderOverlays( l_pTarget->GetSize(), GetScene() );
#endif
				}

				l_pRenderer->EndRender();
			}
		}
	}
}

void RenderWindow :: Resize( int x, int y )
{
	Resize( Size( x, y ) );
}

void RenderWindow :: Resize( Size const & p_size )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetSize( p_size );
	}

	m_bResized = true;
	m_wpListener.lock()->PostEvent( std::make_shared< ResizeWindowEvent >( *this ) );
}

void RenderWindow :: SetCamera( CameraSPtr p_pCamera )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetCamera( p_pCamera );
	}
}

bool RenderWindow :: IsMultisampling()const
{
	bool l_return = false;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->IsMultisampling();
	}

	return l_return;
}

int32_t RenderWindow :: GetSamplesCount()const
{
	int32_t l_return = 0;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetSamplesCount();
	}
	
	return l_return;
}

void RenderWindow :: UpdateFullScreen( bool val )
{
	if( m_bFullscreen != val )
	{
		Logger::LogDebug( cuT( "RenderWindow :: UpdateFullScreen" ) );
		m_pContext->UpdateFullScreen( val );
		Logger::LogDebug( cuT( "Context switched fullscreen" ) );
		m_bFullscreen = val;
	}
}

void RenderWindow :: SetSamplesCount( int32_t val )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetSamplesCount( val );
	}
}

SceneSPtr RenderWindow :: GetScene()const
{
	SceneSPtr l_return;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetScene();
	}

	return l_return;
}

CameraSPtr RenderWindow :: GetCamera()const
{
	CameraSPtr l_return;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetCamera();
	}

	return l_return;
}

eTOPOLOGY RenderWindow :: GetPrimitiveType()const
{
	eTOPOLOGY l_return = eTOPOLOGY( -1 );
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetPrimitiveType();
	}

	return l_return;
}

void RenderWindow :: SetPrimitiveType( eTOPOLOGY val )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetPrimitiveType( val);
	}
}

eVIEWPORT_TYPE RenderWindow :: GetViewportType()const
{
	eVIEWPORT_TYPE l_return = eVIEWPORT_TYPE( -1 );
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetViewportType();
	}

	return l_return;
}

void RenderWindow :: SetViewportType( eVIEWPORT_TYPE val )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetViewportType( val);
	}
}

ePIXEL_FORMAT RenderWindow :: GetPixelFormat()const
{
	ePIXEL_FORMAT l_return = ePIXEL_FORMAT( -1 );
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetPixelFormat();
	}

	return l_return;
}

void RenderWindow :: SetPixelFormat( ePIXEL_FORMAT val )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetPixelFormat( val );
	}
}

void RenderWindow :: SetScene( SceneSPtr p_pScene )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetScene( p_pScene );
	}
}

bool RenderWindow :: IsUsingStereo()const
{
	bool l_return = false;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->IsUsingStereo();
	}

	return l_return;
}

void RenderWindow :: SetStereo( bool p_bStereo )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetStereo( p_bStereo );
	}
}

real RenderWindow :: GetIntraOcularDistance()const
{
	real l_return = 0;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetIntraOcularDistance();
	}

	return l_return;
}

void RenderWindow :: SetIntraOcularDistance( real p_rIao )
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->SetIntraOcularDistance( p_rIao );
	}
}

bool RenderWindow :: IsUsingDeferredRendering()const
{
	bool l_return = false;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->IsUsingDeferredRendering();
	}

	return l_return;
}

Size RenderWindow :: GetSize()const
{
	Size l_return;
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_return = l_pTarget->GetSize();
	}

	return l_return;
}

String RenderWindow :: DoGetName()
{
	String l_strReturn = cuT( "RenderWindow_" );
	l_strReturn += str_utils::to_string( RenderWindow::s_nbRenderWindows++ );
	return l_strReturn;
}

void RenderWindow :: DoRender( eBUFFER p_eTargetBuffer, DynamicTextureSPtr p_pTexture )
{
	m_pContext->Bind( p_eTargetBuffer, eFRAMEBUFFER_TARGET_DRAW );
	m_wpDepthStencilState.lock()->Apply();
	m_wpRasteriserState.lock()->Apply();
	m_pContext->BToBRender( GetRenderTarget()->GetSize(), p_pTexture, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
}

void RenderWindow :: DoUpdateSize()
{
	RenderTargetSPtr l_pTarget = GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->Resize();
	}
}

//*************************************************************************************************

WindowRenderer :: WindowRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer< RenderWindow, WindowRenderer >( p_pRenderSystem )
{
}

WindowRenderer :: ~WindowRenderer()
{
}

bool WindowRenderer :: Initialise()
{
//	ContextSPtr l_pContext = m_target->GetContext();
	ContextRPtr l_pContext = GetRenderSystem()->GetCurrentContext();
	SceneSPtr l_pScene = m_target->GetScene();
	RenderTargetSPtr l_pTarget = m_target->GetRenderTarget();

	if( m_target->IsUsingDeferredRendering() )
	{
		l_pContext->SetDeferredShading( true );
	}

	if( l_pScene )
	{
		l_pContext->SetClearColour( l_pScene->GetBackgroundColour() );
	}
	else
	{
		l_pContext->SetClearColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
	}

	bool l_bReturn = DoInitialise();

	if( l_pTarget )
	{
		l_pTarget->Initialise();
	}

	m_target->SetInitialised();

	return l_bReturn;
}

void WindowRenderer :: Cleanup()
{
	RenderTargetSPtr l_pTarget = m_target->GetRenderTarget();

	if( l_pTarget )
	{
		l_pTarget->Cleanup();
	}

	DoCleanup();
}

//*************************************************************************************************
