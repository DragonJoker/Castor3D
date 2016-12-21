#include "VertexBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	VertexBuffer::VertexBuffer( Engine & p_engine, BufferDeclaration const & p_declaration )
		: CpuBuffer< uint8_t >( p_engine )
		, m_bufferDeclaration( p_declaration )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::Initialise( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetEngine()->GetRenderSystem()->CreateUInt8Buffer( BufferType::eArray );
		}

		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			l_return = DoInitialise( p_type, p_nature );
		}

		return l_return;
	}

	void VertexBuffer::Cleanup()
	{
		DoCleanup();
	}
}
