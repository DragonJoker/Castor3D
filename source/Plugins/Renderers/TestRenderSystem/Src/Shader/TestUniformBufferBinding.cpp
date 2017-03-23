#include "Shader/TestUniformBufferBinding.hpp"

#include <Shader/UniformBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestUniformBufferBinding::TestUniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
	{
		auto l_size = 0u;

		for ( auto & l_variable : p_ubo )
		{
			auto l_stride = l_variable->size();
			m_variables.push_back( { l_size, 0u } );
			l_size += l_stride;
		}

		m_size = l_size;

	}

	TestUniformBufferBinding::~TestUniformBufferBinding()
	{
	}

	void TestUniformBufferBinding::DoBind( uint32_t p_index )const
	{
	}
}
