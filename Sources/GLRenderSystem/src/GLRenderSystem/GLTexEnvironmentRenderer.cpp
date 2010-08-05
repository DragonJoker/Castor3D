//******************************************************************************
#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLTexEnvironmentRenderer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
//#include <Castor3D/material/TextureEnvironment.h>
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void GLTextureEnvironmentRenderer :: Apply()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GLRenderSystem::GetEnvironmentMode( m_target->GetMode()));
	if (m_target->GetMode() == EMCombine)
	{
		if (m_target->GetRGBCombination() != CCNone)
		{
			glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GLRenderSystem::GetRGBCombination( m_target->GetRGBCombination()));
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
		glActiveTextureARB( GL_TEXTURE0_ARB + l_RGBTextureSourceIndex);
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB + l_index, GLRenderSystem::GetCombinationSource( m_target->GetRGBSource( p_index, l_RGBTextureSourceIndex)));
	}
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB + l_index, GLRenderSystem::GetRGBOperand( m_target->GetRGBOperand( p_index)));
}

//******************************************************************************

void GLTextureEnvironmentRenderer :: _applyAlphaCombinationStep( unsigned int p_index)
{
	int l_index = static_cast <int>( p_index);
	int l_alphaTextureSourceIndex = m_target->GetAlphaTextureSourceIndex();
	if (m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex) == CSTexture)
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GL_TEXTURE0_ARB + l_alphaTextureSourceIndex);
	}
	else
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + l_index, GLRenderSystem::GetCombinationSource( m_target->GetAlphaSource( p_index, l_alphaTextureSourceIndex)));
	}
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + l_index, GLRenderSystem::GetAlphaOperand( m_target->GetAlphaOperand( p_index)));
}

//******************************************************************************