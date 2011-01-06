#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2WindowRenderer.h"

using namespace Castor3D;

GL2WindowRenderer :: GL2WindowRenderer( SceneManager * p_pSceneManager)
	:	WindowRenderer( p_pSceneManager)
	,	m_context( NULL)
{
}

GL2WindowRenderer :: ~GL2WindowRenderer()
{
}

void GL2WindowRenderer :: Initialise()
{
	m_context = new GLContext( m_target, NULL);
}

void GL2WindowRenderer :: StartRender()
{
	m_context->SetCurrent();
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - SetContext"));

	glClearColor( 0.5f, 0.5f, 0.5f, 0.5f);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glClearColor - "));
	glEnable( GL_DEPTH_TEST);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST) - "));
	glEnable( GL_NORMALIZE);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glEnable( GL_NORMALIZE) - "));

	// Paramétrage des lumières
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER) - "));
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL) - "));

	if (m_target->GetType() == Viewport::e3DView)
	{
		glEnable( GL_LIGHTING);
		CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glEnable( GL_LIGHTING) - "));
	}
	else
	{
		glDisable( GL_LIGHTING);
		CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glDisable( GL_LIGHTING) - "));
	}

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	glShadeModel( GL_SMOOTH);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glShadeModel - "));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - \n"));
}

void GL2WindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::e3DView)
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

void GL2WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void GL2WindowRenderer :: _initialise()
{
	Logger::LogMessage( CU_T( "GL2WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::e3DView)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
		CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - l_camera->Translate - "));
	}
	else
	{
		glColor3f( 1.0f, 1.0f, 1.0f);
		CheckGLError( CU_T( "GL2WindowRenderer :: StartRender - glColor3f - "));
	}

	l_camera->Resize(m_target->GetWindowWidth(), m_target->GetWindowHeight());

	if ( ! RenderSystem::IsInitialised())
	{
		RenderSystem::GetSingletonPtr()->Initialise();
		m_pSceneManager->GetMaterialManager()->Initialise();
	}
}