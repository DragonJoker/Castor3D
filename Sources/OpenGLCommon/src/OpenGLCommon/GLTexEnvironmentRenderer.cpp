#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLTexEnvironmentRenderer.h"

using namespace Castor3D;

void GLTextureEnvironmentRenderer :: Render()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GetEnvironmentMode( m_target->GetMode()));
	CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_TEXTURE_ENV_MODE"));

	if (m_target->GetMode() == EMCombine)
	{
		if (m_target->GetRGBCombination() != CCNone)
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GetRGBCombination( m_target->GetRGBCombination()));
			CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_RGB"));
			_applyRGBCombinationStep( 0);

			if (m_target->GetRGBCombination() != CCReplace)
			{
				_applyRGBCombinationStep( 1);

				if (m_target->GetRGBCombination() == CCInterpolate)
				{
					_applyRGBCombinationStep( 2);
				}
			}
		}

		if (m_target->GetAlphaCombination() != ACNone)
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GetAlphaCombination( m_target->GetAlphaCombination()));
			CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_ALPHA"));
			_applyAlphaCombinationStep( 0);

			if (m_target->GetAlphaCombination() != ACReplace)
			{
				_applyAlphaCombinationStep( 1);

				if (m_target->GetAlphaCombination() == ACInterpolate)
				{
					_applyAlphaCombinationStep( 2);
				}
			}
		}
	}
}

void GLTextureEnvironmentRenderer :: _applyRGBCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_RGBTextureSourceIndex = m_target->GetRGBTextureSourceIndex();

	if (m_target->GetRGBSource( p_index, l_RGBTextureSourceIndex) == CSTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GL_TEXTURE0 + l_RGBTextureSourceIndex);
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
		glActiveTexture( GL_TEXTURE0 + l_RGBTextureSourceIndex);
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glActiveTextureARB"));
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GetCombinationSource( m_target->GetRGBSource( p_index, l_RGBTextureSourceIndex)));
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB"));
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB + l_index, GetRGBOperand( m_target->GetRGBOperand( p_index)));
	CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_RGB"));
}

void GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_alphaTextureSourceIndex = m_target->GetAlphaTextureSourceIndex();

	if (m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex) == CSTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GL_TEXTURE0 + l_alphaTextureSourceIndex);
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
		glActiveTexture( GL_TEXTURE0 + l_alphaTextureSourceIndex);
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep - glActiveTexture"));
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GetCombinationSource( m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex)));
		CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_ALPHA"));
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + l_index, GetAlphaOperand( m_target->GetAlphaOperand( p_index)));
	CheckGLError( CU_T( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_ALPHA"));
}