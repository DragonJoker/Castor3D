#include "PrecompiledHeader.h"

#include "GL2TextureRenderer.h"
#include "GL2RenderSystem.h"
#include "GL2Buffer.h"

using namespace Castor3D;

bool GL2TextureRenderer :: Initialise()
{
/*
	if (glTexBuffer != NULL)
	{
		m_pfnCleanup	= & GL2TextureRenderer::_cleanup;
		m_pfnInitialise	= & GL2TextureRenderer::_initialise;
		m_pfnRender		= & GL2TextureRenderer::_render;
		m_pfnEndRender	= & GL2TextureRenderer::_endRender;
	}
	else
	{
		m_pfnCleanup	= & GLTextureRenderer::_cleanup;
		m_pfnInitialise	= & GLTextureRenderer::_initialise;
		m_pfnRender		= & GLTextureRenderer::_render;
		m_pfnEndRender	= & GLTextureRenderer::_endRender;
	}

	return m_pfnInitialise( this);
*/
	return GLTextureRenderer::Initialise();
}

void  GL2TextureRenderer :: _cleanup()
{
}

bool GL2TextureRenderer :: _initialise()
{
	m_pTextureBufferObject = BufferManager::CreateBuffer<unsigned char, GLTextureBufferObject>();
	m_pTextureBufferObject->Initialise( Resource::pxfR8G8B8A8, m_target->GetWidth() * m_target->GetHeight(), m_target->GetImagePixels());

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

void GL2TextureRenderer :: _render()
{
	glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	CheckGLError( CU_T( "GL2TextureRenderer :: Apply - glActiveTexture"));
	glBindTexture( GL_TEXTURE_BUFFER, m_texGLName);
	CheckGLError( CU_T( "GL2TextureRenderer :: Apply - glBindTexture"));

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GL2TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eModelView);
}
