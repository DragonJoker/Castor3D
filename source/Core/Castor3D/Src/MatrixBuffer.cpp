#include "MatrixBuffer.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	MatrixBuffer::MatrixBuffer( RenderSystem * p_pRenderSystem )
		:	CpuBuffer< real >( p_pRenderSystem )
	{
	}

	MatrixBuffer::~MatrixBuffer()
	{
	}

	bool MatrixBuffer::Bind( uint32_t p_uiCount )
	{
		bool l_return	= false;
		GpuBufferSPtr l_pBuffer = GetGpuBuffer();

		if ( l_pBuffer )
		{
			l_return = l_pBuffer->Bind( p_uiCount );
		}

		return l_return;
	}

	bool MatrixBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = m_renderSystem->CreateMatrixBuffer( this );
		}

		return m_pBuffer != nullptr;
	}
}
