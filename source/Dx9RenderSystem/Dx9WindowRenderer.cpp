#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9WindowRenderer.h"
#include "Dx9RenderSystem/Dx9Context.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

Dx9WindowRenderer :: Dx9WindowRenderer( SceneManager * p_pSceneManager)
	:	WindowRenderer( p_pSceneManager)
	,	m_context( NULL)
	,	m_vCenter( 0, 0, 0)
	,	m_matWorld(	1, 0, 0, 0,		0, 1, 0, 0,		0, 0, 1, 0,		0, 0, 0, 1)
{
}

Dx9WindowRenderer :: ~Dx9WindowRenderer()
{
}

void Dx9WindowRenderer :: Initialise()
{
	m_context = new Dx9Context( m_target);
}

void Dx9WindowRenderer :: StartRender()
{
	m_context->SetCurrent();

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	CheckDxError( Dx9RenderSystem::GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00808080, 1.0f, 0x00), "Dx9WindowRenderer :: StartRender - Clear", false);
	CheckDxError( Dx9RenderSystem::GetDevice()->BeginScene(), "Dx9WindowRenderer :: StartRender - BeginScene", false);
}

void Dx9WindowRenderer :: EndRender()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->EndScene(), CU_T( "Dx9WindowRenderer :: EndRender - EndScene"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->Present( NULL, NULL, NULL, NULL), CU_T( "Dx9WindowRenderer :: EndRender - Present"), false);
	m_context->EndCurrent();
	m_context->SwapBuffers();
}

void Dx9WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void Dx9WindowRenderer :: _initialise()
{
	Logger::LogMessage( CU_T( "Dx9WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == Viewport::e3DView)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, 5.0f);
	}

	l_camera->Resize(m_target->GetWindowWidth(), m_target->GetWindowHeight());

	if ( ! RenderSystem::IsInitialised())
	{
		RenderSystem::GetSingletonPtr()->Initialise();
		m_pSceneManager->GetMaterialManager()->Initialise();
	}
}
