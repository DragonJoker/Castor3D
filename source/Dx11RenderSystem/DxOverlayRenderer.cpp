#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxOverlayRenderer.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxOverlayRenderer :: DxOverlayRenderer( DxRenderSystem * p_pRenderSystem )
	:	OverlayRenderer( p_pRenderSystem )
{
}

DxOverlayRenderer :: ~DxOverlayRenderer()
{
}

ShaderProgramBaseSPtr DxOverlayRenderer :: DoGetProgram( uint32_t p_uiFlags )
{
	return nullptr;
}

void DxOverlayRenderer :: DoInitialise()
{
}

void DxOverlayRenderer :: DoCleanup()
{
}
