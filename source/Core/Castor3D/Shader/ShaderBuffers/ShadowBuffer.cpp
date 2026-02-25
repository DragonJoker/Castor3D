#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"

CU_ImplementSmartPtr( c3d, ShadowBuffer )

namespace c3d
{
	ShadowBuffer::ShadowBuffer( RenderDevice const & device )
		: UboT{ device, MemoryPropertyFlags::eHostVisible }
	{
	}
}
