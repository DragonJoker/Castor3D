#include "IndexBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	IndexBuffer::IndexBuffer( Engine & p_engine )
		: CpuBuffer< uint32_t >( p_engine )
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	bool IndexBuffer::Create()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetEngine()->GetRenderSystem()->CreateIndexBuffer( this );
		}

		bool l_return = m_pBuffer != nullptr;

		if ( l_return )
		{
			l_return = GetGpuBuffer()->Create();
		}

		return l_return;
	}
}
