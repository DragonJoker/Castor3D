#include "Render/GlViewport.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlViewport::GlViewport( GlRenderSystem & p_renderSystem, Viewport & p_viewport )
		: IViewportImpl{ p_renderSystem, p_viewport }
		, Holder{ p_renderSystem.GetOpenGl() }
	{
	}

	GlViewport::~GlViewport()
	{
	}

	void GlViewport::Apply()const
	{
		GetOpenGl().Viewport( m_viewport.GetPosition().x()
			, m_viewport.GetPosition().y()
			, m_viewport.GetWidth()
			, m_viewport.GetHeight() );
	}
}
