#include "Shader/TestUniformBuffer.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestUniformBuffer::TestUniformBuffer( String const & p_name
		, ShaderProgram & p_program
		, ShaderTypeFlags const & p_flags
		, RenderSystem & p_renderSystem )
		: UniformBuffer( p_name, p_program, p_flags, p_renderSystem )
	{
	}

	TestUniformBuffer::~TestUniformBuffer()
	{
	}

	bool TestUniformBuffer::DoInitialise()
	{
		return true;
	}

	void TestUniformBuffer::DoCleanup()
	{
	}

	void TestUniformBuffer::DoBindTo( uint32_t p_index )
	{
	}

	void TestUniformBuffer::DoUpdate()
	{
	}
}
