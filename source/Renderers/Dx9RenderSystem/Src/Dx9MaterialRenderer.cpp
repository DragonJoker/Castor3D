#include "Dx9MaterialRenderer.hpp"
#include "Dx9RenderSystem.hpp"
//#include "DirectX9.hpp"

using namespace Castor3D;
using namespace Dx9Render;

DxPassRenderer::DxPassRenderer( DxRenderSystem * p_pRenderSystem )
	:	PassRenderer( p_pRenderSystem )
{
}

void DxPassRenderer::Cleanup()
{
}

void DxPassRenderer::Initialise()
{
}

void DxPassRenderer::Render()
{
	PassRenderer::Render();
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_pRenderSystem );
	ShaderProgramBaseSPtr l_pProgram = m_target->GetShader<ShaderProgramBase>();
	memcpy( & m_d3dMaterial.Ambient,	m_target->GetAmbient().const_ptr(),		4 * sizeof( float )	);
	memcpy( & m_d3dMaterial.Diffuse,	m_target->GetDiffuse().const_ptr(),		4 * sizeof( float )	);
	memcpy( & m_d3dMaterial.Specular,	m_target->GetSpecular().const_ptr(),	4 * sizeof( float )	);
	memcpy( & m_d3dMaterial.Emissive,	m_target->GetEmissive().const_ptr(),	4 * sizeof( float )	);
	m_d3dMaterial.Power = m_target->GetShininess();
	l_pRenderSystem->GetDevice()->SetMaterial( &m_d3dMaterial );
// 	if( !m_target->IsDoubleFace() )
// 	{
// 		l_pRenderSystem->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
// 	}
// 	else
// 	{
// //		l_pRenderSystem->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
// 	}
}

void DxPassRenderer::Render2D()
{
	PassRenderer::Render2D();
}

void DxPassRenderer::EndRender()
{
}
