#include "Shader/TestUniformBufferBinding.hpp"

#include <Shader/UniformBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestUniformBufferBinding::TestUniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
	{
		auto size = 0u;

		for ( auto & variable : p_ubo )
		{
			auto stride = variable->size();
			m_variables.push_back( { variable->GetName(), size, 0u } );
			size += stride;
		}

		m_size = size;

	}

	TestUniformBufferBinding::~TestUniformBufferBinding()
	{
	}

	void TestUniformBufferBinding::DoBind( uint32_t p_index )const
	{
	}
}
