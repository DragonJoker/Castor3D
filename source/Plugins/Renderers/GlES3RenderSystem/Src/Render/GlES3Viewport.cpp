#include "Render/GlES3Viewport.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3Viewport::GlES3Viewport( GlES3RenderSystem & p_renderSystem, Viewport & p_viewport )
		: IViewportImpl{ p_renderSystem, p_viewport }
		, Holder{ p_renderSystem.GetOpenGlES3() }
	{
	}

	GlES3Viewport::~GlES3Viewport()
	{
	}

	void GlES3Viewport::Apply()const
	{
		GetOpenGlES3().Viewport( 0, 0, m_viewport.GetWidth(), m_viewport.GetHeight() );
	}
}
