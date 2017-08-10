#include "Shader/TestProgramInputLayout.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace castor;
using namespace castor3d;

namespace TestRender
{
	TestProgramInputLayout::TestProgramInputLayout( RenderSystem & p_renderSystem )
		: ProgramInputLayout( p_renderSystem )
	{
	}

	TestProgramInputLayout::~TestProgramInputLayout()
	{
	}

	bool TestProgramInputLayout::initialise( ShaderProgram const & p_program )
	{
		return true;
	}

	void TestProgramInputLayout::cleanup()
	{
	}
}
