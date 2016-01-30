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

	bool VertexBuffer::Create()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetEngine()->GetRenderSystem()->CreateVertexBuffer( m_bufferDeclaration, this );
		}

		bool l_return = m_pBuffer != nullptr;

		if ( l_return )
		{
			l_return = GetGpuBuffer()->Create();
		}

		return l_return;
	}
}
