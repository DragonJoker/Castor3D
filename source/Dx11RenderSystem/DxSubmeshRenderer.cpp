#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxSubmeshRenderer.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"
#include "Dx11RenderSystem/DxBuffer.hpp"
#include "Dx11RenderSystem/DxShaderProgram.hpp"

using namespace Castor3D;
using namespace Dx11Render;

DxSubmeshRenderer :: DxSubmeshRenderer( DxRenderSystem * p_pRenderSystem )
	:	SubmeshRenderer( p_pRenderSystem )
{
}

DxSubmeshRenderer :: ~DxSubmeshRenderer()
{
}