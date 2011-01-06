#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLTextureRenderer.h"
#include "OpenGLCommon/GLRenderSystem.h"

using namespace Castor3D;

void GLTextureRenderer :: Cleanup()
{
	(this->*m_pfnCleanup)();
}

bool GLTextureRenderer :: Initialise()
{
	m_pfnCleanup	= & GLTextureRenderer::_cleanup;
	m_pfnInitialise	= & GLTextureRenderer::_initialise;
	m_pfnRender		= & GLTextureRenderer::_render;
	m_pfnEndRender	= & GLTextureRenderer::_endRender;

	return (this->*m_pfnInitialise)();
}

void GLTextureRenderer :: Render()
{
	(this->*m_pfnRender)();
}

void GLTextureRenderer :: EndRender()
{
	(this->*m_pfnEndRender)();
}

void  GLTextureRenderer :: _cleanup()
{
	if (glIsTexture( m_texGLName))
	{
		glDeleteTextures( 1, & m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Cleanup - glDeleteTextures"));
	}
}

bool GLTextureRenderer :: _initialise()
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

	if (glGenerateMipmap == NULL)
	{
		gluBuild2DMipmaps( l_textureType, GL_RGBA8, m_target->GetWidth(), m_target->GetHeight(),
							GL_RGBA, GL_UNSIGNED_BYTE, m_target->GetImagePixels());
	}
	else
	{
		glGenerateMipmap( l_textureType);
	}

	CheckGLError( CU_T( "GLTextureRenderer :: Initialise - gluBuild2DMipmaps"));
	return true;
}

void GLTextureRenderer :: _render()
{
	if (RenderSystem::UseMultiTexturing())
	{
		glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glActiveTexture"));
		glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_target->GetPrimaryColour());
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glTexEnvfv"));
		glBindTexture( GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glBindTexture"));
		m_target->GetEnvironment()->Render( eTriangles);
	}
	else
	{
		glBindTexture( GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glBindTexture"));
	}

	glEnable( GetTextureDimension( m_target->GetTextureType()));
	CheckGLError( CU_T( "GLTextureRenderer :: Apply - glEnable"));

	switch(m_target->GetTextureMapMode())
	{
	case TextureUnit::eReflexionMap:
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glTexGeni( GL_S)"));
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glTexGeni( GL_T)"));
		glEnable( GL_TEXTURE_GEN_S);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_S)"));
		glEnable( GL_TEXTURE_GEN_T);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_T)"));
		break;

	case TextureUnit::eSphereMap:
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glTexGeni( GL_S)"));
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glTexGeni( GL_T)"));
		glEnable( GL_TEXTURE_GEN_S);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_S)"));
		glEnable( GL_TEXTURE_GEN_T);
		CheckGLError( CU_T( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_T)"));
		break;
	}

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GLTextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eModelView);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (RenderSystem::UseMultiTexturing())
		{
			glActiveTexture( GL_TEXTURE0 + m_target->GetIndex());
			CheckGLError( CU_T( "GLTextureRenderer :: Remove - glActiveTexture"));
		}

		glDisable( GetTextureDimension( m_target->GetTextureType()));
		CheckGLError( CU_T( "GLTextureRenderer :: Remove - glDisable"));
	}

	glDisable( GL_TEXTURE_GEN_S);
	CheckGLError( CU_T( "GLTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_S)"));
	glDisable( GL_TEXTURE_GEN_T);
	CheckGLError( CU_T( "GLTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_T)"));
}