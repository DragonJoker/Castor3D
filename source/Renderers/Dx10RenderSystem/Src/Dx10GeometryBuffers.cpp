#include "Dx10GeometryBuffers.hpp"
#include "Dx10RenderSystem.hpp"

#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <MatrixBuffer.hpp>

using namespace Castor3D;

namespace Dx10Render
{
	DxGeometryBuffers::DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
		:	GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ) )
		,	m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxGeometryBuffers::~DxGeometryBuffers()
	{
	}

	bool DxGeometryBuffers::Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
	{
		m_pRenderSystem->GetDevice()->IASetPrimitiveTopology( DirectX10::Get( p_ePrimitiveType ) );
		bool l_bReturn = m_pVertexBuffer->Bind();

		if ( l_bReturn )
		{
			if ( m_pIndexBuffer )
			{
				l_bReturn = m_pIndexBuffer->Bind();

				if ( l_bReturn )
				{
					m_pRenderSystem->GetDevice()->DrawIndexed( p_uiSize, p_uiIndex, 0 );
					m_pIndexBuffer->Unbind();
				}
			}
			else
			{
				m_pRenderSystem->GetDevice()->Draw( p_uiSize, p_uiIndex );
			}

			m_pVertexBuffer->Unbind();
		}

		return l_bReturn;
	}

	bool DxGeometryBuffers::DrawInstanced( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
	{
		m_pRenderSystem->GetDevice()->IASetPrimitiveTopology( DirectX10::Get( p_ePrimitiveType ) );
		bool l_bReturn = m_pVertexBuffer->Bind();

		if ( l_bReturn )
		{
			if ( m_pMatrixBuffer )
			{
				l_bReturn = m_pMatrixBuffer->Bind( p_uiCount );
			}

			if ( l_bReturn )
			{
				if ( m_pIndexBuffer )
				{
					l_bReturn = m_pIndexBuffer->Bind();

					if ( l_bReturn )
					{
						m_pRenderSystem->GetDevice()->DrawIndexedInstanced( p_uiSize, p_uiCount, p_uiIndex, 0, 0 );
						m_pIndexBuffer->Unbind();
					}
				}
				else
				{
					m_pRenderSystem->GetDevice()->DrawInstanced( p_uiSize, p_uiCount, p_uiIndex, 0 );
				}
			}

			m_pVertexBuffer->Unbind();
		}

		return l_bReturn;
	}

	bool DxGeometryBuffers::Initialise()
	{
		return true;
	}

	void DxGeometryBuffers::Cleanup()
	{
	}
}
