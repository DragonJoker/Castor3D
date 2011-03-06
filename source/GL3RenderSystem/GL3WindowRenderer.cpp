#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3WindowRenderer.h"
#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3ShaderProgram.h"

using namespace Castor3D;

Gl3WindowRenderer :: Gl3WindowRenderer( SceneManager * p_pSceneManager)
	:	WindowRenderer( p_pSceneManager)
	,	m_context( NULL)
{
}

Gl3WindowRenderer :: ~Gl3WindowRenderer()
{
}

void Gl3WindowRenderer :: Initialise()
{
	m_context = new Gl3Context( m_target);
}

void Gl3WindowRenderer :: StartRender()
{
	m_context->SetCurrent();

	CheckGlError( glClearColor( 0.5f, 0.5f, 0.5f, 0.5f), CU_T( "Gl3WindowRenderer :: StartRender - glClearColor - "));
	CheckGlError( glEnable( GL_DEPTH_TEST), CU_T( "Gl3WindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST) - "));

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	CheckGlError( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT), CU_T( "Gl3WindowRenderer :: StartRender - \n"));
}

void Gl3WindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::e3DView)
	{
		OverlayManager::GetSingleton().RenderOverlays();
	}

	m_context->SwapBuffers();
	m_context->EndCurrent();
}

void Gl3WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void Gl3WindowRenderer :: _initialise()
{
	Logger::LogMessage( CU_T( "Gl3WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::e3DView)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
	}
	else
	{
		CheckGlError( glColor3f( 1.0f, 1.0f, 1.0f), CU_T( "Gl3WindowRenderer :: StartRender - glColor3f - "));
	}

	l_camera->Resize( m_target->GetWindowWidth(), m_target->GetWindowHeight());
}
