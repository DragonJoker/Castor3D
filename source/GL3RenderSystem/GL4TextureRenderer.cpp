#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl4TextureRenderer.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3Buffer.h"

using namespace Castor3D;
using namespace Castor::Resources;

Gl4TextureRenderer :: Gl4TextureRenderer( SceneManager * p_pSceneManager)
	:	Gl3TextureRenderer( p_pSceneManager)
{
}

Gl4TextureRenderer :: ~Gl4TextureRenderer()
{
}

bool Gl4TextureRenderer :: Initialise()
{
	bool l_bReturn;

	if (glTexBuffer != NULL)
	{
		m_pfnCleanup	= PVoidFunction( & Gl4TextureRenderer::_cleanup);
		m_pfnInitialise	= PBoolFunction( & Gl4TextureRenderer::_initialise);
		m_pfnRender		= PVoidFunction( & Gl4TextureRenderer::_render);
		m_pfnEndRender	= PVoidFunction( & Gl4TextureRenderer::_endRender);
		l_bReturn = (this->*m_pfnInitialise)();
	}
	else
	{
		Gl3TextureRenderer::Initialise();
	}

	return l_bReturn;
}

void  Gl4TextureRenderer :: _cleanup()
{
}

bool Gl4TextureRenderer :: _initialise()
{
	m_pTextureBufferObject = BufferManager::CreateBuffer<unsigned char, GlTextureBufferObject>();
	m_pTextureBufferObject->Initialise( eA8R8G8B8, m_target->GetWidth() * m_target->GetHeight(), m_target->GetImagePixels());

	if (m_pTextureBufferObject->GetIndex() != GL_INVALID_INDEX)
	{
		CheckGlError( glGenTextures( 1, & m_glTexName), CU_T( "Gl4TextureRenderer :: _initialise - glGenTextures"))

		if (m_glTexName != GL_INVALID_INDEX)
		{
			CheckGlError( glBindTexture( GL_TEXTURE_BUFFER, m_glTexName), CU_T( "Gl4TextureRenderer :: _initialise - glBindTexture"));
			CheckGlError( glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA8UI, m_pTextureBufferObject->GetIndex()), CU_T( "Gl4TextureRenderer :: _initialise - glTexBuffer"));
			CheckGlError( glBindTexture(GL_TEXTURE_BUFFER, 0), CU_T( "Gl4TextureRenderer :: _initialise - glBindTexture( 0)"));
		}
	}

	return m_glTexName != GL_INVALID_INDEX;
}

void Gl4TextureRenderer :: _render()
{
	CheckGlError( glActiveTexture( GL_TEXTURE0 + m_target->GetIndex()), CU_T( "Gl4TextureRenderer :: Apply - glActiveTexture"));
	CheckGlError( glBindTexture( GL_TEXTURE_BUFFER, m_glTexName), CU_T( "Gl4TextureRenderer :: Apply - glBindTexture"));

	Pipeline::MatrixMode( Pipeline::eMATRIX_MODE( Pipeline::eMatrixTexture0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void Gl4TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);
}
