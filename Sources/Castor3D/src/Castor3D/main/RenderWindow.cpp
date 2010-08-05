#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/RenderWindow.h"
#include "main/FrameListener.h"
#include "render_system/RenderSystem.h"
#include "render_system/WindowRenderer.h"
#include "render_system/Buffer.h"
#include "material/Material.h"
#include "material/MaterialManager.h"
#include "material/TextureUnit.h"
#include "scene/Scene.h"
#include "scene/SceneNode.h"
#include "camera/Camera.h"
#include "camera/Viewport.h"
#include "light/PointLight.h"
#include "main/Root.h"

#include "Log.h"

using namespace Castor3D;

size_t	RenderWindow :: s_nbRenderWindows	= 0;

RenderWindow :: RenderWindow( WindowRenderer * p_renderer, Scene * p_mainScene, void * p_hWnd,
							  int p_windowWidth, int p_windowHeight,
							  ProjectionType p_type, ProjectionDirection p_look)
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
		m_windowHeight			( p_windowHeight),
		m_renderer				( p_renderer),
		m_camera				( NULL)
{
	m_renderer->SetTarget( this);
	Char l_camName[255];
	m_index = s_nbRenderWindows++;
	Sprintf( l_camName, 255, C3D_T( "RenderCamera_%d"), m_index);

	m_camera = m_mainScene->GetRootCamera();

	_setViewPoint();

	m_renderer->Initialise();
	m_listener = new FrameListener();
}

RenderWindow :: ~RenderWindow()
{
	delete m_listener;
}

bool RenderWindow :: PreRender()
{
	if ( ! m_initialised)
	{
		m_timeSinceLastFrame = 0;
		m_nbFrame = 0;
		m_changed = true;
	}

	if ( ! m_listener->FireEvents( fetPreRender))
	{
		return false;
	}

	return true;
}

void RenderWindow :: RenderOneFrame( const float & p_tslf, const bool & p_bForce)
{
	if ( m_focused || m_toUpdate || p_bForce)
	{
		m_renderer->StartRender();

		if (m_changed || m_mainScene->HasChanged())
		{
			m_mainScene->CreateList( m_normalsMode, m_showNormals);
			m_changed = false;
		}


		MaterialManager::GetSingletonPtr()->Update();
		BufferManager::GetSingleton().Update();
		m_camera->Apply();

		m_mainScene->Render( m_drawType, p_tslf);

		m_listener->FireEvents( fetQueueRender);

		m_camera->Remove();

		m_renderer->EndRender();
	}
}

bool RenderWindow :: PostRender()
{
	if ( ! m_listener->FireEvents( fetPostRender))
	{
		return false;
	}

	m_toUpdate = false;

	return true;
}

void RenderWindow :: Resize( int x, int y)
{
	m_renderer->Resize(unsigned( x), unsigned( y));
	m_toUpdate = true;
}

void RenderWindow :: _setViewPoint()
{
	if (m_type == pt3DView)
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