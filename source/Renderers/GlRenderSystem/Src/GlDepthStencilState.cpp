#include "GlDepthStencilState.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlDepthStencilState::GlDepthStencilState( OpenGl & p_gl )
		:	DepthStencilState()
		,	m_gl( p_gl )
	{
	}

	GlDepthStencilState::~GlDepthStencilState()
	{
	}

	bool GlDepthStencilState::Apply()
	{
		bool l_bReturn = true;
		m_gl.DepthMask( m_gl.Get( m_eDepthMask ) );

		if ( m_bDepthTest )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_DEPTH_TEST );
			l_bReturn &= m_gl.DepthFunc( m_gl.Get( m_eDepthFunc ) );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_DEPTH_TEST );
		}

		if ( m_bStencilTest )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_STENCIL_TEST );
			l_bReturn &= m_gl.StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, m_uiStencilWriteMask );
			l_bReturn &= m_gl.StencilFuncSeparate( eGL_FACE_BACK, m_gl.Get( m_stStencilBack.m_eFunc ), m_stStencilBack.m_ref, m_uiStencilReadMask );
			l_bReturn &= m_gl.StencilFuncSeparate( eGL_FACE_FRONT, m_gl.Get( m_stStencilFront.m_eFunc ), m_stStencilFront.m_ref, m_uiStencilReadMask );
			l_bReturn &= m_gl.StencilOpSeparate( eGL_FACE_BACK, m_gl.Get( m_stStencilFront.m_eFailOp ), m_gl.Get( m_stStencilFront.m_eDepthFailOp ), m_gl.Get( m_stStencilFront.m_ePassOp ) );
			l_bReturn &= m_gl.StencilOpSeparate( eGL_FACE_FRONT, m_gl.Get( m_stStencilFront.m_eFailOp ), m_gl.Get( m_stStencilFront.m_eDepthFailOp ), m_gl.Get( m_stStencilFront.m_ePassOp ) );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_STENCIL_TEST );
		}

		m_bChanged = false;
		return l_bReturn;
	}
}
