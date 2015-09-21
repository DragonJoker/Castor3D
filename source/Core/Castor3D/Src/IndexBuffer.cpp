#include "IndexBuffer.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	IndexBuffer::IndexBuffer( RenderSystem * p_pRenderSystem )
		:	CpuBuffer< uint32_t >( p_pRenderSystem )
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	bool IndexBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = m_renderSystem->CreateIndexBuffer( this );
		}

		return m_pBuffer != nullptr;
	}
}
