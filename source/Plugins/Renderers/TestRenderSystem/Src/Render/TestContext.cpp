#include "Render/TestContext.hpp"

#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace TestRender
{
	TestContext::TestContext( TestRenderSystem & p_renderSystem )
		: Context{ p_renderSystem }
	{
	}

	TestContext::~TestContext()
	{
	}

	bool TestContext::doInitialise()
	{
		m_initialised = true;
		getRenderSystem()->getEngine()->getMaterialCache().initialise( getRenderSystem()->getEngine()->getMaterialsType() );
		return m_initialised;
	}

	void TestContext::doCleanup()
	{
	}

	void TestContext::doDestroy()
	{
	}

	void TestContext::doSetCurrent()
	{
	}

	void TestContext::doEndCurrent()
	{
	}

	void TestContext::doSwapBuffers()
	{
	}

	void TestContext::doMemoryBarrier( MemoryBarriers const & p_barriers )
	{
	}
}
