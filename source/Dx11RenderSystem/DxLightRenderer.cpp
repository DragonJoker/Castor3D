#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxLightRenderer.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxLightRenderer :: DxLightRenderer( DxRenderSystem * p_pRenderSystem )
	:	LightRenderer		( p_pRenderSystem	)
	,	m_bChanged			( true				)
	,	m_iIndex			( -1				)
	,	m_iD3dIndex			( -1				)
	,	m_iD3dPreviousIndex	( -1				)
{
}

void DxLightRenderer :: Initialise()
{
}

void DxLightRenderer :: Enable()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );
	m_iIndex = l_pRenderSystem->LockLight();
}

void DxLightRenderer :: Disable()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );

	if( m_iIndex >= 0 )
	{
		l_pRenderSystem->UnlockLight( m_iIndex );
	}

	m_iIndex = -1;
}

void DxLightRenderer :: ApplyAmbient()
{
	if( m_iIndex >= 0 )
	{
		m_pAmbient = m_target->GetAmbient();
	}
}

void DxLightRenderer :: ApplyDiffuse()
{
	if( m_iIndex >= 0 )
	{
		m_pDiffuse = m_target->GetDiffuse();
	}
}

void DxLightRenderer :: ApplySpecular()
{
	if( m_iIndex >= 0 )
	{
		m_pSpecular = m_target->GetSpecular();
	}
}

void DxLightRenderer :: ApplyPosition()
{
	if( m_iIndex >= 0 )
	{
#if CASTOR_USE_DOUBLE
		m_pPosition = Point4f( float( m_target->GetPositionType()[0]), float( m_target->GetPositionType()[1]), float( m_target->GetPositionType()[2]), float( m_target->GetPositionType()[3]));
#else
		m_pPosition = m_target->GetPositionType();
#endif
	}
}

void DxLightRenderer :: ApplyOrientation()
{
	if( m_iIndex >= 0 )
	{
#if CASTOR_USE_DOUBLE
		m_pOrientation = Matrix4x4f( m_target->GetParent()->GetTransformationMatrix().const_ptr() );
#else
		m_pOrientation = m_target->GetParent()->GetTransformationMatrix();
#endif
		Point3f l_ptDirection;
		MtxUtils::mult( m_pOrientation, l_ptDirection );
	}
}

void DxLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if( m_iIndex >= 0 )
	{
		m_pAttenuation[0] = p_constant;
	}
}

void DxLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if( m_iIndex >= 0 )
	{
		m_pAttenuation[1] = p_linear;
	}
}

void DxLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if( m_iIndex >= 0 )
	{
		m_pAttenuation[2] = p_quadratic;
	}
}

void DxLightRenderer :: ApplyExponent( float p_fExponent)
{
	if( m_iIndex >= 0 )
	{
		m_pExponent = p_fExponent;
	}
}

void DxLightRenderer :: ApplyCutOff( float p_fCutOff)
{
	if( m_iIndex >= 0 )
	{
		m_pCutOff = p_fCutOff;
	}
}

void DxLightRenderer :: EnableShader( ShaderProgramBase * p_pProgram )
{
	if( m_iIndex >= 0 )
	{
		DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

		if( l_pProgram )
		{
			m_iD3dIndex = l_pProgram->AssignLight();

			if (m_iD3dIndex != -1 && (m_bChanged || m_iD3dIndex != m_iD3dPreviousIndex))
			{
				l_pProgram->SetLightAmbient(		m_iD3dIndex, m_pAmbient		);
				l_pProgram->SetLightDiffuse(		m_iD3dIndex, m_pDiffuse		);
				l_pProgram->SetLightSpecular(		m_iD3dIndex, m_pSpecular	);
				l_pProgram->SetLightPosition(		m_iD3dIndex, m_pPosition	);
				l_pProgram->SetLightOrientation(	m_iD3dIndex, m_pOrientation	);
				l_pProgram->SetLightAttenuation(	m_iD3dIndex, m_pAttenuation	);
				l_pProgram->SetLightExponent(		m_iD3dIndex, m_pExponent	);
				l_pProgram->SetLightCutOff(			m_iD3dIndex, m_pCutOff		);
				m_iD3dPreviousIndex = m_iD3dIndex;
			}
		}
	}
}

void DxLightRenderer :: DisableShader( ShaderProgramBase * p_pProgram )
{
	DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

	if( m_iD3dIndex != -1 && l_pProgram )
	{
		l_pProgram->FreeLight( m_iD3dIndex);
	}
}

void DxLightRenderer :: ApplyAmbientShader()
{
}

void DxLightRenderer :: ApplyDiffuseShader()
{
}

void DxLightRenderer :: ApplySpecularShader()
{
}

void DxLightRenderer :: ApplyPositionShader()
{
}

void DxLightRenderer :: ApplyOrientationShader()
{
}

void DxLightRenderer :: ApplyConstantAttShader( float CU_PARAM_UNUSED( p_constant ) )
{
}

void DxLightRenderer :: ApplyLinearAttShader( float CU_PARAM_UNUSED( p_linear ) )
{
}

void DxLightRenderer :: ApplyQuadraticAttShader( float CU_PARAM_UNUSED( p_quadratic ) )
{
}

void DxLightRenderer :: ApplyExponentShader( float CU_PARAM_UNUSED( p_exponent ) )
{
}

void DxLightRenderer :: ApplyCutOffShader( float CU_PARAM_UNUSED( p_cutOff ) )
{
}
