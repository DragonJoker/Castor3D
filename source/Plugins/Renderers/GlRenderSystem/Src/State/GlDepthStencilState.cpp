#include "State/GlDepthStencilState.hpp"

#include "Render/GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlDepthStencilState::GlDepthStencilState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: DepthStencilState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
	}

	GlDepthStencilState::~GlDepthStencilState()
	{
	}

	bool GlDepthStencilState::Apply()const
	{
		bool l_return = true;
		GetOpenGl().DepthMask( GetOpenGl().Get( GetDepthMask() ) );

		if ( GetDepthTest() )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_DEPTH_TEST );
			l_return &= GetOpenGl().DepthFunc( GetOpenGl().Get( GetDepthFunc() ) );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_DEPTH_TEST );
		}

		if ( m_bStencilTest )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_STENCIL_TEST );
			l_return &= GetOpenGl().StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, GetStencilWriteMask() );
			l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFunc() ), GetStencilBackRef(), GetStencilReadMask() );
			l_return &= GetOpenGl().StencilFuncSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFunc() ), GetStencilFrontRef(), GetStencilReadMask() );
			l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_BACK, GetOpenGl().Get( GetStencilBackFailOp() ), GetOpenGl().Get( GetStencilBackDepthFailOp() ), GetOpenGl().Get( GetStencilBackPassOp() ) );
			l_return &= GetOpenGl().StencilOpSeparate( eGL_FACE_FRONT, GetOpenGl().Get( GetStencilFrontFailOp() ), GetOpenGl().Get( GetStencilFrontDepthFailOp() ), GetOpenGl().Get( GetStencilFrontPassOp() ) );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_STENCIL_TEST );
		}

		return l_return;
	}
}
