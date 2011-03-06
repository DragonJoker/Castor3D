#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3TextureEnvironmentRenderer.h"

using namespace Castor3D;

Gl3TextureEnvironmentRenderer :: Gl3TextureEnvironmentRenderer ( SceneManager * p_pSceneManager)
	:	GlTextureEnvironmentRenderer( p_pSceneManager)
{
	m_pfnRender = PRenderFunc( & Gl3TextureEnvironmentRenderer::_render);
}

Gl3TextureEnvironmentRenderer :: ~Gl3TextureEnvironmentRenderer()
{
}

void Gl3TextureEnvironmentRenderer :: _render()
{
}