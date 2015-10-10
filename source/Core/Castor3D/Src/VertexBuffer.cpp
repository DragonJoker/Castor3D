#include "VertexBuffer.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	VertexBuffer::VertexBuffer( Engine & p_engine, BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements )
		: CpuBuffer< uint8_t >( p_engine )
		, m_bufferDeclaration( p_pElements, p_uiNbElements )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetOwner()->GetRenderSystem()->CreateVertexBuffer( m_bufferDeclaration, this );
		}

		return m_pBuffer != nullptr;
	}
}
