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
		bool l_bReturn	= false;
		GpuBufferSPtr l_pBuffer = GetGpuBuffer();

		if ( l_pBuffer )
		{
			l_bReturn = l_pBuffer->Bind( p_uiCount );
		}

		return l_bReturn;
	}

	bool MatrixBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = m_pRenderSystem->CreateMatrixBuffer( this );
		}

		return m_pBuffer != nullptr;
	}
}
