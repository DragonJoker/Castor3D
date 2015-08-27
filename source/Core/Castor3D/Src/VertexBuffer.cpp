#include "VertexBuffer.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	VertexBuffer::VertexBuffer( RenderSystem * p_pRenderSystem, BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements )
		:	CpuBuffer< uint8_t >( p_pRenderSystem )
		,	m_bufferDeclaration( p_pElements, p_uiNbElements )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = m_pRenderSystem->CreateVertexBuffer( m_bufferDeclaration, this );
		}

		return m_pBuffer != nullptr;
	}
}
