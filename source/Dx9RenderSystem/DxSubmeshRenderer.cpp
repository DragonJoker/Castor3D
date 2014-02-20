#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxSubmeshRenderer.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"
#include "Dx9RenderSystem/DxBuffer.hpp"
#include "Dx9RenderSystem/DxShaderProgram.hpp"
//#include "Dx9RenderSystem/DirectX9.hpp"

using namespace Castor3D;
using namespace Dx9Render;

DxSubmeshRenderer :: DxSubmeshRenderer( DxRenderSystem * p_pRenderSystem )
	:	SubmeshRenderer( p_pRenderSystem )
{
}

DxSubmeshRenderer :: ~DxSubmeshRenderer()
{
}
