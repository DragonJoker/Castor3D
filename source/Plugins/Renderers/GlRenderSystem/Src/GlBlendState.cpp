#include "GlBlendState.hpp"

#include "GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlBlendState::GlBlendState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: BlendState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
		CreateCurrent();
	}

	GlBlendState::~GlBlendState()
	{
		DestroyCurrent();
	}

	bool GlBlendState::Apply()
	{
		bool l_return = true;
		bool l_bEnabled = false;

		if ( GetColourMaskR() != m_currentState->GetColourMaskR()
				|| GetColourMaskG() != m_currentState->GetColourMaskG()
				|| GetColourMaskB() != m_currentState->GetColourMaskB()
				|| GetColourMaskA() != m_currentState->GetColourMaskA() )
		{
			GetOpenGl().ColorMask( GetOpenGl().Get( GetColourMaskR() ), GetOpenGl().Get( GetColourMaskG() ), GetOpenGl().Get( GetColourMaskB() ), GetOpenGl().Get( GetColourMaskA() ) );
			m_currentState->SetColourMask( GetColourMaskR(), GetColourMaskG(), GetColourMaskB(), GetColourMaskA() );
		}

		if ( m_bEnableAlphaToCoverage )
		{
			if ( !m_currentState->IsAlphaToCoverageEnabled() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_ALPHA_TO_COVERAGE );
				m_currentState->EnableAlphaToCoverage( IsAlphaToCoverageEnabled() );
			}

			if ( m_uiSampleMask != m_currentState->GetSampleCoverageMask() )
			{
				l_return &= GetOpenGl().SampleCoverage( float( GetSampleCoverageMask() / double( 0xFFFFFFFF ) ), false );
				m_currentState->SetSampleCoverageMask( GetSampleCoverageMask() );
			}

			l_bEnabled = true;
		}
		else if ( m_currentState->IsAlphaToCoverageEnabled() )
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_ALPHA_TO_COVERAGE );
			m_currentState->EnableAlphaToCoverage( IsAlphaToCoverageEnabled() );
		}

		if ( m_bIndependantBlend )
		{
			for ( int i = 0; i < 8; i++ )
			{
				if ( m_rtStates[i].m_bEnableBlend )
				{
					l_bEnabled = true;

					if ( !m_currentState->IsBlendEnabled( i ) )
					{
						m_currentState->EnableBlend( true, i );

						l_return &= GetOpenGl().BlendFunc( i, GetOpenGl().Get( GetRgbSrcBlend( i ) ), GetOpenGl().Get( GetRgbDstBlend( i ) ), GetOpenGl().Get( GetAlphaSrcBlend( i ) ), GetOpenGl().Get( GetAlphaDstBlend( i ) ) );
						m_currentState->SetRgbSrcBlend( GetRgbSrcBlend( i ), i );
						m_currentState->SetRgbDstBlend( GetRgbDstBlend( i ), i );
						m_currentState->SetAlphaSrcBlend( GetAlphaSrcBlend( i ), i );
						m_currentState->SetAlphaDstBlend( GetAlphaDstBlend( i ), i );

						l_return &= GetOpenGl().BlendEquation( i, GetOpenGl().Get( GetRgbBlendOp( i ) ) );
						m_currentState->SetRgbBlendOp( GetRgbBlendOp( i ), i );
					}
					else
					{
						if ( GetRgbSrcBlend( i ) != m_currentState->GetRgbSrcBlend( i )
								|| GetRgbDstBlend( i ) != m_currentState->GetRgbDstBlend( i )
								|| GetAlphaSrcBlend( i ) != m_currentState->GetAlphaSrcBlend( i )
								|| GetAlphaDstBlend( i ) != m_currentState->GetAlphaDstBlend( i ) )
						{
							l_return &= GetOpenGl().BlendFunc( i, GetOpenGl().Get( GetRgbSrcBlend( i ) ), GetOpenGl().Get( GetRgbDstBlend( i ) ), GetOpenGl().Get( GetAlphaSrcBlend( i ) ), GetOpenGl().Get( GetAlphaDstBlend( i ) ) );
							m_currentState->SetRgbSrcBlend( GetRgbSrcBlend( i ), i );
							m_currentState->SetRgbDstBlend( GetRgbDstBlend( i ), i );
							m_currentState->SetAlphaSrcBlend( GetAlphaSrcBlend( i ), i );
							m_currentState->SetAlphaDstBlend( GetAlphaDstBlend( i ), i );
						}

						if ( GetRgbBlendOp( i ) != !m_currentState->GetRgbBlendOp( i ) )
						{
							l_return &= GetOpenGl().BlendEquation( i, GetOpenGl().Get( GetRgbBlendOp( i ) ) );
							m_currentState->SetRgbBlendOp( GetRgbBlendOp( i ), i );
						}
					}
				}
				else if ( m_currentState->IsBlendEnabled( i ) )
				{
					m_currentState->EnableBlend( false, i );
					l_return &= GetOpenGl().BlendFunc( i, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
					l_return &= GetOpenGl().BlendEquation( i, eGL_BLEND_OP_ADD );
				}
			}
		}
		else
		{
			if ( m_rtStates[0].m_bEnableBlend )
			{
				l_bEnabled = true;

				if ( !m_currentState->IsBlendEnabled() )
				{
					m_currentState->EnableBlend( true, 0 );

					l_return &= GetOpenGl().BlendFunc( GetOpenGl().Get( GetRgbSrcBlend() ), GetOpenGl().Get( GetRgbDstBlend() ), GetOpenGl().Get( GetAlphaSrcBlend() ), GetOpenGl().Get( GetAlphaDstBlend() ) );
					m_currentState->SetRgbSrcBlend( GetRgbSrcBlend() );
					m_currentState->SetRgbDstBlend( GetRgbDstBlend() );
					m_currentState->SetAlphaSrcBlend( GetAlphaSrcBlend() );
					m_currentState->SetAlphaDstBlend( GetAlphaDstBlend() );

					l_return &= GetOpenGl().BlendEquation( GetOpenGl().Get( GetRgbBlendOp() ) );
					m_currentState->SetRgbBlendOp( GetRgbBlendOp() );
				}
				else
				{
					if ( GetRgbSrcBlend() != m_currentState->GetRgbSrcBlend()
							|| GetRgbDstBlend() != m_currentState->GetRgbDstBlend()
							|| GetAlphaSrcBlend() != m_currentState->GetAlphaSrcBlend()
							|| GetAlphaDstBlend() != m_currentState->GetAlphaDstBlend() )
					{
						l_return &= GetOpenGl().BlendFunc( GetOpenGl().Get( GetRgbSrcBlend() ), GetOpenGl().Get( GetRgbDstBlend() ), GetOpenGl().Get( GetAlphaSrcBlend() ), GetOpenGl().Get( GetAlphaDstBlend() ) );
						m_currentState->SetRgbSrcBlend( GetRgbSrcBlend() );
						m_currentState->SetRgbDstBlend( GetRgbDstBlend() );
						m_currentState->SetAlphaSrcBlend( GetAlphaSrcBlend() );
						m_currentState->SetAlphaDstBlend( GetAlphaDstBlend() );
					}

					if ( GetRgbBlendOp() != !m_currentState->GetRgbBlendOp() )
					{
						l_return &= GetOpenGl().BlendEquation( GetOpenGl().Get( GetRgbBlendOp() ) );
						m_currentState->SetRgbBlendOp( GetRgbBlendOp() );
					}
				}
			}
			else if ( m_currentState->IsBlendEnabled() )
			{
				m_currentState->EnableBlend( false );
				l_return &= GetOpenGl().BlendFunc( eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
				l_return &= GetOpenGl().BlendEquation( eGL_BLEND_OP_ADD );
			}
		}

		if ( l_bEnabled )
		{
			if ( GetBlendFactors() != m_currentState->GetBlendFactors() )
			{
				l_return &= GetOpenGl().BlendColor( GetBlendFactors() );
				m_currentState->SetBlendFactors( GetBlendFactors() );
			}

			l_return &= GetOpenGl().Enable( eGL_TWEAK_BLEND );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_BLEND );
		}

		m_changed = false;
		return l_return;
	}

	BlendStateSPtr GlBlendState::DoCreateCurrent()
	{
		return std::make_shared< GlBlendState >( m_renderSystem, GetOpenGl() );
	}
}
