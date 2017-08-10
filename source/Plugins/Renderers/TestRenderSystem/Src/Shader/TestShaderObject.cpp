#include "Shader/TestShaderObject.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestShaderObject::TestShaderObject( TestShaderProgram * p_parent, ShaderType p_type )
		: ShaderObject( p_parent, p_type )
	{
	}

	TestShaderObject::~TestShaderObject()
	{
	}

	bool TestShaderObject::create()
	{
		return true;
	}

	void TestShaderObject::destroy()
	{
		detach();
	}

	bool TestShaderObject::compile()
	{
		return true;
	}

	void TestShaderObject::detach()
	{
	}

	void TestShaderObject::attachTo( ShaderProgram & p_program )
	{
		detach();
	}

	String TestShaderObject::doRetrieveCompilerLog()
	{
		return String{};
	}
}
