//******************************************************************************
#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLTextureRenderer.h"
#include "../../include/GLRenderSystem/GLContext.h"
#include "../../include/GLRenderSystem/GLTexEnvironmentRenderer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
#include "../../include/GLRenderSystem/GLRenderSystem.h"

//#include <Castor3D/material/TextureEnvironment.h>
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void  GLTextureRenderer :: Cleanup()
{
}

//******************************************************************************

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

//******************************************************************************

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
		m_target->GetEnvironment()->Apply();
	}
	else
	{
		glBindTexture( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()), m_texGLName);
		CheckGLError( "GLTextureRenderer :: Apply - glBindTexture");
	}

	glEnable( static_cast <GLRenderSystem *>( Root::GetRenderSystem())->GetTextureDimension( m_target->GetTextureType()));
	CheckGLError( "GLTextureRenderer :: Apply - glEnable");

	/*
	//	Génération Automatique des coordonnées de texture 
	float l_s[] = {1.0, 0.0, 0.0, 1.0};
	float l_t[] = {0.0, 1.0, 0.0, 1.0};
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv( GL_S, GL_OBJECT_PLANE, l_s);
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv( GL_T, GL_OBJECT_PLANE, l_t);
	glEnable( GL_TEXTURE_GEN_S);
	glEnable( GL_TEXTURE_GEN_T);
	/**/
	if (m_target->GetTextureMapMode() == TextureUnit::eReflexionMap)
	{
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable( GL_TEXTURE_GEN_S);
		glEnable( GL_TEXTURE_GEN_T);
	}
	else if (m_target->GetTextureMapMode() == TextureUnit::eSphereMap)
	{

		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable( GL_TEXTURE_GEN_S);
		glEnable( GL_TEXTURE_GEN_T);
	}

	glMatrixMode( GL_TEXTURE);
	CheckGLError( "GLTextureRenderer :: Apply - glMatrixMode");
	glLoadIdentity();
	CheckGLError( "GLTextureRenderer :: Apply - glLoadIdentity");
	glMatrixMode( GL_MODELVIEW);
	CheckGLError( "GLTextureRenderer :: Apply - glMatrixMode");
}

//******************************************************************************

void GLTextureRenderer :: Remove()
{
	glMatrixMode( GL_TEXTURE);
	glMatrixMode( GL_MODELVIEW);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (GLRenderSystem::UseMultiTexturing())
		{
			glActiveTextureARB( GL_TEXTURE0_ARB + m_target->GetIndex());
		}
		glDisable( GLRenderSystem::GetTextureDimension( m_target->GetTextureType()));
	}

	glDisable( GL_TEXTURE_GEN_S);
	glDisable( GL_TEXTURE_GEN_T);
}

//******************************************************************************
