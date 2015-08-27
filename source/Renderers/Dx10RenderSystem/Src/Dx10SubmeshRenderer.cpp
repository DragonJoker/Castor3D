#include "Dx10SubmeshRenderer.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10ShaderProgram.hpp"

using namespace Castor3D;
using namespace Dx10Render;

DxSubmeshRenderer::DxSubmeshRenderer( DxRenderSystem * p_pRenderSystem )
	:	SubmeshRenderer( p_pRenderSystem )
{
}

DxSubmeshRenderer::~DxSubmeshRenderer()
{
}