#include "UniformBufferBinding.hpp"

#include "UniformBuffer.hpp"

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
		if ( m_size )
		{
			GetOwner()->GetStorage().SetBindingPoint( p_index );
			DoBind( p_index );
		}
	}
}
