#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlTextureRenderer.h"
#include "OpenGlCommon/GlRenderSystem.h"

using namespace Castor3D;

void GlTextureRenderer :: Cleanup()
{
	(this->*m_pfnCleanup)();
}

bool GlTextureRenderer :: Initialise()
{
	m_pfnCleanup	= & GlTextureRenderer::_cleanup;
	m_pfnInitialise	= & GlTextureRenderer::_initialise;
	m_pfnRender		= & GlTextureRenderer::_render;
	m_pfnEndRender	= & GlTextureRenderer::_endRender;

	return (this->*m_pfnInitialise)();
}

void GlTextureRenderer :: Render()
{
	(this->*m_pfnRender)();
}

void GlTextureRenderer :: EndRender()
{
	(this->*m_pfnEndRender)();
}

void  GlTextureRenderer :: _cleanup()
{
	if (glIsTexture( m_glTexName))
	{
		CheckGlError( glDeleteTextures( 1, & m_glTexName), CU_T( "GlTextureRenderer :: Cleanup - glDeleteTextures"));
	}
}

bool GlTextureRenderer :: _initialise()
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
	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_WRAP_T, GlEnum::Get( m_target->GetWrapMode( 1))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_WRAP_T"));

	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_MIN_FILTER, GlEnum::Get( m_target->GetInterpolationMode( 0))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MIN_FILTER"));
	CheckGlError( glTexParameteri( l_textureType, GL_TEXTURE_MAG_FILTER, GlEnum::Get( m_target->GetInterpolationMode( 1))), CU_T( "GlTextureRenderer :: Initialise - glTexParameteri - GL_TEXTURE_MAG_FILTER"));

	CheckGlError( glTexImage2D( l_textureType, 0, GlEnum::Get( m_target->GetPixelFormat()).Internal, m_target->GetWidth(),
								m_target->GetHeight(), 0, GlEnum::Get( m_target->GetPixelFormat()).Format,
								GlEnum::Get( m_target->GetPixelFormat()).Type, m_target->GetImagePixels()),
				  CU_T( "GlTextureRenderer :: Initialise - glTexImage2D"));

	if (glGenerateMipmap == NULL)
	{
		CheckGlError( glGenerateMipmapEXT( l_textureType), CU_T( "GlTextureRenderer :: Initialise - glGenerateMipmapEXT"));
	}
	else
	{
		CheckGlError( glGenerateMipmap( l_textureType), CU_T( "GlTextureRenderer :: Initialise - glGenerateMipmapEXT"));
	}

	return true;
}

void GlTextureRenderer :: _render()
{
	if (RenderSystem::UseMultiTexturing())
	{
		CheckGlError( glActiveTexture( GL_TEXTURE0 + m_target->GetIndex()), CU_T( "GlTextureRenderer :: Apply - glActiveTexture"));
		CheckGlError( glBindTexture( GlEnum::Get( m_target->GetTextureType()), m_glTexName), CU_T( "GlTextureRenderer :: Apply - glBindTexture"));
		CheckGlError( glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_target->GetPrimaryColour().const_ptr()), CU_T( "GlTextureRenderer :: Apply - glTexEnvfv"));

		if (m_target->GetRgbMode() != TextureUnit::eRgbModeNone || m_target->GetAlpMode() != TextureUnit::eAlphaModeNone)
		{
			CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE), CU_T( "GlTextureRenderer :: Apply - glTexEnvi GL_TEXTURE_ENV_MODE"));

			if (m_target->GetRgbMode() != TextureUnit::eRgbModeNone)
			{
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GlEnum::Get( m_target->GetRgbMode())), CU_T( "GlTextureRenderer :: Apply - glTexEnvi RgbMode"));
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GlEnum::Get( m_target->GetRgbArgument( 0))), CU_T( "GlTextureRenderer :: Apply - glTexEnvi RgbArgument 0"));
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GlEnum::Get( m_target->GetRgbArgument( 1))), CU_T( "GlTextureRenderer :: Apply - glTexEnvi RgbArgument 1"));
        		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR), CU_T( "GlTextureRenderer :: Apply - glTexEnvi GL_OPERAND0_RGB"));
        		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR), CU_T( "GlTextureRenderer :: Apply - glTexEnvi GL_OPERAND1_RGB"));
			}

			if (m_target->GetAlpMode() != TextureUnit::eAlphaModeNone)
			{
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GlEnum::Get( m_target->GetAlpMode())), CU_T( "GlTextureRenderer :: Apply - glTexEnvi AlpMode"));
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GlEnum::Get( m_target->GetAlpArgument( 0))), CU_T( "GlTextureRenderer :: Apply - glTexEnvi AlpArgument 0"));
				CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GlEnum::Get( m_target->GetAlpArgument( 1))), CU_T( "GlTextureRenderer :: Apply - glTexEnvi AlpArgument 1"));
        		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA), CU_T( "GlTextureRenderer :: Apply - glTexEnvi GL_OPERAND0_ALPHA"));
        		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA), CU_T( "GlTextureRenderer :: Apply - glTexEnvi GL_OPERAND1_ALPHA"));
			}
		}
		else
		{
			m_target->GetEnvironment()->Render( eTriangles);
		}
	}
	else
	{
		CheckGlError( glBindTexture( GlEnum::Get( m_target->GetTextureType()), m_glTexName), CU_T( "GlTextureRenderer :: Apply - glBindTexture"));
	}

	CheckGlError( glEnable( GlEnum::Get( m_target->GetTextureType())), CU_T( "GlTextureRenderer :: Apply - glEnable"));

	if (m_target->GetTextureMapMode() != TextureUnit::eNone)
	{
		if (m_target->GetTextureMapMode() == TextureUnit::eReflexionMap)
		{
			CheckGlError( glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP), CU_T( "GlTextureRenderer :: Apply - glTexGeni( GL_S)"));
			CheckGlError( glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP), CU_T( "GlTextureRenderer :: Apply - glTexGeni( GL_T)"));
		}
		else if (m_target->GetTextureMapMode() == TextureUnit::eSphereMap)
		{
			CheckGlError( glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP), CU_T( "GlTextureRenderer :: Apply - glTexGeni( GL_S)"));
			CheckGlError( glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP), CU_T( "GlTextureRenderer :: Apply - glTexGeni( GL_T)"));
		}

		CheckGlError( glEnable( GL_TEXTURE_GEN_S), CU_T( "GlTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_S)"));
		CheckGlError( glEnable( GL_TEXTURE_GEN_T), CU_T( "GlTextureRenderer :: Apply - glEnable( GL_TEXTURE_GEN_T)"));
	}

	if (m_target->GetAlphaFunc() != TextureUnit::eAlphaAlways)
	{
		CheckGlError( glEnable( GL_ALPHA_TEST), CU_T( "GlTextureRenderer :: Apply - glEnable( GL_ALPHA_TEST)"));
		CheckGlError( glAlphaFunc( GlEnum::Get( m_target->GetAlphaFunc()), m_target->GetAlphaValue()), CU_T( "GlTextureRenderer :: Apply - glAlphaFunc"));
	}

	Pipeline::MatrixMode( Pipeline::eMATRIX_MODE( Pipeline::eMatrixTexture0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
	//TODO : rotations de texture
}

void GlTextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);

	if (m_target->GetAlphaFunc() != TextureUnit::eAlphaAlways)
	{
		CheckGlError( glDisable( GL_ALPHA_TEST), CU_T( "GlTextureRenderer :: Remove - glDisable( GL_ALPHA_TEST)"));
	}

	if (RenderSystem::UseMultiTexturing())
	{
		CheckGlError( glActiveTexture( GL_TEXTURE0 + m_target->GetIndex()), CU_T( "GlTextureRenderer :: Remove - glActiveTexture"));
	}

	CheckGlError( glDisable( GlEnum::Get( m_target->GetTextureType())), CU_T( "GlTextureRenderer :: Remove - glDisable"));

	if (m_target->GetTextureMapMode() != TextureUnit::eNone)
	{
		CheckGlError( glDisable( GL_TEXTURE_GEN_S), CU_T( "GlTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_S)"));
		CheckGlError( glDisable( GL_TEXTURE_GEN_T), CU_T( "GlTextureRenderer :: Remove - glDisable( GL_TEXTURE_GEN_T)"));
	}
}
