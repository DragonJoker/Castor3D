#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxOverlayRenderer.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

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
