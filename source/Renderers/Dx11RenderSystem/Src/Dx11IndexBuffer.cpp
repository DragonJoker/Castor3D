#include "Dx11IndexBuffer.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxIndexBuffer::DxIndexBuffer( HardwareBufferPtr p_pBuffer )
		: DxBufferObject< uint32_t, ID3D11Buffer >( p_pBuffer )
	{
	}

	DxIndexBuffer::~DxIndexBuffer()
	{
	}

	bool DxIndexBuffer::Create()
	{
		return true;
	}

	void DxIndexBuffer::Destroy()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoDestroy();
	}

	void DxIndexBuffer::Cleanup()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoCleanup();
	}

	bool DxIndexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_bReturn = false;

		if ( m_pBuffer )
		{
			ID3D11Device * l_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize > 0 )
			{
				HRESULT l_hr;
				D3D11_BUFFER_DESC l_desc = { 0 };
				l_desc.ByteWidth = l_uiSize * UINT( sizeof( uint32_t ) );
				l_desc.Usage = DirectX11::Get( p_eType );
				l_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				l_desc.CPUAccessFlags = DirectX11::GetCpuAccessFlags( p_eType | p_eNature );
				l_desc.MiscFlags = 0;
				l_desc.StructureByteStride = 0;//sizeof( uint32_t );

				if ( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
				{
					D3D11_SUBRESOURCE_DATA l_data = { 0 };
					l_data.pSysMem = &m_pBuffer->data()[0];
					l_hr = l_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
					dxDebugName( m_pBufferObject, IndexBuffer );
				}
				else
				{
					l_hr = l_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
					dxDebugName( m_pBufferObject, IndexBuffer );
				}

				l_bReturn = dxCheckError( l_hr, "ID3D11Device::CreateIndexBuffer" );
			}
			else
			{
				l_bReturn = false;
			}

			if ( m_pBufferObject )
			{
				if ( p_eType != eBUFFER_ACCESS_TYPE_STATIC )
				{
					UINT l_uiSize = UINT( m_pBuffer->GetSize() );

					if ( l_uiSize )
					{
						uint32_t * l_pBuffer = Lock( 0, l_uiSize, eLOCK_FLAG_WRITE_ONLY );

						if ( l_pBuffer )
						{
							memcpy( l_pBuffer, &m_pBuffer->data()[0], l_uiSize * sizeof( uint32_t ) );
							Unlock();
						}
					}
				}

				m_pBuffer->Assign();
				l_bReturn = true;
			}
		}

		return l_bReturn;
	}

	bool DxIndexBuffer::Bind()
	{
		bool l_bReturn = true;

		if ( m_pBuffer && m_pBuffer->IsAssigned() )
		{
			ID3D11DeviceContext * l_pDeviceContext;
			reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetRenderSystem() )->GetDevice()->GetImmediateContext( &l_pDeviceContext );
			l_pDeviceContext->IASetIndexBuffer( m_pBufferObject, DXGI_FORMAT_R32_UINT, 0 );
			l_pDeviceContext->Release();
		}

		return l_bReturn;
	}

	void DxIndexBuffer::Unbind()
	{
	}

	uint32_t * DxIndexBuffer::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		return DxBufferObject< uint32_t, ID3D11Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	void DxIndexBuffer::Unlock()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoUnlock();
	}
}
