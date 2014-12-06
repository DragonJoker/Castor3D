#include "GlBlendState.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlBlendState::GlBlendState( OpenGl & p_gl )
		:	BlendState()
		,	m_gl( p_gl )
	{
	}

	GlBlendState::~GlBlendState()
	{
	}

	bool GlBlendState::Apply()
	{
		bool l_bReturn = true;
		bool l_bEnabled = false;

		m_gl.ColorMask( m_gl.Get( m_eColourMask[0] ), m_gl.Get( m_eColourMask[1] ), m_gl.Get( m_eColourMask[2] ), m_gl.Get( m_eColourMask[3] ) );

		if ( m_bEnableAlphaToCoverage )
		{
			l_bReturn &= m_gl.Enable( eGL_TWEAK_ALPHA_TO_COVERAGE );
			l_bReturn &= m_gl.SampleCoverage( float( m_uiSampleMask / double( 0xFFFFFFFF ) ), false );
			l_bEnabled = true;
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_ALPHA_TO_COVERAGE );
		}

		if ( m_bIndependantBlend )
		{
			for ( int i = 0; i < 8; i++ )
			{
				if ( m_rtStates[i].m_bEnableBlend )
				{
					l_bEnabled = true;
					l_bReturn &= m_gl.BlendFunc( i, m_gl.Get( m_rtStates[i].m_eRgbSrcBlend ), m_gl.Get( m_rtStates[i].m_eRgbDstBlend ), m_gl.Get( m_rtStates[i].m_eAlphaSrcBlend ), m_gl.Get( m_rtStates[i].m_eAlphaDstBlend ) );
					l_bReturn &= m_gl.BlendEquation( i, m_gl.Get( m_rtStates[i].m_eRgbBlendOp ) );
				}
				else
				{
					l_bReturn &= m_gl.BlendFunc( i, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
					l_bReturn &= m_gl.BlendEquation( i, eGL_BLEND_OP_ADD );
				}
			}
		}
		else
		{
			if ( m_rtStates[0].m_bEnableBlend )
			{
				l_bEnabled = true;
				l_bReturn &= m_gl.BlendFunc( m_gl.Get( m_rtStates[0].m_eRgbSrcBlend ), m_gl.Get( m_rtStates[0].m_eRgbDstBlend ), m_gl.Get( m_rtStates[0].m_eAlphaSrcBlend ), m_gl.Get( m_rtStates[0].m_eAlphaDstBlend ) );
				l_bReturn &= m_gl.BlendEquation( m_gl.Get( m_rtStates[0].m_eRgbBlendOp ) );
			}
			else
			{
				l_bReturn &= m_gl.BlendFunc( eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
				l_bReturn &= m_gl.BlendEquation( eGL_BLEND_OP_ADD );
			}
		}

		if ( l_bEnabled )
		{
			l_bReturn &= m_gl.BlendColor( m_blendFactors );
			l_bReturn &= m_gl.Enable( eGL_TWEAK_BLEND );
		}
		else
		{
			l_bReturn &= m_gl.Disable( eGL_TWEAK_BLEND );
		}

		m_bChanged = false;
		return l_bReturn;
	}
}
