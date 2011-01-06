#include "GL2RenderSystem/PrecompiledHeader.h"

#include "GL2RenderSystem/GL2TextureRenderer.h"

using namespace Castor3D;

GL2TextureRenderer :: GL2TextureRenderer( SceneManager * p_pSceneManager)
	:	GLTextureRenderer( p_pSceneManager)
{
}

GL2TextureRenderer :: ~GL2TextureRenderer()
{
}

bool GL2TextureRenderer :: Initialise()
{
	return GLTextureRenderer::Initialise();
}
