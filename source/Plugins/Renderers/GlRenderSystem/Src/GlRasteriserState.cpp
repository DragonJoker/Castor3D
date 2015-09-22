#include "GlRasteriserState.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	GlRasteriserState::GlRasteriserState( OpenGl & p_gl )
		: RasteriserState()
		, m_gl( p_gl )
	{
		CreateCurrent();
	}

	GlRasteriserState::~GlRasteriserState()
	{
		DestroyCurrent();
	}

	bool GlRasteriserState::Apply()
	{
		bool l_return = true;

		if ( GetFillMode() != m_currentState->GetFillMode() )
		{
			l_return &= m_gl.PolygonMode( eGL_FACE_FRONT_AND_BACK, m_gl.Get( GetFillMode() ) );
			m_currentState->SetFillMode( GetFillMode() );
		}

		if ( m_eCulledFaces != eFACE_NONE )
		{
			if ( m_currentState->GetCulledFaces() == eFACE_NONE )
			{
				l_return &= m_gl.Enable( eGL_TWEAK_CULL_FACE );
				l_return &= m_gl.CullFace( m_gl.Get( GetCulledFaces() ) );
				m_currentState->SetCulledFaces( GetCulledFaces() );

				if ( GetFrontCCW() )
				{
					l_return &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
				}
				else
				{
					l_return &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
				}

				m_currentState->SetFrontCCW( GetFrontCCW() );
			}
			else
			{
				if ( m_currentState->GetCulledFaces() != GetCulledFaces() )
				{
					l_return &= m_gl.CullFace( m_gl.Get( GetCulledFaces() ) );
					m_currentState->SetCulledFaces( GetCulledFaces() );
				}

				if ( m_currentState->GetFrontCCW() != GetFrontCCW() )
				{
					if ( m_bFrontCCW )
					{
						l_return &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
					}
					else
					{
						l_return &= m_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
					}

					m_currentState->SetFrontCCW( GetFrontCCW() );
				}
			}
		}
		else if ( m_currentState->GetCulledFaces() != GetCulledFaces() )
		{
			l_return &= m_gl.Disable( eGL_TWEAK_CULL_FACE );
			m_currentState->SetCulledFaces( GetCulledFaces() );
		}

		if ( m_currentState->GetAntialiasedLines() != GetAntialiasedLines() )
		{
			if ( GetAntialiasedLines() )
			{
				l_return &= m_gl.Enable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_NICEST );
			}
			else
			{
				l_return &= m_gl.Disable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= m_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_DONTCARE );
			}

			m_currentState->SetAntialiasedLines( GetAntialiasedLines() );
		}

		if ( m_currentState->GetScissor() != GetScissor() )
		{
			if ( GetScissor() )
			{
				l_return &= m_gl.Enable( eGL_TWEAK_SCISSOR_TEST );
			}
			else
			{
				l_return &= m_gl.Disable( eGL_TWEAK_SCISSOR_TEST );
			}

			m_currentState->SetScissor( GetScissor() );
		}

		if ( m_currentState->GetMultisample() != GetMultisample() )
		{
			if ( GetMultisample() )
			{
				l_return &= m_gl.Enable( eGL_TWEAK_MULTISAMPLE );
			}
			else
			{
				l_return &= m_gl.Disable( eGL_TWEAK_MULTISAMPLE );
			}

			m_currentState->SetMultisample( GetMultisample() );
		}

		if ( m_currentState->GetDepthClipping() != GetDepthClipping() )
		{
			if ( GetDepthClipping() )
			{
				l_return &= m_gl.Disable( eGL_TWEAK_DEPTH_CLAMP );
			}
			else
			{
				l_return &= m_gl.Enable( eGL_TWEAK_DEPTH_CLAMP );
			}

			m_currentState->SetDepthClipping( GetDepthClipping() );
		}

		if ( m_currentState->GetDepthBias() != GetDepthBias() )
		{
			l_return &= m_gl.PolygonOffset( GetDepthBias(), 4096.0 );
			m_currentState->SetDepthBias( GetDepthBias() );
		}

		m_bChanged = false;
		return l_return;
	}

	RasteriserStateSPtr GlRasteriserState::DoCreateCurrent()
	{
		return std::make_shared< GlRasteriserState >( m_gl );
	}
}
