#include "Dx11OverlayRenderer.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxOverlayRenderer::DxOverlayRenderer( DxRenderSystem * p_pRenderSystem )
	:	OverlayRenderer( p_pRenderSystem )
{
}

DxOverlayRenderer::~DxOverlayRenderer()
{
}

ShaderProgramBaseSPtr DxOverlayRenderer::DoGetProgram( uint32_t p_uiFlags )
{
	return nullptr;
}

void DxOverlayRenderer::DoInitialise()
{
}

void DxOverlayRenderer::DoCleanup()
{
}
