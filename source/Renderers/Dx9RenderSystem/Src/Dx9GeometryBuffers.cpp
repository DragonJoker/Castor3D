#include "Dx9GeometryBuffers.hpp"
#include "Dx9RenderSystem.hpp"

#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>

using namespace Castor3D;

namespace Dx9Render
{
	DxGeometryBuffers::DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer )
		:	GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), nullptr )
		,	m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxGeometryBuffers::~DxGeometryBuffers()
	{
	}

	bool DxGeometryBuffers::Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
	{
		bool l_bReturn = m_pVertexBuffer->Bind();

		if ( l_bReturn )
		{
			UINT l_uiSize = UINT( m_pVertexBuffer->GetSize() / m_pVertexBuffer->GetDeclaration().GetStride() );

			if ( m_pIndexBuffer )
			{
				l_bReturn = m_pIndexBuffer->Bind();

				if ( l_bReturn )
				{
					m_pRenderSystem->GetDevice()->DrawIndexedPrimitive( DirectX9::Get( p_ePrimitiveType ), 0, 0, l_uiSize, p_uiIndex, p_uiSize / 3 );
					m_pIndexBuffer->Unbind();
				}
			}
			else
			{
				m_pRenderSystem->GetDevice()->DrawPrimitive( DirectX9::Get( p_ePrimitiveType ), p_uiIndex, p_uiSize / 3 );
			}

			m_pVertexBuffer->Unbind();
		}

		return l_bReturn;
	}

	bool DxGeometryBuffers::DrawInstanced( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
	{
		bool l_bReturn = m_pVertexBuffer->Bind();

		if ( l_bReturn )
		{
			UINT l_uiSize = UINT( m_pVertexBuffer->GetSize() / m_pVertexBuffer->GetDeclaration().GetStride() );

			if ( m_pIndexBuffer )
			{
				l_bReturn = m_pIndexBuffer->Bind();

				if ( l_bReturn )
				{
					m_pRenderSystem->GetDevice()->DrawIndexedPrimitive( DirectX9::Get( p_ePrimitiveType ), 0, 0, l_uiSize, p_uiIndex, p_uiSize / 3 );
					m_pIndexBuffer->Unbind();
				}
			}
			else
			{
				m_pRenderSystem->GetDevice()->DrawPrimitive( DirectX9::Get( p_ePrimitiveType ), p_uiIndex, p_uiSize / 3 );
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
