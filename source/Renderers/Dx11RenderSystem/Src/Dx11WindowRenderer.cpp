#include "Dx11WindowRenderer.hpp"
#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>
#include <Material.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxWindowRenderer::DxWindowRenderer( DxRenderSystem * p_pRenderSystem )
	:	WindowRenderer( p_pRenderSystem	)
{
}

DxWindowRenderer::~DxWindowRenderer()
{
	m_context.reset();
}

bool DxWindowRenderer::SetCurrent()
{
	bool l_bReturn = m_context->IsInitialised();

	if ( l_bReturn )
	{
		m_context->SetCurrent();
	}

	return l_bReturn;
}

void DxWindowRenderer::EndCurrent()
{
	m_context->EndCurrent();
}

bool DxWindowRenderer::BeginScene()
{
	m_context->SetCurrent();
	return true;
}

bool DxWindowRenderer::EndScene()
{
	return true;
}

void DxWindowRenderer::EndRender()
{
	m_context->SwapBuffers();
	m_context->EndCurrent();
}

bool DxWindowRenderer::DoInitialise()
{
	m_context = std::static_pointer_cast< DxContext >( m_target->GetContext() );
	Logger::LogMessage( cuT( "Dx11WindowRenderer::StartRender - Initialisation" ) );

	if ( !m_pRenderSystem->IsInitialised() )
	{
		m_pRenderSystem->Initialise();
		m_pRenderSystem->GetEngine()->GetMaterialManager().Initialise();
	}

	return m_target->IsInitialised();
}

void DxWindowRenderer::DoCleanup()
{
}