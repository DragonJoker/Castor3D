#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlLightRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

#include <Castor3D/SceneNode.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlLightRenderer :: GlLightRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	LightRenderer		( p_pRenderSystem			)
	,	m_eIndex			( eGL_LIGHT_INDEX_INVALID	)
	,	m_bChanged			( true						)
	,	m_iGlIndex			( eGL_INVALID_INDEX			)
	,	m_iGlPreviousIndex	( eGL_INVALID_INDEX			)
	,	m_gl				( p_gl						)
{
}

void GlLightRenderer :: Initialise()
{
	m_bChanged = true;
	m_mapChangedByProgram.clear();
	m_iGlPreviousIndex = eGL_INVALID_INDEX;
}

void GlLightRenderer :: Enable()
{
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( m_pRenderSystem );
	m_eIndex = eGL_LIGHT_INDEX( l_pRenderSystem->LockLight() );

	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Enable( m_eIndex );
	}
}

void GlLightRenderer :: Disable()
{
	m_pRenderSystem->UnlockLight( m_eIndex );

	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Disable( m_eIndex );
		m_eIndex = eGL_LIGHT_INDEX_INVALID;
	}
}

void GlLightRenderer :: ApplyAmbient()
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_AMBIENT, m_target->GetAmbient().const_ptr() );
	}
}

void GlLightRenderer :: ApplyDiffuse()
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_DIFFUSE, m_target->GetDiffuse().const_ptr() );
	}
}

void GlLightRenderer :: ApplySpecular()
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPECULAR, m_target->GetSpecular().const_ptr() );
	}
}

void GlLightRenderer :: ApplyPosition()
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_POSITION, m_target->GetPositionType().const_ptr() );
	}
}

void GlLightRenderer :: ApplyOrientation()
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
#if CASTOR_USE_DOUBLE
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_DIRECTION, Matrix4x4f( m_target->GetParent()->GetTransformationMatrix().const_ptr() ).const_ptr() );
#else
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_DIRECTION, m_target->GetParent()->GetTransformationMatrix().const_ptr() );
#endif
	}
}

void GlLightRenderer :: ApplyConstantAtt( float p_constant )
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_CONSTANT_ATTENUATION, p_constant );
	}
}

void GlLightRenderer :: ApplyLinearAtt( float p_linear )
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_LINEAR_ATTENUATION, p_linear );
	}
}

void GlLightRenderer :: ApplyQuadraticAtt( float p_quadratic )
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_QUADRATIC_ATTENUATION, p_quadratic );
	}
}

void GlLightRenderer :: ApplyExponent( float p_exponent )
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_EXPONENT, p_exponent );
	}
}

void GlLightRenderer :: ApplyCutOff( float p_cutOff )
{
	if( m_eIndex != eGL_LIGHT_INDEX_INVALID )
	{
		m_gl.Light( m_eIndex, eGL_LIGHT_COMPONENT_SPOT_CUTOFF, p_cutOff );
	}
}

void GlLightRenderer :: EnableShader( ShaderProgramBase * p_pProgram )
{
	if( p_pProgram )
	{
		m_iGlIndex = p_pProgram->AssignLight();

		PointerBoolMapIt l_it = m_mapChangedByProgram.find( p_pProgram );

		if( l_it == m_mapChangedByProgram.end() )
		{
			m_mapChangedByProgram.insert( std::make_pair( p_pProgram, true ) );
			l_it = m_mapChangedByProgram.find( p_pProgram );
		}

		if( m_bChanged )
		{
			for( PointerBoolMapIt l_it2 = m_mapChangedByProgram.begin(); l_it2 != m_mapChangedByProgram.end(); ++l_it2 )
			{
				l_it2->second = true;
			}

			m_bChanged = false;
		}

		if( m_iGlIndex != eGL_INVALID_INDEX && l_it->second )
		{
			p_pProgram->SetLightAmbient(		m_iGlIndex,	m_pAmbient		);
			p_pProgram->SetLightDiffuse(		m_iGlIndex,	m_pDiffuse		);
			p_pProgram->SetLightSpecular(		m_iGlIndex,	m_pSpecular		);
			p_pProgram->SetLightPosition(		m_iGlIndex,	m_pPosition		);
			p_pProgram->SetLightOrientation(	m_iGlIndex,	m_pOrientation	);
			p_pProgram->SetLightAttenuation(	m_iGlIndex,	m_pAttenuation	);
			p_pProgram->SetLightExponent(		m_iGlIndex,	m_pExponent		);
			p_pProgram->SetLightCutOff(			m_iGlIndex,	m_pCutOff		);
			l_it->second = false;
			SetGlPreviousIndex( m_iGlIndex );
		}
	}
}

void GlLightRenderer :: DisableShader( ShaderProgramBase * p_pProgram )
{
	if( m_iGlIndex != eGL_LIGHT_INDEX_INVALID )
	{
		if( p_pProgram )
		{
			p_pProgram->FreeLight( m_iGlIndex );
		}
	}
}

void GlLightRenderer :: ApplyAmbientShader()
{
	m_bChanged |= m_pAmbient != m_target->GetAmbient();
	m_pAmbient = m_target->GetAmbient();
}

void GlLightRenderer :: ApplyDiffuseShader()
{
	m_bChanged |= m_pDiffuse != m_target->GetDiffuse();
	m_pDiffuse = m_target->GetDiffuse();
}

void GlLightRenderer :: ApplySpecularShader()
{
	m_bChanged |= m_pSpecular != m_target->GetSpecular();
	m_pSpecular = m_target->GetSpecular();
}

void GlLightRenderer :: ApplyPositionShader()
{
	m_bChanged |= m_pPosition != m_target->GetPositionType();
	m_pPosition = m_target->GetPositionType();
}

void GlLightRenderer :: ApplyOrientationShader()
{
	m_bChanged |= m_pOrientation != m_target->GetParent()->GetTransformationMatrix();
	m_pOrientation = m_target->GetParent()->GetTransformationMatrix();
}

void GlLightRenderer :: ApplyConstantAttShader( float p_constant)
{
	m_bChanged |= m_pAttenuation[0] != p_constant;
	m_pAttenuation[0] = p_constant;
}

void GlLightRenderer :: ApplyLinearAttShader( float p_linear)
{
	m_bChanged |= m_pAttenuation[1] != p_linear;
	m_pAttenuation[1] = p_linear;
}

void GlLightRenderer :: ApplyQuadraticAttShader( float p_quadratic)
{
	m_bChanged |= m_pAttenuation[2] != p_quadratic;
	m_pAttenuation[2] = p_quadratic;
}

void GlLightRenderer :: ApplyExponentShader( float p_exponent)
{
	m_bChanged |= m_pExponent != p_exponent;
	m_pExponent = p_exponent;
}

void GlLightRenderer :: ApplyCutOffShader( float p_cutOff)
{
	m_bChanged |= m_pCutOff != p_cutOff;
	m_pCutOff = p_cutOff;
}
