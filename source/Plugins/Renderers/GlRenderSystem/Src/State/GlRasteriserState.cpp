#include "State/GlRasteriserState.hpp"

#include "Render/GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	GlRasteriserState::GlRasteriserState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: RasteriserState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
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
			l_return &= GetOpenGl().PolygonMode( eGL_FACE_FRONT_AND_BACK, GetOpenGl().Get( GetFillMode() ) );
			m_currentState->SetFillMode( GetFillMode() );
		}

		if ( m_eCulledFaces != eFACE_NONE )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_CULL_FACE );

			if ( m_currentState->GetCulledFaces() == eFACE_NONE )
			{
				l_return &= GetOpenGl().CullFace( GetOpenGl().Get( GetCulledFaces() ) );
				m_currentState->SetCulledFaces( GetCulledFaces() );

				if ( GetFrontCCW() )
				{
					l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
				}
				else
				{
					l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
				}

				m_currentState->SetFrontCCW( GetFrontCCW() );
			}
			else
			{
				if ( m_currentState->GetCulledFaces() != GetCulledFaces() )
				{
					l_return &= GetOpenGl().CullFace( GetOpenGl().Get( GetCulledFaces() ) );
					m_currentState->SetCulledFaces( GetCulledFaces() );
				}

				if ( m_currentState->GetFrontCCW() != GetFrontCCW() )
				{
					if ( m_bFrontCCW )
					{
						l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
					}
					else
					{
						l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
					}

					m_currentState->SetFrontCCW( GetFrontCCW() );
				}
			}
		}
		else if ( m_currentState->GetCulledFaces() != GetCulledFaces() )
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_CULL_FACE );
			m_currentState->SetCulledFaces( GetCulledFaces() );
		}

		if ( m_currentState->GetAntialiasedLines() != GetAntialiasedLines() )
		{
			if ( GetAntialiasedLines() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= GetOpenGl().Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_NICEST );
			}
			else
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= GetOpenGl().Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_DONTCARE );
			}

			m_currentState->SetAntialiasedLines( GetAntialiasedLines() );
		}

		if ( m_currentState->GetScissor() != GetScissor() )
		{
			if ( GetScissor() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_SCISSOR_TEST );
			}
			else
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_SCISSOR_TEST );
			}

			m_currentState->SetScissor( GetScissor() );
		}

		if ( m_currentState->GetMultisample() != GetMultisample() )
		{
			if ( GetMultisample() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_MULTISAMPLE );
			}
			else
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_MULTISAMPLE );
			}

			m_currentState->SetMultisample( GetMultisample() );
		}

		if ( m_currentState->GetDepthClipping() != GetDepthClipping() )
		{
			if ( GetDepthClipping() )
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_DEPTH_CLAMP );
			}
			else
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_DEPTH_CLAMP );
			}

			m_currentState->SetDepthClipping( GetDepthClipping() );
		}

		if ( m_currentState->GetDepthBias() != GetDepthBias() )
		{
			l_return &= GetOpenGl().PolygonOffset( GetDepthBias(), 4096.0 );
			m_currentState->SetDepthBias( GetDepthBias() );
		}

		m_changed = false;
		return l_return;
	}

	RasteriserStateSPtr GlRasteriserState::DoCreateCurrent()
	{
		return std::make_shared< GlRasteriserState >( m_renderSystem, GetOpenGl() );
	}
}
