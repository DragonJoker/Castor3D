#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9WindowRenderer.hpp"
#include "Dx9RenderSystem/Dx9Context.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

Dx9WindowRenderer :: Dx9WindowRenderer()
	:	WindowRenderer()
	,	m_context( nullptr)
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

bool Dx9WindowRenderer :: StartRender()
{
	m_context->SetCurrent();

	if ( ! m_target->IsInitialised())
	{
		_initialise();

		m_target->SetInitialised();
	}

	CheckDxError( Dx9RenderSystem::GetDevice()->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00808080, 1.0f, 0x00), "Dx9WindowRenderer :: StartRender - Clear", false);
	CheckDxError( Dx9RenderSystem::GetDevice()->BeginScene(), "Dx9WindowRenderer :: StartRender - BeginScene", false);

	return true;
}

void Dx9WindowRenderer :: EndRender()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->EndScene(), cuT( "Dx9WindowRenderer :: EndRender - EndScene"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->Present( nullptr, nullptr, nullptr, nullptr), cuT( "Dx9WindowRenderer :: EndRender - Present"), false);
	m_context->EndCurrent();
	m_context->SwapBuffers();
}

void Dx9WindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_target->GetCamera()->Resize( p_width, p_height);
}

void Dx9WindowRenderer :: _initialise()
{
	Logger::LogMessage( cuT( "Dx9WindowRenderer :: StartRender - Initialisation"));

	CameraPtr l_camera = m_target->GetCamera();

	if (m_target->GetType() == eVIEWPORT_TYPE_3D)
	{
		l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
	}

	l_camera->Resize(m_target->GetTargetWidth(), m_target->GetTargetHeight());

	if ( ! RenderSystem::IsInitialised())
	{
		RenderSystem::GetSingletonPtr()->Initialise();
		Root::GetSingleton()->GetMaterialManager()->Initialise();
	}
}
