#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9TextureEnvironmentRenderer.h"

using namespace Castor3D;

Dx9TextureEnvironmentRenderer :: Dx9TextureEnvironmentRenderer( SceneManager * p_pSceneManager)
	:	TextureEnvironmentRenderer( p_pSceneManager)
{
	m_pfnRender = & Dx9TextureEnvironmentRenderer::_render;
}

Dx9TextureEnvironmentRenderer :: ~Dx9TextureEnvironmentRenderer()
{
}

void Dx9TextureEnvironmentRenderer :: Render()
{
	(this->*m_pfnRender)();
}

void Dx9TextureEnvironmentRenderer :: _render()
{
/*
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GetEnvironmentMode( m_target->GetMode()));
	CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: Apply - glTexEnvi GL_TEXTURE_ENV_MODE"));

	if (m_target->GetMode() == TextureEnvironment::eModeCombine)
	{
		if (m_target->GetRgbCombination() != TextureEnvironment::eRgbCombiNone)
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GetRgbCombination( m_target->GetRgbCombination()));
			CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_RGB"));
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
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GetAlphaCombination( m_target->GetAlphaCombination()));
			CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_ALPHA"));
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
*/
}

void Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep( unsigned int p_index)
{
/*
	int l_index = static_cast <int>( p_index);
	int l_RGBTextureSourceIndex = m_target->GetRgbTextureSourceIndex();

	if (m_target->GetRgbSource( p_index, l_RGBTextureSourceIndex) == TextureEnvironment::eCombiSrcTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GL_TEXTURE0 + l_RGBTextureSourceIndex);
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
		glActiveTexture( GL_TEXTURE0 + l_RGBTextureSourceIndex);
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glActiveTextureARB"));
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GetCombinationSource( m_target->GetRgbSource( p_index, l_RGBTextureSourceIndex)));
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB + l_index, GetRgbOperand( m_target->GetRgbOperand( p_index)));
	CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_RGB"));
*/
}

void Dx9TextureEnvironmentRenderer :: _applyAlphaCombinationStep( unsigned int p_index)
{
/*
	int l_index = static_cast <int>( p_index);
	int l_alphaTextureSourceIndex = m_target->GetAlphaTextureSourceIndex();

	if (m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex) == TextureEnvironment::eCombiSrcTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GL_TEXTURE0 + l_alphaTextureSourceIndex);
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
		glActiveTexture( GL_TEXTURE0 + l_alphaTextureSourceIndex);
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyAlphaCombinationStep - glActiveTexture"));
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GetCombinationSource( m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex)));
		CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + l_index, GetAlphaOperand( m_target->GetAlphaOperand( p_index)));
	CheckDxError( CU_T( "Dx9TextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_ALPHA"));
*/
}