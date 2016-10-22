#include "TestTransformFeedback.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;

namespace TestRender
{
	TestTransformFeedback::TestTransformFeedback( TestRenderSystem & p_renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: TransformFeedback{ p_renderSystem, p_computed, p_topology, p_program }
	{
	}

	TestTransformFeedback::~TestTransformFeedback()
	{
	}

	bool TestTransformFeedback::DoInitialise()
	{
		return true;
	}

	void TestTransformFeedback::DoCleanup()
	{
	}

	bool TestTransformFeedback::DoBind()const
	{
		return true;
	}

	void TestTransformFeedback::DoUnbind()const
	{
	}

	bool TestTransformFeedback::DoBegin()const
	{
		return true;
	}

	void TestTransformFeedback::DoEnd()const
	{
	}
}
