#include "GeometryBuffers.hpp"
#include "IndexBuffer.hpp"
#include "MatrixBuffer.hpp"
#include "VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
		:	m_pVertexBuffer( std::move( p_pVertexBuffer ) )
		,	m_pIndexBuffer( std::move( p_pIndexBuffer ) )
		,	m_pMatrixBuffer( std::move( p_pMatrixBuffer ) )
	{
		m_bIndexBuffer = m_pIndexBuffer != nullptr;
		m_bMatrixBuffer = m_pMatrixBuffer != nullptr;
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}

	bool GeometryBuffers::Bind()
	{
		bool l_bReturn = m_pVertexBuffer && m_pIndexBuffer;

		if ( l_bReturn )
		{
			l_bReturn = m_pVertexBuffer->Bind();

			if ( l_bReturn )
			{
				if ( m_bMatrixBuffer )
				{
					l_bReturn = m_pMatrixBuffer->Bind( 1 );
				}

				if ( l_bReturn && m_bIndexBuffer )
				{
					l_bReturn = m_pIndexBuffer->Bind();

					if ( !l_bReturn )
					{
						m_pIndexBuffer->Unbind();
					}
				}
			}
		}

		return l_bReturn;
	}

	void GeometryBuffers::Unbind()
	{
		if ( m_pVertexBuffer && m_pIndexBuffer )
		{
			if ( m_bMatrixBuffer )
			{
				m_pMatrixBuffer->Unbind();
			}

			m_pVertexBuffer->Unbind();

			if ( m_bIndexBuffer )
			{
				m_pIndexBuffer->Unbind();
			}
		}
	}
}
