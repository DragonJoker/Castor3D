#include "TestTransformFeedback.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;

namespace TestRender
{
	TestTransformFeedback::TestTransformFeedback( TestRenderSystem & p_renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: TransformFeedback{ p_renderSystem, p_computed, p_topology, p_program }
	{
	}

	TestTransformFeedback::~TestTransformFeedback()
	{
	}

	bool TestTransformFeedback::doInitialise()
	{
		return true;
	}

	void TestTransformFeedback::doCleanup()
	{
	}

	void TestTransformFeedback::doBind()const
	{
	}

	void TestTransformFeedback::doUnbind()const
	{
	}

	void TestTransformFeedback::doBegin()const
	{
	}

	void TestTransformFeedback::doEnd()const
	{
	}
}
