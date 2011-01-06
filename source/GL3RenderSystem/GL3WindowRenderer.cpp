#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL3WindowRenderer.h"
#include "GL3RenderSystem/GL3Context.h"
#include "GL3RenderSystem/GL3ShaderProgram.h"

using namespace Castor3D;

GL3WindowRenderer :: GL3WindowRenderer( SceneManager * p_pSceneManager)
	:	WindowRenderer( p_pSceneManager)
	,	m_context( NULL)
{
}

GL3WindowRenderer :: ~GL3WindowRenderer()
{
}

void GL3WindowRenderer :: Initialise()
{
	m_context = new GL3Context( m_target, NULL);
}

void GL3WindowRenderer :: StartRender()
{
	m_context->SetCurrent();

	glClearColor( 0.5f, 0.5f, 0.5f, 0.5f);
	CheckGLError( CU_T( "GL3WindowRenderer :: StartRender - glClearColor - "));
	glEnable( GL_DEPTH_TEST);
	CheckGLError( CU_T( "GL3WindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST) - "));

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGLError( CU_T( "GL3WindowRenderer :: StartRender - \n"));
}

void GL3WindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::e3DView)
	{
		OverlayManager::GetSingleton().RenderOverlays();
	}

	glSwapBuffers( m_context);

	m_context->EndCurrent();
}

void GL3WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void GL3WindowRenderer :: _initialise()
{
	Logger::LogMessage( CU_T( "GL3WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::e3DView)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
	}
	else
	{
		glColor3f( 1.0f, 1.0f, 1.0f);
		CheckGLError( CU_T( "GL3WindowRenderer :: StartRender - glColor3f - "));
	}

	l_camera->Resize( m_target->GetWindowWidth(), m_target->GetWindowHeight());
}