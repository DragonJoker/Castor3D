#include "PrecompiledHeader.h"

#include "GLWindowRenderer.h"
#include "GLContext.h"
#include "Module_GL.h"

/*
#include <Castor3D/camera/Camera.h>
#include <Castor3d/camera/Viewport.h>
#include <Castor3D/light/PointLight.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/material/MaterialManager.h>
#include <CastorUtils/Log.h>
*/
using namespace Castor3D;

void GLWindowRenderer :: Initialise()
{
	m_context = new GLContext( m_target, NULL);
}

void GLWindowRenderer :: StartRender()
{
	m_context->SetCurrent();

	CheckGLError( "GLWindowRenderer :: StartRender - SetContext");

	glClearColor( 0.5f, 0.5f, 0.5f, 0.5f);
	CheckGLError( "GLWindowRenderer :: StartRender - glClearColor - ");
	glEnable( GL_DEPTH_TEST);
	CheckGLError( "GLWindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST) - ");
	glEnable( GL_NORMALIZE);
	CheckGLError( "GLWindowRenderer :: StartRender - glEnable( GL_NORMALIZE) - ");

	// Paramétrage des lumières
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	CheckGLError( "GLWindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER) - ");
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	CheckGLError( "GLWindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL) - ");

	if (m_target->GetType() == Viewport::pt3DView)
	{
		glEnable( GL_LIGHTING);
		CheckGLError( "GLWindowRenderer :: StartRender - glEnable( GL_LIGHTING) - ");
	}
	else
	{
		glDisable( GL_LIGHTING);
		CheckGLError( "GLWindowRenderer :: StartRender - glDisable( GL_LIGHTING) - ");
	}

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	glShadeModel( GL_SMOOTH);
	CheckGLError( "GLWindowRenderer :: StartRender - glShadeModel - ");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGLError( "GLWindowRenderer :: StartRender - \n");

}

void GLWindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::pt3DView)
	{
		OverlayManager::GetSingleton().RenderOverlays();
	}

#ifdef WIN32
	SwapBuffers( m_context->GetHDC());
#else
	glXSwapBuffers( m_context->GetDisplay(), m_context->GetWindow());
#endif

	m_context->EndCurrent();
}

void GLWindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void GLWindowRenderer :: _initialise()
{
	Log::LogMessage( "GLWindowRenderer :: StartRender - Initialisation");

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::pt3DView)
	{
		l_camera->Translate( 0.0f, 0.0f, -5.0f);
		CheckGLError( "GLWindowRenderer :: StartRender - l_camera->Translate - ");
	}
	else
	{
		glColor3f( 1.0f, 1.0f, 1.0f);
		CheckGLError( "GLWindowRenderer :: StartRender - glColor3f - ");
	}

	l_camera->Resize(m_target->GetWindowWidth(), m_target->GetWindowHeight());

	if ( ! GLRenderSystem::IsInitialised())
	{
		GLRenderSystem::GetSingletonPtr()->Initialise();
		CheckGLError( "GLWindowRenderer :: StartRender - RenderSystem::GetSingletonPtr()->Initialise - ");
		MaterialManager::Initialise();
		CheckGLError( "GLWindowRenderer :: StartRender - MaterialManager::Initialise - ");
	}
}