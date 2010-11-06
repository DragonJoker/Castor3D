//******************************************************************************
#include "PrecompiledHeader.h"

#include "GLWindowRenderer.h"
#include "GLContext.h"
#include "GLSLProgram.h"
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

	glClearColor( 0.5f, 0.5f, 0.5f, 0.5f);
	CheckGLError( "GLWindowRenderer :: StartRender - glClearColor - ");
	glEnable( GL_DEPTH_TEST);
	CheckGLError( "GLWindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST) - ");

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGLError( "GLWindowRenderer :: StartRender - \n");
}

void GLWindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::pt3DView)
	{
		OverlayManager::GetSingleton().RenderOverlays();
	}

	GLSwapBuffers( m_context);

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
	}
	else
	{
		glColor3f( 1.0f, 1.0f, 1.0f);
		CheckGLError( "GLWindowRenderer :: StartRender - glColor3f - ");
	}

	l_camera->Resize( m_target->GetWindowWidth(), m_target->GetWindowHeight());
}