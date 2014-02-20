#include "Dx10RenderSystem/PrecompiledHeader.hpp"

#include "Dx10RenderSystem/DxSubmeshRenderer.hpp"
#include "Dx10RenderSystem/DxRenderSystem.hpp"
#include "Dx10RenderSystem/DxBuffer.hpp"
#include "Dx10RenderSystem/DxShaderProgram.hpp"

using namespace Castor3D;
using namespace Dx10Render;

DxSubmeshRenderer :: DxSubmeshRenderer( DxRenderSystem * p_pRenderSystem )
	:	SubmeshRenderer( p_pRenderSystem )
{
}

DxSubmeshRenderer :: ~DxSubmeshRenderer()
{
}