#include "Shader/TestUniformBufferBinding.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestUniformBufferBinding::TestUniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
	{
	}

	TestUniformBufferBinding::~TestUniformBufferBinding()
	{
	}

	void TestUniformBufferBinding::DoBind( uint32_t p_index )const
	{
	}
}
