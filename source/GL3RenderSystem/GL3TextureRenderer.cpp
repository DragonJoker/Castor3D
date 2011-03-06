#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3TextureRenderer.h"
#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"

using namespace Castor3D;

Gl3TextureRenderer :: Gl3TextureRenderer( SceneManager * p_pSceneManager)
	:	GlTextureRenderer( p_pSceneManager)
	,	m_uiIndex( GL_INVALID_INDEX)
{
}

Gl3TextureRenderer :: ~Gl3TextureRenderer()
{
}

bool Gl3TextureRenderer :: Initialise()
{
	m_pfnCleanup	= PVoidFunction( & Gl3TextureRenderer::_cleanup);
	m_pfnInitialise	= PBoolFunction( & Gl3TextureRenderer::_initialise);
	m_pfnRender		= PVoidFunction( & Gl3TextureRenderer::_render);
	m_pfnEndRender	= PVoidFunction( & Gl3TextureRenderer::_endRender);

	return (this->*m_pfnInitialise)();
}

void  Gl3TextureRenderer :: _cleanup()
{
	if (glIsTexture( m_glTexName))
	{
		CheckGlError( glDeleteTextures( 1, & m_glTexName), CU_T( "GlTextureRenderer :: Cleanup - glDeleteTextures"));
	}
}

bool Gl3TextureRenderer :: _initialise()
{
	CheckGlError( glGenTextures( 1, & m_glTexName), CU_T( "GlTextureRenderer :: Initialise - glGenTextures"));

	if (m_glTexName == 0)
	{
		Logger::LogError( CU_T( "GlTextureRenderer :: Initialise - Not a valid texture name"));
		return false;
	}

	int l_textureType = GlEnum::Get( m_target->GetTextureType());
	CheckGlError( glBindTexture( l_textureType, m_glTexName), CU_T( "GlTextureRenderer :: Initialise - glBindTexture"));

	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_WRAP_S, GlEnum::Get( m_target->GetWrapMode( 0))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_WRAP_S"));
	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GlEnum::Get( m_target->GetWrapMode( 0))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_WRAP_T"));

	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GlEnum::Get( m_target->GetInterpolationMode( 0))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MIN_FILTER"));
	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GlEnum::Get( m_target->GetInterpolationMode( 1))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MAG_FILTER"));

	CheckGlError( glTexImage2D(	l_textureType, 0, GlEnum::Get( m_target->GetPixelFormat()).Internal, m_target->GetWidth(),
								m_target->GetHeight(), 0, GlEnum::Get( m_target->GetPixelFormat()).Format,
								GlEnum::Get( m_target->GetPixelFormat()).Type, m_target->GetImagePixels()),
				  CU_T( "GlTextureRenderer :: Initialise - glTexImage2D"));

	if (glGenerateMipmap == NULL)
	{
		CheckGlError( glGenerateMipmapEXT( l_textureType), CU_T( "GlTextureRenderer :: Initialise - glGenerateMipmap"));
	}
	else
	{
		CheckGlError( glGenerateMipmap( l_textureType), CU_T( "GlTextureRenderer :: Initialise - glGenerateMipmap"));
	}
	
	return true;
}

void Gl3TextureRenderer :: _render()
{
	if (RenderSystem::UseMultiTexturing())
	{
		CheckGlError( glActiveTexture( GL_TEXTURE0 + m_target->GetIndex()), CU_T( "GlTextureRenderer :: Apply - glActiveTexture"));
		CheckGlError( glBindTexture( GlEnum::Get( m_target->GetTextureType()), m_glTexName), CU_T( "GlTextureRenderer :: Apply - glBindTexture"));
	}
	else
	{
		CheckGlError( glBindTexture( GlEnum::Get( m_target->GetTextureType()), m_glTexName), CU_T( "GlTextureRenderer :: Apply - glBindTexture"));
	}

	Pipeline::MatrixMode( Pipeline::eMATRIX_MODE( Pipeline::eMatrixTexture0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : transformations de texture
}

void Gl3TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);

	if (m_target->IsTextured() && m_target->TextureInitialised())
	{
		if (RenderSystem::UseMultiTexturing())
		{
			CheckGlError( glActiveTexture( GL_TEXTURE0 + m_target->GetIndex()), CU_T( "GlTextureRenderer :: Remove - glActiveTexture"));
		}
	}
}
