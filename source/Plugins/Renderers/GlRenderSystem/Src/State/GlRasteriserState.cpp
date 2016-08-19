#include "State/GlRasteriserState.hpp"

#include "Render/GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlRasteriserState::GlRasteriserState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: RasteriserState( *p_renderSystem->GetEngine() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
	}

	GlRasteriserState::~GlRasteriserState()
	{
	}

	bool GlRasteriserState::Apply()const
	{
		bool l_return = true;

		l_return &= GetOpenGl().PolygonMode( eGL_FACE_FRONT_AND_BACK, GetOpenGl().Get( GetFillMode() ) );

		if ( m_eCulledFaces != eFACE_NONE )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_CULL_FACE );
			l_return &= GetOpenGl().CullFace( GetOpenGl().Get( GetCulledFaces() ) );

			if ( GetFrontCCW() )
			{
				l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
			}
			else
			{
				l_return &= GetOpenGl().FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
			}
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_CULL_FACE );
		}

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

		if ( GetScissor() )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_SCISSOR_TEST );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_SCISSOR_TEST );
		}

		if ( GetDepthClipping() )
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_DEPTH_CLAMP );
		}
		else
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_DEPTH_CLAMP );
		}

		l_return &= GetOpenGl().PolygonOffset( GetDepthBias(), 4096.0 );
		return l_return;
	}
}
