#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9MaterialRenderer.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

Dx9PassRenderer :: Dx9PassRenderer( SceneManager * p_pSceneManager)
	:	PassRenderer( p_pSceneManager)
{
	m_pfnApply = & Dx9PassRenderer::_apply;
	m_pfnApply2D = & Dx9PassRenderer::_apply2D;
	m_pfnRemove = & Dx9PassRenderer::_remove;
}

void Dx9PassRenderer :: Cleanup()
{
}

void Dx9PassRenderer :: Initialise()
{
	m_pfnApply = & Dx9PassRenderer::_apply;
	m_pfnApply2D = & Dx9PassRenderer::_apply2D;
	m_pfnRemove = & Dx9PassRenderer::_remove;
}

void Dx9PassRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	(this->*m_pfnApply)( p_displayMode);
}

void Dx9PassRenderer :: Render2D()
{
	(this->*m_pfnApply2D)();
}

void Dx9PassRenderer :: EndRender()
{
	(this->*m_pfnRemove)();
}

void Dx9PassRenderer :: _apply( ePRIMITIVE_TYPE p_displayMode)
{
	memcpy( & m_d3dMaterial.Ambient, m_target->GetAmbient().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Diffuse, m_target->GetDiffuse().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Specular, m_target->GetSpecular().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Emissive, m_target->GetEmissive().const_ptr(), 4 * sizeof( float));
	m_d3dMaterial.Power = m_target->GetShininess();

	CheckDxError( Dx9RenderSystem::GetDevice()->SetMaterial( & m_d3dMaterial), CU_T( "Dx9PassRenderer :: _apply - SetMaterial"), false);

	if ( ! m_target->IsDoubleFace())
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}
	else
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3dEnum::Get( m_target->GetSrcBlendFactor())), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3dEnum::Get( m_target->GetDstBlendFactor())), CU_T( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}
}

void Dx9PassRenderer :: _apply2D()
{
	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE), CU_T( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE), CU_T( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3dEnum::Get( m_target->GetSrcBlendFactor())), CU_T( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3dEnum::Get( m_target->GetDstBlendFactor())), CU_T( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
	}
}

void Dx9PassRenderer :: _remove()
{
	if ( ! m_target->IsDoubleFace())
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE), CU_T( "Dx9PassRenderer :: _remove - SetMaterial"), false);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE), CU_T( "Dx9PassRenderer :: _remove - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE), CU_T( "Dx9PassRenderer :: _remove - SetRenderState"), false);
	}
}