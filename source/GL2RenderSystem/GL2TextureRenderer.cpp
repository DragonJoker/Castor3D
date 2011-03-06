#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2TextureRenderer.h"

using namespace Castor3D;

Gl2TextureRenderer :: Gl2TextureRenderer( SceneManager * p_pSceneManager)
	:	GlTextureRenderer( p_pSceneManager)
{
}

Gl2TextureRenderer :: ~Gl2TextureRenderer()
{
}

bool Gl2TextureRenderer :: Initialise()
{
	return GlTextureRenderer::Initialise();
}
