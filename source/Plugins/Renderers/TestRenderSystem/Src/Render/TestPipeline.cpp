#include "Render/TestPipeline.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestPipeline::TestPipeline( Context & p_context )
		: Pipeline( p_context )
	{
	}

	TestPipeline::~TestPipeline()
	{
	}

	void TestPipeline::ApplyViewport( int p_windowWidth, int p_windowHeight )
	{
	}
}
