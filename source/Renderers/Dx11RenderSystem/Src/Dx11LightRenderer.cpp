#include "Dx11LightRenderer.hpp"
#include "Dx11RenderSystem.hpp"

#include <Light.hpp>
#include <SceneNode.hpp>
#include <TransformationMatrix.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

DxLightRenderer::DxLightRenderer( DxRenderSystem * p_pRenderSystem )
	: LightRenderer( p_pRenderSystem )
	, m_bChanged( true )
	, m_iIndex( -1 )
	, m_iD3dIndex( -1 )
	, m_iD3dPreviousIndex( -1 )
	, m_pCutOff( 0.0f )
	, m_pExponent( 0.0f )
{
}

void DxLightRenderer::Initialise()
{
}

void DxLightRenderer::Enable()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );
	m_iIndex = l_pRenderSystem->LockLight();
}

void DxLightRenderer::Disable()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );

	if ( m_iIndex >= 0 )
	{
		l_pRenderSystem->UnlockLight( m_iIndex );
	}

	m_iIndex = -1;
}

void DxLightRenderer::ApplyAmbient()
{
	if ( m_iIndex >= 0 )
	{
		m_pAmbient = m_target->GetAmbient();
	}
}

void DxLightRenderer::ApplyDiffuse()
{
	if ( m_iIndex >= 0 )
	{
		m_pDiffuse = m_target->GetDiffuse();
	}
}

void DxLightRenderer::ApplySpecular()
{
	if ( m_iIndex >= 0 )
	{
		m_pSpecular = m_target->GetSpecular();
	}
}

void DxLightRenderer::ApplyPosition()
{
	if ( m_iIndex >= 0 )
	{
#if CASTOR_USE_DOUBLE
		m_pPosition = Point4f( float( m_target->GetPositionType()[0] ), float( m_target->GetPositionType()[1] ), float( m_target->GetPositionType()[2] ), float( m_target->GetPositionType()[3] ) );
#else
		m_pPosition = m_target->GetPositionType();
#endif
	}
}

void DxLightRenderer::ApplyOrientation()
{
	if ( m_iIndex >= 0 )
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

void DxLightRenderer::ApplyConstantAtt( float p_constant )
{
	if ( m_iIndex >= 0 )
	{
		m_pAttenuation[0] = p_constant;
	}
}

void DxLightRenderer::ApplyLinearAtt( float p_linear )
{
	if ( m_iIndex >= 0 )
	{
		m_pAttenuation[1] = p_linear;
	}
}

void DxLightRenderer::ApplyQuadraticAtt( float p_quadratic )
{
	if ( m_iIndex >= 0 )
	{
		m_pAttenuation[2] = p_quadratic;
	}
}

void DxLightRenderer::ApplyExponent( float p_fExponent )
{
	if ( m_iIndex >= 0 )
	{
		m_pExponent = p_fExponent;
	}
}

void DxLightRenderer::ApplyCutOff( float p_fCutOff )
{
	if ( m_iIndex >= 0 )
	{
		m_pCutOff = p_fCutOff;
	}
}
