#include "UniformBufferBinding.hpp"

#include "UniformBuffer.hpp"
#include "Mesh/Buffer/GpuBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	UniformBufferBinding::UniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const  & p_program )
		: OwnedBy< UniformBuffer >{ p_ubo }
		, m_program{ p_program }
	{
	}

	UniformBufferBinding::~UniformBufferBinding()
	{
	}

	void UniformBufferBinding::Bind( uint32_t p_index )const
	{
		GetOwner()->GetStorage().SetBindingPoint( p_index );
		DoBind( p_index );
	}
}
