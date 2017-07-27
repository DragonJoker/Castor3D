#include "Render/TestContext.hpp"

#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestContext::TestContext( TestRenderSystem & p_renderSystem )
		: Context{ p_renderSystem }
	{
	}

	TestContext::~TestContext()
	{
	}

	bool TestContext::DoInitialise()
	{
		m_initialised = true;
		GetRenderSystem()->GetEngine()->GetMaterialCache().Initialise( GetRenderSystem()->GetEngine()->GetMaterialsType() );
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

	void TestContext::DoBarrier( MemoryBarriers const & p_barriers )
	{
	}
}
