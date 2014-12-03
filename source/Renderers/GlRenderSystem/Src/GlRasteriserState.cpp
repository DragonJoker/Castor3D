#include "GlRasteriserState.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlRasteriserState::GlRasteriserState( OpenGl & p_gl )
		:	RasteriserState()
		,	m_gl( p_gl )
	{
	}

	GlRasteriserState::~GlRasteriserState()
	{
	}

	bool GlRasteriserState::Apply()
	{
		bool l_bReturn = true;
		l_bReturn &= m_gl.PolygonMode( eGL_FACE_FRONT_AND_BACK, m_gl.Get( m_eFillMode ) );

		if ( m_eCulledFaces != eFACE_NONE )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_CULL_FACE );
			l_bReturn &= m_gl.CullFace( m_gl.Get( m_eCulledFaces ) );

			if ( m_bFrontCCW )
			{
				l_bReturn &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
			}
			else
			{
				l_bReturn &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
			}
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_CULL_FACE );
		}

		if ( m_bAntialiasedLines )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_LINE_SMOOTH );
			l_bReturn &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_NICEST );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_LINE_SMOOTH );
			l_bReturn &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_DONTCARE );
		}

		if ( m_bScissor )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_SCISSOR_TEST );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_SCISSOR_TEST );
		}

		if ( m_bMultisampled )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_MULTISAMPLE );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_MULTISAMPLE );
		}

		if ( m_bDepthClipping )
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_DEPTH_CLAMP );
		}
		else
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_DEPTH_CLAMP );
		}

		l_bReturn &= m_gl.PolygonOffset( m_fDepthBias, 4096.0 );
		m_bChanged = false;
		return l_bReturn;
	}
}
