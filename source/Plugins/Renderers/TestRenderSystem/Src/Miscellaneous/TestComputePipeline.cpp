#include "Miscellaneous/TestComputePipeline.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestComputePipeline::TestComputePipeline( TestRenderSystem & p_renderSystem, ShaderProgram & p_program )
		: ComputePipeline{ p_renderSystem, p_program }
	{
	}

	TestComputePipeline::~TestComputePipeline()
	{
	}

	void TestComputePipeline::Run( Point3ui const & p_count, Point3ui const & p_size, FlagCombination< MemoryBarrier > const & p_barriers )const
	{
	}
}
