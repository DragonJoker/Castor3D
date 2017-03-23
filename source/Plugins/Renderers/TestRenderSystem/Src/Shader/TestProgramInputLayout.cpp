#include "Shader/TestProgramInputLayout.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestProgramInputLayout::TestProgramInputLayout( RenderSystem & p_renderSystem )
		: ProgramInputLayout( p_renderSystem )
	{
	}

	TestProgramInputLayout::~TestProgramInputLayout()
	{
	}

	bool TestProgramInputLayout::Initialise( ShaderProgram const & p_program )
	{
		return true;
	}

	void TestProgramInputLayout::Cleanup()
	{
	}
}
