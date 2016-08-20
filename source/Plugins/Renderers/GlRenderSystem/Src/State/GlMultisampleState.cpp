#include "State/GlMultisampleState.hpp"

#include "Render/GlRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlMultisampleState::GlMultisampleState( GlRenderSystem * p_renderSystem, OpenGl & p_gl )
		: MultisampleState{ *p_renderSystem->GetEngine() }
		, Holder{ p_gl }
		, m_renderSystem{ p_renderSystem }
	{
	}

	GlMultisampleState::~GlMultisampleState()
	{
	}

	bool GlMultisampleState::Apply()const
	{
		bool l_return = true;

		if ( GetMultisample() )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_MULTISAMPLE );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_MULTISAMPLE );
		}

		if ( m_bEnableAlphaToCoverage )
		{
			l_return &= GetOpenGl().Enable( eGL_TWEAK_ALPHA_TO_COVERAGE );
		}
		else
		{
			l_return &= GetOpenGl().Disable( eGL_TWEAK_ALPHA_TO_COVERAGE );
		}

		return l_return;
	}
}
