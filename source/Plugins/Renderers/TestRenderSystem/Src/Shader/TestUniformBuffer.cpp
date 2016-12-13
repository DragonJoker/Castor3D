#include "Shader/TestUniformBuffer.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestUniformBuffer::TestUniformBuffer( String const & p_name, RenderSystem & p_renderSystem, uint32_t p_index )
		: UniformBuffer( p_name, p_renderSystem, p_index )
	{
	}

	TestUniformBuffer::~TestUniformBuffer()
	{
	}

	bool TestUniformBuffer::DoInitialise( ShaderProgram & p_program )
	{
		return true;
	}

	void TestUniformBuffer::DoCleanup()
	{
	}

	void TestUniformBuffer::DoBindTo( ShaderProgram & p_program )
	{
	}

	void TestUniformBuffer::DoUpdate()
	{
	}
}
