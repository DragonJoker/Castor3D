#include "UniformBufferBinding.hpp"

#include "UniformBuffer.hpp"

using namespace castor;

namespace castor3d
{
	UniformBufferBinding::UniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const  & p_program )
		: OwnedBy< UniformBuffer >{ p_ubo }
		, m_program{ p_program }
	{
	}

	UniformBufferBinding::~UniformBufferBinding()
	{
	}

	void UniformBufferBinding::bind( uint32_t p_index )const
	{
		if ( m_size )
		{
			getOwner()->getStorage().setBindingPoint( p_index );
			doBind( p_index );
		}
	}
}
