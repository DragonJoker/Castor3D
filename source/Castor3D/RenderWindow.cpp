#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderWindow.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/FrameListener.hpp"
#include "Castor3D/Buffer.hpp"

using namespace Castor3D;

size_t	RenderWindow :: s_nbRenderWindows	= 0;

RenderWindow :: RenderWindow( Root * p_pRoot, ScenePtr p_mainScene, void * p_handle, int p_windowWidth, int p_windowHeight,
							 eVIEWPORT_TYPE p_type, ePIXEL_FORMAT p_pixelFormat, ePROJECTION_DIRECTION p_look)
    :   RenderTarget( _getName(), p_pRoot, p_mainScene, Point2i( p_windowWidth, p_windowHeight), p_type, p_look)
	,	m_handle                ( p_handle)
	,	m_showNormals			( false)
	,	m_normalsMode			( eNORMALS_MODE_FACE)
	,	m_timeSinceLastFrame	( 0)
	,	m_initialised			( false)
	,	m_focused				( false)
	,	m_changed				( false)
	,	m_nbFrame				( 0)
	,	m_pixelFormat			( p_pixelFormat)
	,	m_index					( s_nbRenderWindows)
{
	WindowRendererPtr l_pRenderer( m_pRenderer);
	l_pRenderer->Initialise();
	m_pListener = FrameListenerPtr( new FrameListener());
}

RenderWindow :: ~RenderWindow()
{
}

bool RenderWindow :: PreRender()
{
	CASTOR_TRACK;
	if ( ! m_initialised)
	{
		m_timeSinceLastFrame = 0;
		m_nbFrame = 0;
		m_changed = true;
	}

	return m_pListener->FireEvents( eEVENT_TYPE_PRE_RENDER);
}

bool RenderWindow :: PostRender()
{
	CASTOR_TRACK;
	bool l_bReturn = m_pListener->FireEvents( eEVENT_TYPE_POST_RENDER);

	if (l_bReturn)
	{
		m_toUpdate = false;
	}

	return l_bReturn;
}

void RenderWindow :: RenderOneFrame( const real & p_tslf, const bool & p_bForce)
{
	CASTOR_TRACK;
	if ( m_focused || m_toUpdate || p_bForce)
	{
		if (m_pRenderer->StartRender())
		{
			if (m_changed || m_pMainScene->HasChanged())
			{
				m_pMainScene->CreateList( m_normalsMode, m_showNormals);
				m_changed = false;
			}

			RenderTarget::RenderOneFrame( p_tslf, p_bForce);
			m_pRenderer->EndRender();
		}
	}
}

void RenderWindow :: Resize( int x, int y)
{
	CASTOR_TRACK;
	WindowRendererPtr l_pRenderer( m_pRenderer);
	l_pRenderer->Resize( unsigned( x), unsigned( y));
	m_toUpdate = true;
}

String RenderWindow :: _getName()
{
	String l_strReturn = cuT( "RenderWindow_");
	l_strReturn << RenderWindow::s_nbRenderWindows++;
	return l_strReturn;
}
