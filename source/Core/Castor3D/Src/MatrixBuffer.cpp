#include "MatrixBuffer.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	MatrixBuffer::MatrixBuffer( Engine & p_engine )
		: CpuBuffer< real >( p_engine )
	{
	}

	MatrixBuffer::~MatrixBuffer()
	{
	}

	bool MatrixBuffer::Bind( bool p_instantiated )
	{
		bool l_return	= false;
		GpuBufferSPtr l_pBuffer = GetGpuBuffer();

		if ( l_pBuffer )
		{
			l_return = l_pBuffer->Bind( p_instantiated );
		}

		return l_return;
	}

	bool MatrixBuffer::Create()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetEngine()->GetRenderSystem()->CreateMatrixBuffer( this );
		}

		bool l_return = m_pBuffer != nullptr;

		if ( l_return )
		{
			l_return = GetGpuBuffer()->Create();
		}

		return l_return;
	}
}
