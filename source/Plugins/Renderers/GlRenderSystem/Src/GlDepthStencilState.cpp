#include "GlDepthStencilState.hpp"

#include "GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	GlDepthStencilState::GlDepthStencilState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: DepthStencilState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
		CreateCurrent();
	}

	GlDepthStencilState::~GlDepthStencilState()
	{
		DestroyCurrent();
	}

	bool GlDepthStencilState::Apply()
	{
		bool l_return = true;

		if ( GetDepthMask() != m_currentState->GetDepthMask() )
		{
			GetOpenGl().DepthMask( GetOpenGl().Get( GetDepthMask() ) );
			m_currentState->SetDepthMask( GetDepthMask() );
		}

		if ( GetDepthTest() )
		{
			if ( GetDepthTest() != m_currentState->GetDepthTest() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_DEPTH_TEST );
				m_currentState->SetDepthTest( GetDepthTest() );
			}

			if ( GetDepthFunc() != m_currentState->GetDepthFunc() )
			{
				l_return &= GetOpenGl().DepthFunc( GetOpenGl().Get( GetDepthFunc() ) );
				m_currentState->SetDepthFunc( GetDepthFunc() );
			}
		}
		else
		{
			if ( GetDepthTest() != m_currentState->GetDepthTest() )
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_DEPTH_TEST );
				m_currentState->SetDepthTest( GetDepthTest() );
			}
		}

		if ( m_bStencilTest )
		{
			if ( GetStencilTest() != m_currentState->GetStencilTest() )
			{
				l_return &= GetOpenGl().Enable( eGL_TWEAK_STENCIL_TEST );
				m_currentState->SetStencilTest( GetStencilTest() );

				l_return &= GetOpenGl().StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, GetStencilWriteMask() );
				m_currentState->SetStencilWriteMask( GetStencilWriteMask() );

				l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFunc() ), GetStencilBackRef(), GetStencilReadMask() );
				m_currentState->SetStencilBackRef( GetStencilBackRef() );
				m_currentState->SetStencilBackFunc( GetStencilBackFunc() );

				l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFunc() ), GetStencilFrontRef(), GetStencilReadMask() );
				m_currentState->SetStencilFrontRef( GetStencilFrontRef() );
				m_currentState->SetStencilFrontFunc( GetStencilFrontFunc() );

				l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFailOp() ), GetOpenGl().Get( GetStencilBackDepthFailOp() ), GetOpenGl().Get( GetStencilBackPassOp() ) );
				m_currentState->SetStencilBackFailOp( GetStencilBackFailOp() );
				m_currentState->SetStencilBackDepthFailOp( GetStencilBackDepthFailOp() );
				m_currentState->SetStencilBackPassOp( GetStencilBackPassOp() );

				l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFailOp() ), GetOpenGl().Get( GetStencilFrontDepthFailOp() ), GetOpenGl().Get( GetStencilFrontPassOp() ) );
				m_currentState->SetStencilFrontFailOp( GetStencilFrontFailOp() );
				m_currentState->SetStencilFrontDepthFailOp( GetStencilFrontDepthFailOp() );
				m_currentState->SetStencilFrontPassOp( GetStencilFrontPassOp() );
			}
			else
			{
				if ( GetStencilWriteMask() != m_currentState->GetStencilWriteMask() )
				{
					l_return &= GetOpenGl().StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, GetStencilWriteMask() );
					m_currentState->SetStencilWriteMask( GetStencilWriteMask() );
				}

				if ( GetStencilReadMask() != m_currentState->GetStencilReadMask()
						|| GetStencilBackRef() != m_currentState->GetStencilBackRef()
						|| GetStencilBackFunc() != m_currentState->GetStencilBackFunc() )
				{
					l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFunc() ), GetStencilBackRef(), GetStencilReadMask() );
					m_currentState->SetStencilBackRef( GetStencilBackRef() );
					m_currentState->SetStencilBackFunc( GetStencilBackFunc() );
				}

				if ( GetStencilReadMask() != m_currentState->GetStencilReadMask()
						|| GetStencilFrontRef() != m_currentState->GetStencilFrontRef()
						|| GetStencilFrontFunc() != m_currentState->GetStencilFrontFunc() )
				{
					l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFunc() ), GetStencilFrontRef(), GetStencilReadMask() );
					m_currentState->SetStencilFrontRef( GetStencilFrontRef() );
					m_currentState->SetStencilFrontFunc( GetStencilFrontFunc() );
				}

				m_currentState->SetStencilReadMask( GetStencilReadMask() );

				if ( GetStencilBackFailOp() != m_currentState->GetStencilBackFailOp()
						|| GetStencilBackDepthFailOp() != m_currentState->GetStencilBackDepthFailOp()
						|| GetStencilBackPassOp() != m_currentState->GetStencilBackPassOp() )
				{
					l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFailOp() ), GetOpenGl().Get( GetStencilBackDepthFailOp() ), GetOpenGl().Get( GetStencilBackPassOp() ) );
					m_currentState->SetStencilBackFailOp( GetStencilBackFailOp() );
					m_currentState->SetStencilBackDepthFailOp( GetStencilBackDepthFailOp() );
					m_currentState->SetStencilBackPassOp( GetStencilBackPassOp() );
				}

				if ( GetStencilFrontFailOp() != m_currentState->GetStencilFrontFailOp()
						|| GetStencilFrontDepthFailOp() != m_currentState->GetStencilFrontDepthFailOp()
						|| GetStencilFrontPassOp() != m_currentState->GetStencilFrontPassOp() )
				{
					l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFailOp() ), GetOpenGl().Get( GetStencilFrontDepthFailOp() ), GetOpenGl().Get( GetStencilFrontPassOp() ) );
					m_currentState->SetStencilFrontFailOp( GetStencilFrontFailOp() );
					m_currentState->SetStencilFrontDepthFailOp( GetStencilFrontDepthFailOp() );
					m_currentState->SetStencilFrontPassOp( GetStencilFrontPassOp() );
				}
			}
		}
		else
		{
			if ( GetStencilTest() != m_currentState->GetStencilTest() )
			{
				l_return &= GetOpenGl().Disable( eGL_TWEAK_STENCIL_TEST );
				m_currentState->SetStencilTest( GetStencilTest() );
			}
		}

		m_changed = false;
		return l_return;
	}

	DepthStencilStateSPtr GlDepthStencilState::DoCreateCurrent()
	{
		return std::make_shared< GlDepthStencilState >( m_renderSystem, GetOpenGl() );
	}
}
