#include "Castor3D/PrecompiledHeader.h"
#include "Castor3D/main/RenderWindow.h"

#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/main/FrameListener.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/camera/Camera.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/scene/SceneManager.h"

using namespace Castor3D;

size_t	RenderWindow :: s_nbRenderWindows	= 0;

RenderWindow :: RenderWindow( Root * p_pRoot, ScenePtr p_mainScene, void * p_handle, int p_windowWidth, int p_windowHeight,
							 Viewport::eTYPE p_type, ePIXEL_FORMAT p_pixelFormat, ePROJECTION_DIRECTION p_look)
    :   m_handle                ( p_handle)
	,	m_type					( p_type)
	,	m_lookAt				( p_look)
	,	m_drawType				( eTriangles)
	,	m_showNormals			( false)
	,	m_normalsMode			( eFace)
	,	m_timeSinceLastFrame	( 0)
	,	m_initialised			( false)
	,	m_focused				( false)
	,	m_changed				( false)
	,	m_mainScene				( p_mainScene)
	,	m_nbFrame				( 0)
	,	m_windowWidth			( p_windowWidth)
	,	m_windowHeight			( p_windowHeight)
	,	m_pixelFormat			( p_pixelFormat)
	,	m_pRoot					( p_pRoot)
{
	String l_camName;
	m_index = s_nbRenderWindows++;
	l_camName << "RenderCamera_" << m_index;

//	m_camera = m_mainScene->GetRootCamera();
	m_camera = m_mainScene->CreateCamera( l_camName, m_windowWidth, m_windowHeight, p_mainScene->CreateSceneNode( l_camName + "Node"), m_type);

	_setViewPoint();

	WindowRendererPtr l_pRenderer( m_pRenderer);
	l_pRenderer->Initialise();
	m_listener = FrameListenerPtr( new FrameListener());
}

RenderWindow :: ~RenderWindow()
{
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
	if ( m_focused || m_toUpdate || p_bForce)
	{
		m_pRenderer->StartRender();

		if (m_changed || m_mainScene->HasChanged())
		{
			m_mainScene->CreateList( m_normalsMode, m_showNormals);
			m_changed = false;
		}

		m_pRoot->GetSceneManager()->GetMaterialManager()->GetShaderManager()->Update();
		m_pRoot->GetSceneManager()->GetMaterialManager()->Update();
		BufferManager::GetSingleton().Update();
		RenderSystem::GetSingletonPtr()->CleanupRenderersToCleanup();

		m_camera->Render( m_drawType);
		m_mainScene->Render( m_drawType, p_tslf);
		m_listener->FireEvents( FrameEvent::eQueueRender);
		Root::GetSingletonPtr()->FireEvents( FrameEvent::eQueueRender);
		m_camera->EndRender();
		m_pRenderer->EndRender();
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
	l_pRenderer->Resize( unsigned( x), unsigned( y));
	m_toUpdate = true;
}

void RenderWindow :: _setViewPoint()
{
	if (m_type == Viewport::e3DView)
	{
		m_drawType = eTriangles;
	}
	else
	{
		m_drawType = eLines;

		switch (m_lookAt)
		{
		case pdLookToBack:
			m_camera->GetParent()->Yaw( 180.0);
			break;

		case pdLookToLeft:
			m_camera->GetParent()->Yaw( -90.0);
			break;

		case pdLookToRight:
			m_camera->GetParent()->Yaw( 90.0);
			break;

		case pdLookToTop:
			m_camera->GetParent()->Pitch( -90.0);
			break;

		case pdLookToBottom:
			m_camera->GetParent()->Pitch( 90.0);
			break;
		}
	}
}
