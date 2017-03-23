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

	void TestTransformFeedback::DoBind()const
	{
	}

	void TestTransformFeedback::DoUnbind()const
	{
	}

	void TestTransformFeedback::DoBegin()const
	{
	}

	void TestTransformFeedback::DoEnd()const
	{
	}
}
