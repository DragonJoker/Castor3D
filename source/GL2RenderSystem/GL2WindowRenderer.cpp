#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2WindowRenderer.h"
#include "Gl2RenderSystem/Gl2Context.h"

using namespace Castor3D;

Gl2WindowRenderer :: Gl2WindowRenderer( SceneManager * p_pSceneManager)
	:	WindowRenderer( p_pSceneManager)
	,	m_context( NULL)
{
}

Gl2WindowRenderer :: ~Gl2WindowRenderer()
{
}

void Gl2WindowRenderer :: Initialise()
{
	m_context = new Gl2Context( m_target);
}

void Gl2WindowRenderer :: StartRender()
{
	m_context->SetCurrent();
	CheckGlError( glClearColor( 0.5f, 0.5f, 0.5f, 0.5f), CU_T( "Gl2WindowRenderer :: StartRender - glClearColor"));
	CheckGlError( glEnable( GL_DEPTH_TEST), CU_T( "Gl2WindowRenderer :: StartRender - glEnable( GL_DEPTH_TEST)"));
	CheckGlError( glEnable( GL_NORMALIZE), CU_T( "Gl2WindowRenderer :: StartRender - glEnable( GL_NORMALIZE)"));

	// Paramétrage des lumières
	CheckGlError( glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE), CU_T( "Gl2WindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER)"));
	CheckGlError( glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR), CU_T( "Gl2WindowRenderer :: StartRender - glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL)"));

	if (m_target->GetType() == Viewport::e3DView)
	{
		CheckGlError( glEnable( GL_LIGHTING), CU_T( "Gl2WindowRenderer :: StartRender - glEnable( GL_LIGHTING)"));
	}
	else
	{
		CheckGlError( glDisable( GL_LIGHTING), CU_T( "Gl2WindowRenderer :: StartRender - glDisable( GL_LIGHTING)"));
	}

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	CheckGlError( glShadeModel( GL_SMOOTH), CU_T( "Gl2WindowRenderer :: StartRender - glShadeModel"));
	CheckGlError( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT), CU_T( "Gl2WindowRenderer :: StartRender - glClear"));
}

void Gl2WindowRenderer :: EndRender()
{
	if (m_target->GetType() == Viewport::e3DView)
	{
		OverlayManager::GetSingleton().RenderOverlays();
	}

    m_context->SwapBuffers();
	m_context->EndCurrent();
}

void Gl2WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void Gl2WindowRenderer :: _initialise()
{
	Logger::LogMessage( CU_T( "Gl2WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::e3DView)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
	}
	else
	{
		CheckGlError( glColor3f( 1.0f, 1.0f, 1.0f), CU_T( "Gl2WindowRenderer :: StartRender - glColor3f"));
	}

	l_camera->Resize(m_target->GetWindowWidth(), m_target->GetWindowHeight());

	if ( ! RenderSystem::IsInitialised())
	{
		RenderSystem::GetSingletonPtr()->Initialise();
		m_pSceneManager->GetMaterialManager()->Initialise();
	}
}
