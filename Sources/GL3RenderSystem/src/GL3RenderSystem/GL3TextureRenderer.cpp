#include "PrecompiledHeader.h"

#include "GL3TextureRenderer.h"
#include "GL3Context.h"
#include "GL3RenderSystem.h"

using namespace Castor3D;

bool GL3TextureRenderer :: Initialise()
{
	m_pfnCleanup	= PVoidFunction( & GL3TextureRenderer::_cleanup);
	m_pfnInitialise	= PBoolFunction( & GL3TextureRenderer::_initialise);
	m_pfnRender		= PVoidFunction( & GL3TextureRenderer::_render);
	m_pfnEndRender	= PVoidFunction( & GL3TextureRenderer::_endRender);

	return (this->*m_pfnInitialise)();
}

void  GL3TextureRenderer :: _cleanup()
{
	if (glIsTexture( m_texGLName))
	{
		glDeleteTextures( 1, & m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Cleanup - glDeleteTextures"));
	}
}

bool GL3TextureRenderer :: _initialise()
{
	glGenTextures( 1, & m_texGLName);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glGenTextures"));

	if (m_texGLName == 0)
	{
		Logger::LogError( CU_T( "GLTextureRenderer :: Initialise - Not a valid texture name"));
		return false;
	}

	int l_textureType = GetTextureDimension( m_target->GetTextureType());
	glBindTexture( l_textureType, m_texGLName);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glBindTexture"));

	glTexParameteri( l_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_WRAP_S"));
	glTexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_WRAP_T"));

	glTexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MIN_FILTER"));
	glTexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MAG_FILTER"));

	glTexImage2D( l_textureType, 0, GL_RGBA8, m_target->GetWidth(),
				  m_target->GetHeight(), 0, GL_RGBA, 
				  GL_UNSIGNED_BYTE, m_target->GetImagePixels());
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glTexImage2D"));

	glGenerateMipmap( l_textureType);
	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - glGenerateMipmap"));
	return true;
}

void GL3TextureRenderer :: _render()
{
	if (RenderSystem::UseMultiTexturing())
	{
		glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glActiveTexture"));
		glBindTexture( GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glBindTexture"));
	}
	else
	{
		glBindTexture( GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glBindTexture"));
	}

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
	//TODO : transformations de texture
}

void GL3TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eModelView);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (RenderSystem::UseMultiTexturing())
		{
			glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
			CheckGLError( CU_T( "GLTextureRenderer :: Remove - glActiveTexture"));
		}
	}
}
