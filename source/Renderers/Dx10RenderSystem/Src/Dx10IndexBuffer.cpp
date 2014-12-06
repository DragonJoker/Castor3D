#include "Dx10IndexBuffer.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;

namespace Dx10Render
{
	DxIndexBuffer::DxIndexBuffer( HardwareBufferPtr p_pBuffer )
		:	DxBufferObject< uint32_t, ID3D10Buffer >( p_pBuffer )
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
		DxBufferObject< uint32_t, ID3D10Buffer >::DoDestroy();
	}

	void DxIndexBuffer::Cleanup()
	{
		DxBufferObject< uint32_t, ID3D10Buffer >::DoCleanup();
	}

	bool DxIndexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_bReturn = false;

		if ( m_pBuffer )
		{
			m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize > 0 )
			{
				HRESULT l_hr;
				D3D10_BUFFER_DESC l_desc	= { 0 };
				l_desc.ByteWidth			= l_uiSize * UINT( sizeof( uint32_t ) );
				l_desc.Usage				= DirectX10::Get( p_eType );
				l_desc.BindFlags			= D3D10_BIND_INDEX_BUFFER;
				l_desc.CPUAccessFlags		= DirectX10::GetCpuAccessFlags( p_eType | p_eNature );
				l_desc.MiscFlags			= 0;

				if ( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
				{
					D3D10_SUBRESOURCE_DATA l_data = { 0 };
					l_data.pSysMem = &m_pBuffer->data()[0];
					l_hr = m_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
					dxDebugName( m_pBufferObject, IndexBuffer );
				}
				else
				{
					l_hr = m_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
					dxDebugName( m_pBufferObject, IndexBuffer );
				}

				l_bReturn = dxCheckError( l_hr, "ID3D10Device::CreateIndexBuffer" );
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
			ID3D10Device * l_pDevice = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetRenderSystem() )->GetDevice();
			l_pDevice->IASetIndexBuffer( m_pBufferObject, DXGI_FORMAT_R32_UINT, 0 );
		}

		return l_bReturn;
	}

	void DxIndexBuffer::Unbind()
	{
	}

	uint32_t * DxIndexBuffer::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		return DxBufferObject< uint32_t, ID3D10Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	void DxIndexBuffer::Unlock()
	{
		DxBufferObject< uint32_t, ID3D10Buffer >::DoUnlock();
	}
}
