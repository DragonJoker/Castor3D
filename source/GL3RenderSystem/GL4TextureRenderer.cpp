#include "GL3RenderSystem/PrecompiledHeader.h"

#include "GL3RenderSystem/GL4TextureRenderer.h"
#include "GL3RenderSystem/GL3RenderSystem.h"
#include "GL3RenderSystem/GL3Buffer.h"

using namespace Castor3D;
using namespace Castor::Resources;

GL4TextureRenderer :: GL4TextureRenderer( SceneManager * p_pSceneManager)
	:	GL3TextureRenderer( p_pSceneManager)
{
}

GL4TextureRenderer :: ~GL4TextureRenderer()
{
}

bool GL4TextureRenderer :: Initialise()
{
	bool l_bReturn;

	if (glTexBuffer != NULL)
	{
		m_pfnCleanup	= PVoidFunction( & GL4TextureRenderer::_cleanup);
		m_pfnInitialise	= PBoolFunction( & GL4TextureRenderer::_initialise);
		m_pfnRender		= PVoidFunction( & GL4TextureRenderer::_render);
		m_pfnEndRender	= PVoidFunction( & GL4TextureRenderer::_endRender);
		l_bReturn = (this->*m_pfnInitialise)();
	}
	else
	{
		GL3TextureRenderer::Initialise();
	}

	return l_bReturn;
}

void  GL4TextureRenderer :: _cleanup()
{
}

bool GL4TextureRenderer :: _initialise()
{
	m_pTextureBufferObject = BufferManager::CreateBuffer<unsigned char, GLTextureBufferObject>();
	m_pTextureBufferObject->Initialise( pxfR8G8B8A8, m_target->GetWidth() * m_target->GetHeight(), m_target->GetImagePixels());

	if (m_pTextureBufferObject->GetIndex() != GL_INVALID_INDEX)
	{
		glGenTextures( 1, & m_texGLName);

		if (m_texGLName != GL_INVALID_INDEX)
		{
			CheckGLError( CU_T( "GL2TextureRenderer :: _initialise - glGenTextures"));
			glBindTexture( GL_TEXTURE_BUFFER, m_texGLName);
			CheckGLError( CU_T( "GL2TextureRenderer :: _initialise - glBindTexture"));
			glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA8UI, m_pTextureBufferObject->GetIndex());
			CheckGLError( CU_T( "GL2TextureRenderer :: _initialise - glTexBuffer"));
			glBindTexture(GL_TEXTURE_BUFFER, 0);
			CheckGLError( CU_T( "GL2TextureRenderer :: _initialise - glBindTexture( 0)"));
		}
	}

	return m_texGLName != GL_INVALID_INDEX;
}

void GL4TextureRenderer :: _render()
{
	glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	CheckGLError( CU_T( "GL2TextureRenderer :: Apply - glActiveTexture"));
	glBindTexture( GL_TEXTURE_BUFFER, m_texGLName);
	CheckGLError( CU_T( "GL2TextureRenderer :: Apply - glBindTexture"));

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GL4TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eModelView);
}
