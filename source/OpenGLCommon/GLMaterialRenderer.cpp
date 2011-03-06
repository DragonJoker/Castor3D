#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlMaterialRenderer.h"

using namespace Castor3D;

GlPassRenderer :: GlPassRenderer( SceneManager * p_pSceneManager)
	:	PassRenderer( p_pSceneManager)
{
	m_pfnApply = & GlPassRenderer::_apply;
	m_pfnApply2D = & GlPassRenderer::_apply2D;
	m_pfnRemove = & GlPassRenderer::_remove;
}

void GlPassRenderer :: Cleanup()
{
}

void GlPassRenderer :: Initialise()
{
	m_pfnApply = & GlPassRenderer::_apply;
	m_pfnApply2D = & GlPassRenderer::_apply2D;
	m_pfnRemove = & GlPassRenderer::_remove;
}

void GlPassRenderer :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	(this->*m_pfnApply)( p_displayMode);
}

void GlPassRenderer :: Render2D()
{
	(this->*m_pfnApply2D)();
}

void GlPassRenderer :: EndRender()
{
	(this->*m_pfnRemove)();
}

void GlPassRenderer :: _apply( ePRIMITIVE_TYPE p_displayMode)
{
	CheckGlError( glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, m_target->GetAmbient().const_ptr()), CU_T( "GlPassRenderer :: Apply - glMaterialfv( GL_AMBIENT)"));
	CheckGlError( glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_target->GetDiffuse().const_ptr()), CU_T( "GlPassRenderer :: Apply - glMaterialfv( GL_DIFFUSE)"));
	CheckGlError( glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_target->GetSpecular().const_ptr()), CU_T( "GlPassRenderer :: Apply - glMaterialfv( GL_SPECULAR)"));
	CheckGlError( glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, m_target->GetEmissive().const_ptr()), CU_T( "GlPassRenderer :: Apply - glMaterialfv( GL_EMISSION)"));
	CheckGlError( glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_target->GetShininess()), CU_T( "GlPassRenderer :: Apply - glMaterialf( GL_SHININESS)"));

	if ( ! m_target->IsDoubleFace())
	{
		CheckGlError( glCullFace( GL_FRONT), CU_T( "GlPassRenderer :: Apply - glCullFace( GL_FRONT)"));
		CheckGlError( glFrontFace( GL_CCW), CU_T( "GlPassRenderer :: Apply - glFrontFace( GL_CCW)"));
		CheckGlError( glEnable( GL_CULL_FACE), CU_T( "GlPassRenderer :: Apply - glEnable( GL_CULL_FACE)"));
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckGlError( glEnable( GL_BLEND), CU_T( "GlPassRenderer :: Apply - glEnable( GL_BLEND)"));
 		CheckGlError( glDisable( GL_DEPTH_TEST), CU_T( "GlPassRenderer :: Apply - glDisable( GL_DEPTH_TEST)"));
		CheckGlError( glBlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor())), CU_T( "GlPassRenderer :: Apply - glBlendFunc"));
	}
}

void GlPassRenderer :: _apply2D()
{
	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckGlError( glEnable( GL_BLEND), CU_T( "GlPassRenderer :: Apply - glEnable( GL_BLEND)"));
 		CheckGlError( glDisable( GL_DEPTH_TEST), CU_T( "GlPassRenderer :: Apply - glDisable( GL_DEPTH_TEST)"));
		CheckGlError( glBlendFunc( GlEnum::Get( m_target->GetSrcBlendFactor()), GlEnum::Get( m_target->GetDstBlendFactor())), CU_T( "GlPassRenderer :: Apply - glBlendFunc"));
	}

	CheckGlError( glColor4fv( m_target->GetAmbient().const_ptr()), CU_T( "GlPassRenderer :: _apply2D - glColor4fv"));
}

void GlPassRenderer :: _remove()
{
	if ( ! m_target->IsDoubleFace())
	{
		CheckGlError( glDisable( GL_CULL_FACE), CU_T( "GlPassRenderer :: Apply - glDisable( GL_CULL_FACE)"));
	}

	if (m_target->GetSrcBlendFactor() != Pass::eSrcBlendNone && m_target->GetDstBlendFactor() != Pass::eDstBlendNone)
	{
		CheckGlError( glDisable( GL_BLEND), CU_T( "GlPassRenderer :: Apply - glDisable( GL_BLEND)"));
 		CheckGlError( glEnable( GL_DEPTH_TEST), CU_T( "GlPassRenderer :: Apply - glEnable( GL_DEPTH_TEST)"));
	}
}