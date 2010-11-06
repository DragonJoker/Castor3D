#include "PrecompiledHeader.h"

#include "GLTextureRenderer.h"
#include "GLContext.h"
#include "GLTexEnvironmentRenderer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLPipeline.h"

using namespace Castor3D;

void  GLTextureRenderer :: Cleanup()
{
}

bool GLTextureRenderer :: Initialise()
{
//	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glGenTextures( 1, & m_texGLName);
	CheckGLError( "GLTextureRenderer :: Initialise - glGenTextures");

	if (m_texGLName == 0)
	{
		Log::LogMessage( "GLTextureRenderer :: Initialise - Not a valid texture name");
		return false;
	}
	int l_textureType = GLRenderSystem::GetTextureDimension( m_target->GetTextureType());
	glBindTexture( l_textureType, m_texGLName);
	CheckGLError( "GLTextureRenderer :: Initialise - glBindTexture");

	glTexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CheckGLError( "GLTextureRenderer :: Initialise - glTexParameteri - 1");
	glTexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CheckGLError( "GLTextureRenderer :: Initialise - glTexParameteri - 2");

	glTexParameteri( l_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	CheckGLError( "GLTextureRenderer :: Initialise - glTexParameteri - 1");
	glTexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CheckGLError( "GLTextureRenderer :: Initialise - glTexParameteri - 2");
/**/
	glTexImage2D( l_textureType, 0, 4, m_target->GetWidth(),
				  m_target->GetHeight(), 0, GL_RGBA, 
				  GL_UNSIGNED_BYTE, m_target->GetImagePixels());
	CheckGLError( "GLTextureRenderer :: Initialise - glTexImage2D");
/**/
/**/
	gluBuild2DMipmaps( l_textureType, GL_RGBA, m_target->GetWidth(),
					   m_target->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE,
					   m_target->GetImagePixels());
	CheckGLError( "GLTextureRenderer :: Initialise - gluBuild2DMipmaps");
	return true;
/**/
}

void GLTextureRenderer :: Apply()
{
	if (RenderSystem::UseMultiTexturing())
	{
		glActiveTextureARB( GL_TEXTURE0_ARB + m_target->GetIndex());
		CheckGLError( "GLTextureRenderer :: Apply - glActiveTextureARB");
		glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_target->GetPrimaryColour());
		CheckGLError( "GLTextureRenderer :: Apply - glTexEnvfv");
		glBindTexture( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( "GLTextureRenderer :: Apply - glBindTexture");
		m_target->GetEnvironment()->Apply( DTTriangles);
	}
	else
	{
		glBindTexture( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( "GLTextureRenderer :: Apply - glBindTexture");
	}

	glEnable( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()));
	CheckGLError( "GLTextureRenderer :: Apply - glEnable");

	if (m_target->GetTextureMapMode() == TextureUnit::eReflexionMap)
	{
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		CheckGLError( "GLTextureRenderer :: Apply - glTexGeni( GL_S)");
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		CheckGLError( "GLTextureRenderer :: Apply - glTexGeni( GL_T)");
		glEnable( GL_TEXTURE_GEN_S);
		CheckGLError( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_S)");
		glEnable( GL_TEXTURE_GEN_T);
		CheckGLError( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_T)");
	}
	else if (m_target->GetTextureMapMode() == TextureUnit::eSphereMap)
	{

		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		CheckGLError( "GLTextureRenderer :: Apply - glTexGeni( GL_S)");
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		CheckGLError( "GLTextureRenderer :: Apply - glTexGeni( GL_T)");
		glEnable( GL_TEXTURE_GEN_S);
		CheckGLError( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_S)");
		glEnable( GL_TEXTURE_GEN_T);
		CheckGLError( "GLTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_T)");
	}

	Pipeline::MatrixMode( Pipeline::eTexture);
	Pipeline::LoadIdentity();
}

void GLTextureRenderer :: Remove()
{
	Pipeline::MatrixMode( Pipeline::eModelView);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (GLRenderSystem::UseMultiTexturing())
		{
			glActiveTextureARB( GL_TEXTURE0_ARB + m_target->GetIndex());
			CheckGLError( "GLTextureRenderer :: Remove - glActiveTextureARB");
		}

		glDisable( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()));
		CheckGLError( "GLTextureRenderer :: Remove - glDisable");
	}

	glDisable( GL_TEXTURE_GEN_S);
	CheckGLError( "GLTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_S)");
	glDisable( GL_TEXTURE_GEN_T);
	CheckGLError( "GLTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_T)");
}