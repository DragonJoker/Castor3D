#include "Shader/TestShaderObject.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestShaderObject::TestShaderObject( TestShaderProgram * p_parent, ShaderType p_type )
		: ShaderObject( p_parent, p_type )
	{
	}

	TestShaderObject::~TestShaderObject()
	{
	}

	bool TestShaderObject::Create()
	{
		return true;
	}

	void TestShaderObject::Destroy()
	{
		Detach();
	}

	bool TestShaderObject::Compile()
	{
		return true;
	}

	void TestShaderObject::Detach()
	{
	}

	void TestShaderObject::AttachTo( ShaderProgram & p_program )
	{
		Detach();
	}

	String TestShaderObject::DoRetrieveCompilerLog()
	{
		return String{};
	}
}
