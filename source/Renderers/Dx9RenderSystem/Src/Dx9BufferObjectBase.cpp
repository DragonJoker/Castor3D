#include "Dx9BufferObjectBase.hpp"

using namespace Castor3D;

namespace Dx9Render
{
	uint32_t DxBufferObjectBase::BuffersCount = 0;

	DxBufferObjectBase::DxBufferObjectBase()
		: m_pDevice( NULL )
	{
		BuffersCount++;
	}

	DxBufferObjectBase::~DxBufferObjectBase()
	{
	}
}
