#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlTexEnvironmentRenderer.h"

using namespace Castor3D;

GlTextureEnvironmentRenderer :: GlTextureEnvironmentRenderer( SceneManager * p_pSceneManager)
	:	TextureEnvironmentRenderer( p_pSceneManager)
{
	m_pfnRender = & GlTextureEnvironmentRenderer::_render;
}

GlTextureEnvironmentRenderer :: ~GlTextureEnvironmentRenderer()
{
}

void GlTextureEnvironmentRenderer :: Render()
{
	(this->*m_pfnRender)();
}

void GlTextureEnvironmentRenderer :: _render()
{
	CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GlEnum::Get( m_target->GetMode())), CU_T( "GlTextureEnvironmentRenderer :: Apply - glTexEnvi GL_TEXTURE_ENV_MODE"));

	if (m_target->GetMode() == TextureEnvironment::eModeCombine)
	{
		if (m_target->GetRgbCombination() != TextureEnvironment::eRgbCombiNone)
		{
			CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GlEnum::Get( m_target->GetRgbCombination())), CU_T( "GlTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_RGB"));
			_applyRGBCombinationStep( 0);

			if (m_target->GetRgbCombination() != TextureEnvironment::eRgbCombiReplace)
			{
				_applyRGBCombinationStep( 1);

				if (m_target->GetRgbCombination() == TextureEnvironment::eRgbCombiInterpolate)
				{
					_applyRGBCombinationStep( 2);
				}
			}
		}

		if (m_target->GetAlphaCombination() != TextureEnvironment::eAlphaCombiNone)
		{
			CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GlEnum::Get( m_target->GetAlphaCombination())), CU_T( "GlTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_ALPHA"));
			_applyAlphaCombinationStep( 0);

			if (m_target->GetAlphaCombination() != TextureEnvironment::eAlphaCombiReplace)
			{
				_applyAlphaCombinationStep( 1);

				if (m_target->GetAlphaCombination() == TextureEnvironment::eAlphaCombiInterpolate)
				{
					_applyAlphaCombinationStep( 2);
				}
			}
		}
	}
}

void GlTextureEnvironmentRenderer :: _applyRGBCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_RGBTextureSourceIndex = m_target->GetRgbTextureSourceIndex();

	if (m_target->GetRgbSource( p_index, l_RGBTextureSourceIndex) == TextureEnvironment::eCombiSrcTexture)
	{
		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GL_TEXTURE0 + l_RGBTextureSourceIndex), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
		CheckGlError( glActiveTexture( GL_TEXTURE0 + l_RGBTextureSourceIndex), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glActiveTextureARB"));
	}
	else
	{
		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GlEnum::Get( m_target->GetRgbSource( p_index, l_RGBTextureSourceIndex))), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
	}

	CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB + l_index, GlEnum::Get( m_target->GetRgbOperand( p_index))), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_RGB"));
}

void GlTextureEnvironmentRenderer :: _applyAlphaCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_alphaTextureSourceIndex = m_target->GetAlphaTextureSourceIndex();

	if (m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex) == TextureEnvironment::eCombiSrcTexture)
	{
		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GL_TEXTURE0 + l_alphaTextureSourceIndex), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
		CheckGlError( glActiveTexture( GL_TEXTURE0 + l_alphaTextureSourceIndex), CU_T( "GlTextureEnvironmentRenderer :: _applyAlphaCombinationStep - glActiveTexture"));
	}
	else
	{
		CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GlEnum::Get( m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex))), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
	}

	CheckGlError( glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + l_index, GlEnum::Get( m_target->GetAlphaOperand( p_index))), CU_T( "GlTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_ALPHA"));
}
