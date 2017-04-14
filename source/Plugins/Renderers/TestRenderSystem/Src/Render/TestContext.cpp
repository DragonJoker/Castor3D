#include "Render/TestContext.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestContext::TestContext( TestRenderSystem & p_renderSystem )
		: Context{ p_renderSystem }
	{
		p_renderSystem.GetEngine()->GetMaterialCache().Initialise();
	}

	TestContext::~TestContext()
	{
	}

	bool TestContext::DoInitialise()
	{
		m_initialised = true;
		return m_initialised;
	}

	void TestContext::DoCleanup()
	{
	}

	void TestContext::DoDestroy()
	{
	}

	void TestContext::DoSetCurrent()
	{
	}

	void TestContext::DoEndCurrent()
	{
	}

	void TestContext::DoSwapBuffers()
	{
	}
}
