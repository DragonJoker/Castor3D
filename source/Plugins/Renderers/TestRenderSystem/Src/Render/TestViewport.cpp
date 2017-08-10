#include "Render/TestViewport.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestViewport::TestViewport( TestRenderSystem & p_renderSystem, Viewport & p_viewport )
		: IViewportImpl{ p_renderSystem, p_viewport }
	{
	}

	TestViewport::~TestViewport()
	{
	}

	void TestViewport::apply()const
	{
	}
}
