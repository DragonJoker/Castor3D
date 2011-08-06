#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9MaterialRenderer.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include "Dx9RenderSystem/CgDx9ShaderProgram.hpp"

using namespace Castor3D;

Dx9PassRenderer :: Dx9PassRenderer()
	:	PassRenderer()
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
	if (m_target->GetShader<ShaderProgramBase>())
	{
		if (m_target->GetShader<ShaderProgramBase>()->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_CG)
		{
			CgDx9ShaderProgramPtr l_pProgram = m_target->GetShader<CgDx9ShaderProgram>();

			if (l_pProgram)
			{
				l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
				l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
				l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
				l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
				l_pProgram->SetMaterialShininess(	m_target->GetShininess());
			}
		}
		else
		{
			Dx9ShaderProgramPtr l_pProgram = m_target->GetShader<Dx9ShaderProgram>();

			if (l_pProgram)
			{
				l_pProgram->SetMaterialAmbient(		m_target->GetAmbient());
				l_pProgram->SetMaterialDiffuse(		m_target->GetDiffuse());
				l_pProgram->SetMaterialSpecular(	m_target->GetSpecular());
				l_pProgram->SetMaterialEmissive(	m_target->GetEmissive());
				l_pProgram->SetMaterialShininess(	m_target->GetShininess());
			}
		}
	}

	memcpy( & m_d3dMaterial.Ambient, m_target->GetAmbient().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Diffuse, m_target->GetDiffuse().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Specular, m_target->GetSpecular().const_ptr(), 4 * sizeof( float));
	memcpy( & m_d3dMaterial.Emissive, m_target->GetEmissive().const_ptr(), 4 * sizeof( float));
	m_d3dMaterial.Power = m_target->GetShininess();

	CheckDxError( Dx9RenderSystem::GetDevice()->SetMaterial( & m_d3dMaterial), cuT( "Dx9PassRenderer :: _apply - SetMaterial"), false);

	if ( ! m_target->IsDoubleFace())
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}
	else
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3dEnum::Get( m_target->GetSrcBlendFactor())), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3dEnum::Get( m_target->GetDstBlendFactor())), cuT( "Dx9PassRenderer :: _apply - SetRenderState"), false);
	}
}

void Dx9PassRenderer :: _apply2D()
{
	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, FALSE), cuT( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE), cuT( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3dEnum::Get( m_target->GetSrcBlendFactor())), cuT( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3dEnum::Get( m_target->GetDstBlendFactor())), cuT( "Dx9PassRenderer :: _apply2D - SetRenderState"), false);
	}
}

void Dx9PassRenderer :: _remove()
{
	if ( ! m_target->IsDoubleFace())
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE), cuT( "Dx9PassRenderer :: _remove - SetMaterial"), false);
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSRC_BLEND_NONE && m_target->GetDstBlendFactor() != Pass::eDST_BLEND_NONE)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ZENABLE, TRUE), cuT( "Dx9PassRenderer :: _remove - SetRenderState"), false);
//		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE), cuT( "Dx9PassRenderer :: _remove - SetRenderState"), false);
	}
}
