#include "PrecompiledHeader.h"

#include "GLTextureRenderer.h"
#include "GLContext.h"
#include "GLTexEnvironmentRenderer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

void  GLTextureRenderer :: Cleanup()
{
	glBindBuffer( 1, m_uiIndex);
	glDeleteBuffers( 1, & m_uiIndex);
	m_uiIndex = GL_INVALID_INDEX;
}

bool GLTextureRenderer :: Initialise()
{
	glGenBuffers( 1, & m_uiIndex);
	glBindBuffer( GL_TEXTURE_BUFFER, m_uiIndex);

	unsigned int l_uiSize = m_target->GetWidth() * m_target->GetHeight() * 4 * sizeof( float);
	glBufferData( GL_TEXTURE_BUFFER, l_uiSize, 0, GL_DYNAMIC_DRAW);

	glGenTextures( 1, & m_texGLName);
	glBindTexture( GL_TEXTURE_BUFFER, m_texGLName);
	glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA8UI, m_uiIndex);

	unsigned char * l_pBuffer = (unsigned char *)glMapBuffer( GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	memcpy( l_pBuffer, m_target->GetImagePixels(), m_target->GetHeight() * m_target->GetWidth() * 4);
	glUnmapBuffer( GL_TEXTURE_BUFFER);

	glBindBuffer( GL_TEXTURE_BUFFER, 0);

	return true;
}

void GLTextureRenderer :: Apply()
{
	glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
	CheckGLError( "GLTextureRenderer :: Apply - glActiveTexture");
	glBindTexture( GL_TEXTURE_BUFFER, m_texGLName);
	CheckGLError( "GLTextureRenderer :: Apply - glBindTexture");

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GLTextureRenderer :: Remove()
{
	Pipeline::MatrixMode( Pipeline::eModelView);
}
