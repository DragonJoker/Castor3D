#include "PrecompiledHeader.h"
#include "main/RenderWindow.h"

#include "scene/Scene.h"
#include "main/FrameListener.h"
#include "camera/Camera.h"
#include "material/MaterialManager.h"
#include "shader/ShaderManager.h"
#include "render_system/Buffer.h"

using namespace Castor3D;

size_t	RenderWindow :: s_nbRenderWindows	= 0;

RenderWindow :: RenderWindow( ScenePtr p_mainScene, void * p_hWnd, int p_windowWidth, int p_windowHeight,
							  Viewport::eTYPE p_type, ProjectionDirection p_look)
	:	m_type					( p_type),
		m_lookAt				( p_look),
		m_hWnd					( p_hWnd),
		m_drawType				( DTTriangles),
		m_showNormals			( false),
		m_normalsMode			( nmFace),
		m_timeSinceLastFrame	( 0),
		m_initialised			( false),
		m_focused				( false),
		m_changed				( false),
		m_mainScene				( p_mainScene),
		m_nbFrame				( 0),
		m_windowWidth			( p_windowWidth),
		m_windowHeight			( p_windowHeight)
{
	Char l_camName[255];
	m_index = s_nbRenderWindows++;
	Sprintf( l_camName, 255, CU_T( "RenderCamera_%d"), m_index);

	m_camera = m_mainScene->GetRootCamera();

	_setViewPoint();

	WindowRendererPtr l_pRenderer( m_pRenderer);
	l_pRenderer->Initialise();
	m_listener = new FrameListener();
}

RenderWindow :: ~RenderWindow()
{
//	delete m_listener;
}

bool RenderWindow :: PreRender()
{
	if ( ! m_initialised)
	{
		m_timeSinceLastFrame = 0;
		m_nbFrame = 0;
		m_changed = true;
	}

	if ( ! m_listener->FireEvents( FrameEvent::ePreRender))
	{
		return false;
	}

	return true;
}

void RenderWindow :: RenderOneFrame( const real & p_tslf, const bool & p_bForce)
{
	WindowRendererPtr l_pRenderer( m_pRenderer);

	if ( m_focused || m_toUpdate || p_bForce)
	{
		l_pRenderer->StartRender();

		if (m_changed || m_mainScene->HasChanged())
		{
			m_mainScene->CreateList( m_normalsMode, m_showNormals);
			m_changed = false;
		}

		ShaderManager::GetSingleton().Update();
		MaterialManager::Update();
		BufferManager::GetSingleton().Update();
		m_camera->Apply( m_drawType);

		m_mainScene->Render( m_drawType, p_tslf);

		m_listener->FireEvents( FrameEvent::eQueueRender);

		m_camera->Remove();

		l_pRenderer->EndRender();
	}
}

bool RenderWindow :: PostRender()
{
	if ( ! m_listener->FireEvents( FrameEvent::ePostRender))
	{
		return false;
	}

	m_toUpdate = false;

	return true;
}

void RenderWindow :: Resize( int x, int y)
{
	WindowRendererPtr l_pRenderer( m_pRenderer);
	l_pRenderer->Resize(unsigned( x), unsigned( y));
	m_toUpdate = true;
}

void RenderWindow :: _setViewPoint()
{
	if (m_type == Viewport::pt3DView)
	{
		m_drawType = DTTriangles;
	}
	else
	{
		m_drawType = DTLines;
		if (m_lookAt != pdLookToFront)
		{
			if (m_lookAt == pdLookToBack)
			{
				m_camera->Yaw( 180.0);
			}
			else if (m_lookAt == pdLookToLeft)
			{
				m_camera->Yaw( -90.0);
			}
			else if (m_lookAt == pdLookToRight)
			{
				m_camera->Yaw( 90.0);
			}
			else if (m_lookAt == pdLookToTop)
			{
				m_camera->Pitch( -90.0);
			}
			else if (m_lookAt == pdLookToBottom)
			{
				m_camera->Pitch( 90.0);
			}
		}
	}
}