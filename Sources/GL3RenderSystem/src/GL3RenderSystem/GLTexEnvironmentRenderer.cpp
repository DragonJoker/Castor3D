//******************************************************************************
#include "PrecompiledHeader.h"

#include "GLTexEnvironmentRenderer.h"
#include "Module_GL.h"
//#include <Castor3D/material/TextureEnvironment.h>
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void GLTextureEnvironmentRenderer :: Apply()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GLRenderSystem::GetEnvironmentMode( m_target->GetMode()));
	CheckGLError( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_TEXTURE_ENV_MODE");

	if (m_target->GetMode() == EMCombine)
	{
		if (m_target->GetRGBCombination() != CCNone)
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GLRenderSystem::GetRGBCombination( m_target->GetRGBCombination()));
			CheckGLError( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_RGB");
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
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GLRenderSystem::GetAlphaCombination( m_target->GetAlphaCombination()));
			CheckGLError( "GLTextureEnvironmentRenderer :: Apply - glTexEnvi GL_COMBINE_ALPHA");
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

//******************************************************************************

void GLTextureEnvironmentRenderer :: _applyRGBCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_RGBTextureSourceIndex = m_target->GetRGBTextureSourceIndex();

	if (m_target->GetRGBSource( p_index, l_RGBTextureSourceIndex) == CSTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GL_TEXTURE0_ARB + l_RGBTextureSourceIndex);
		CheckGLError( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB");
		glActiveTextureARB( GL_TEXTURE0_ARB + l_RGBTextureSourceIndex);
		CheckGLError( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_TEXTURE0_ARB");
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GLRenderSystem::GetCombinationSource( m_target->GetRGBSource( p_index, l_RGBTextureSourceIndex)));
		CheckGLError( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_SOURCE0_RGB");
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB + l_index, GLRenderSystem::GetRGBOperand( m_target->GetRGBOperand( p_index)));
	CheckGLError( "GLTextureEnvironmentRenderer :: _applyRGBCombinationStep - glTexEnvi GL_OPERAND0_RGB");
}

//******************************************************************************

void GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_alphaTextureSourceIndex = m_target->GetAlphaTextureSourceIndex();

	if (m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex) == CSTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GL_TEXTURE0_ARB + l_alphaTextureSourceIndex);
		CheckGLError( "GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep - glTexEnvi GL_SOURCE0_ALPHA");
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GLRenderSystem::GetCombinationSource( m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex)));
		CheckGLError( "GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep - glTexEnvi GL_SOURCE0_ALPHA");
	}

	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + l_index, GLRenderSystem::GetAlphaOperand( m_target->GetAlphaOperand( p_index)));
	CheckGLError( "GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep - glTexEnvi GL_OPERAND0_ALPHA");
}

//******************************************************************************