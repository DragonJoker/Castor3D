#include "Dx9LightRenderer.hpp"
#include "Dx9RenderSystem.hpp"

#include <Light.hpp>
#include <SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

DxLightRenderer::DxLightRenderer( DxRenderSystem * p_pRenderSystem )
	:	LightRenderer( p_pRenderSystem )
	,	m_bChanged( true )
	,	m_iIndex( -1 )
	,	m_iD3dIndex( -1 )
	,	m_iD3dPreviousIndex( -1 )
	,	m_pCutOff( 0.0f )
	,	m_pExponent( 0.0f )
{
	memset( &m_dx9Light, 0, sizeof( D3DLIGHT9 ) );
	m_dx9Light.Range	= 1000.0f;
}

void DxLightRenderer::Initialise()
{
	m_dx9Light.Type = DirectX9::Get( m_target->GetLightType() );
}

void DxLightRenderer::Enable()
{
}

void DxLightRenderer::Disable()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );

	if ( m_iIndex >= 0 )
	{
		m_pRenderSystem->UnlockLight( m_iIndex );
		l_pRenderSystem->GetDevice()->SetLight( m_iIndex, &m_dx9Light );
		l_pRenderSystem->GetDevice()->LightEnable( m_iIndex, FALSE );
	}

	m_iIndex = -1;
}

void DxLightRenderer::Bind()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );
	m_iIndex = l_pRenderSystem->LockLight();

	if ( m_iIndex >= 0 )
	{
		l_pRenderSystem->GetDevice()->SetLight( m_iIndex, &m_dx9Light );
		l_pRenderSystem->GetDevice()->LightEnable( m_iIndex, TRUE );
	}
}

void DxLightRenderer::Unbind()
{
}

void DxLightRenderer::ApplyAmbient()
{
	m_pAmbient = m_target->GetAmbient();
	memcpy( &m_dx9Light.Ambient, m_pAmbient.const_ptr(), 4 * sizeof( float ) );
}

void DxLightRenderer::ApplyDiffuse()
{
	m_pDiffuse = m_target->GetDiffuse();
	memcpy( &m_dx9Light.Diffuse, m_pDiffuse.const_ptr(), 4 * sizeof( float ) );
}

void DxLightRenderer::ApplySpecular()
{
	m_pSpecular = m_target->GetSpecular();
	memcpy( &m_dx9Light.Specular, m_pSpecular.const_ptr(), 4 * sizeof( float ) );
}

void DxLightRenderer::ApplyPosition()
{
#if CASTOR_USE_DOUBLE
	m_pPosition = Point4f( float( m_target->GetPositionType()[0] ), float( m_target->GetPositionType()[1] ), float( m_target->GetPositionType()[2] ), float( m_target->GetPositionType()[3] ) );
#else
	m_pPosition = m_target->GetPositionType();
#endif
	m_dx9Light.Type = DirectX9::Get( m_target->GetLightType() );

	if ( m_target->GetLightType() == eLIGHT_TYPE_DIRECTIONAL )
	{
		memcpy( & m_dx9Light.Direction, point::get_normalised( m_pPosition ).const_ptr(), 3 * sizeof( float ) );
	}
	else
	{
		memcpy( & m_dx9Light.Position, m_pPosition.const_ptr(), 3 * sizeof( float ) );
	}
}

void DxLightRenderer::ApplyOrientation()
{
#if CASTOR_USE_DOUBLE
	m_pOrientation = Matrix4x4f( m_target->GetParent()->GetTransformationMatrix().const_ptr() );
#else
	m_pOrientation = m_target->GetParent()->GetTransformationMatrix();
#endif
	Point3f l_ptDirection;
	MtxUtils::mult( m_pOrientation, l_ptDirection );
	memcpy( &m_dx9Light.Direction, l_ptDirection.const_ptr(), 3 * sizeof( float ) );
}

void DxLightRenderer::ApplyConstantAtt( float p_constant )
{
	m_pAttenuation[0] = p_constant;
	m_dx9Light.Attenuation0 = p_constant;
}

void DxLightRenderer::ApplyLinearAtt( float p_linear )
{
	m_pAttenuation[1] = p_linear;
	m_dx9Light.Attenuation1 = p_linear;
}

void DxLightRenderer::ApplyQuadraticAtt( float p_quadratic )
{
	m_pAttenuation[2] = p_quadratic;
	m_dx9Light.Attenuation2 = p_quadratic;
}

void DxLightRenderer::ApplyExponent( float p_fExponent )
{
	m_pExponent = p_fExponent;
	m_dx9Light.Falloff = m_pExponent;
}

void DxLightRenderer::ApplyCutOff( float p_fCutOff )
{
	m_pCutOff = p_fCutOff;
	m_dx9Light.Theta = m_pCutOff;
}

void DxLightRenderer::EnableShader( ShaderProgramBase * p_pProgram )
{
	DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

	if ( l_pProgram )
	{
		m_iD3dIndex = l_pProgram->AssignLight();

		if ( m_iD3dIndex != -1 && ( m_bChanged || m_iD3dIndex != m_iD3dPreviousIndex ) )
		{
			l_pProgram->SetLightAmbient(	m_iD3dIndex, m_pAmbient	);
			l_pProgram->SetLightDiffuse(	m_iD3dIndex, m_pDiffuse	);
			l_pProgram->SetLightSpecular(	m_iD3dIndex, m_pSpecular	);
			l_pProgram->SetLightPosition(	m_iD3dIndex, m_pPosition	);
			l_pProgram->SetLightOrientation(	m_iD3dIndex, m_pOrientation	);
			l_pProgram->SetLightAttenuation(	m_iD3dIndex, m_pAttenuation	);
			l_pProgram->SetLightExponent(	m_iD3dIndex, m_pExponent	);
			l_pProgram->SetLightCutOff(	m_iD3dIndex, m_pCutOff	);
			m_iD3dPreviousIndex = m_iD3dIndex;
		}
	}
}

void DxLightRenderer::DisableShader( ShaderProgramBase * p_pProgram )
{
	DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

	if ( m_iD3dIndex != -1 && l_pProgram )
	{
		l_pProgram->FreeLight( m_iD3dIndex );
	}
}

void DxLightRenderer::ApplyAmbientShader()
{
}

void DxLightRenderer::ApplyDiffuseShader()
{
}

void DxLightRenderer::ApplySpecularShader()
{
}

void DxLightRenderer::ApplyPositionShader()
{
}

void DxLightRenderer::ApplyOrientationShader()
{
}

void DxLightRenderer::ApplyConstantAttShader( float CU_PARAM_UNUSED( p_constant ) )
{
}

void DxLightRenderer::ApplyLinearAttShader( float CU_PARAM_UNUSED( p_linear ) )
{
}

void DxLightRenderer::ApplyQuadraticAttShader( float CU_PARAM_UNUSED( p_quadratic ) )
{
}

void DxLightRenderer::ApplyExponentShader( float CU_PARAM_UNUSED( p_exponent ) )
{
}

void DxLightRenderer::ApplyCutOffShader( float CU_PARAM_UNUSED( p_cutOff ) )
{
}
