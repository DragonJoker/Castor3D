#include "Render/GlViewport.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlViewport::GlViewport( GlRenderSystem & renderSystem, Viewport & p_viewport )
		: IViewportImpl{ renderSystem, p_viewport }
		, Holder{ renderSystem.getOpenGl() }
	{
	}

	GlViewport::~GlViewport()
	{
	}

	void GlViewport::apply()const
	{
		getOpenGl().Viewport( m_viewport.getPosition().x()
			, m_viewport.getPosition().y()
			, m_viewport.getWidth()
			, m_viewport.getHeight() );
	}
}
