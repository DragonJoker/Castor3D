#include "State/GlBlendState.hpp"

#include "Render/GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlBlendState::GlBlendState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: BlendState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
	}

	GlBlendState::~GlBlendState()
	{
	}

	bool GlBlendState::Apply()const
	{
		bool l_return{ true };
		bool l_enabled{ false };

		GetOpenGl().ColorMask( GetOpenGl().Get( GetColourMaskR() ), GetOpenGl().Get( GetColourMaskG() ), GetOpenGl().Get( GetColourMaskB() ), GetOpenGl().Get( GetColourMaskA() ) );

		if ( m_bIndependantBlend )
		{
			for ( int i = 0; i < 8; i++ )
			{
				if ( m_rtStates[i].m_bEnableBlend )
				{
					l_enabled = true;
					l_return &= GetOpenGl().BlendFunc( i, GetOpenGl().Get( GetRgbSrcBlend( i ) ), GetOpenGl().Get( GetRgbDstBlend( i ) ), GetOpenGl().Get( GetAlphaSrcBlend( i ) ), GetOpenGl().Get( GetAlphaDstBlend( i ) ) );
					l_return &= GetOpenGl().BlendEquation( i, GetOpenGl().Get( GetRgbBlendOp( i ) ) );
				}
				else
				{
					l_return &= GetOpenGl().BlendFunc( i, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
					l_return &= GetOpenGl().BlendEquation( i, eGL_BLEND_OP_ADD );
				}
			}
		}
		else
		{
			if ( m_rtStates[0].m_bEnableBlend )
			{
				l_enabled = true;
				l_return &= GetOpenGl().BlendFunc( GetOpenGl().Get( GetRgbSrcBlend() ), GetOpenGl().Get( GetRgbDstBlend() ), GetOpenGl().Get( GetAlphaSrcBlend() ), GetOpenGl().Get( GetAlphaDstBlend() ) );
				l_return &= GetOpenGl().BlendEquation( GetOpenGl().Get( GetRgbBlendOp() ) );
			}
			else
			{
				l_return &= GetOpenGl().BlendFunc( eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
				l_return &= GetOpenGl().BlendEquation( eGL_BLEND_OP_ADD );
			}
		}

		if ( l_enabled )
		{
			l_return &= GetOpenGl().BlendColor( GetBlendFactors() );
			l_return &= GetOpenGl().Enable( eGL_TWEAK_BLEND );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_BLEND );
		}

		return l_return;
	}
}
