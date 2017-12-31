#include "Shader/TestUniformBufferBinding.hpp"

#include <Shader/UniformBuffer.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestUniformBufferBinding::TestUniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
	{
		auto size = 0u;

		for ( auto & variable : p_ubo )
		{
			auto stride = variable->size();
			m_variables.push_back( { variable->getName(), size, 0u } );
			size += stride;
		}

		m_size = size;

	}

	TestUniformBufferBinding::~TestUniformBufferBinding()
	{
	}

	void TestUniformBufferBinding::doBind( uint32_t p_index )const
	{
	}
}
