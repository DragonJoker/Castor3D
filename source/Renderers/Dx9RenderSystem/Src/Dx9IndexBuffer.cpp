#include "Dx9IndexBuffer.hpp"
#include "Dx9RenderSystem.hpp"

#include <CpuBuffer.hpp>

using namespace Castor3D;
namespace Dx9Render
{
	DxIndexBuffer::DxIndexBuffer( HardwareBufferPtr p_pBuffer )
		:	DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >( p_pBuffer )
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
		DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoDestroy();
	}

	void DxIndexBuffer::Cleanup()
	{
		DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoCleanup();
	}

	bool DxIndexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
	{
		bool l_bReturn = false;

		if ( m_pBuffer )
		{
			m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize )
			{
				l_bReturn = dxCheckError( m_pDevice->CreateIndexBuffer( UINT( l_uiSize * sizeof( uint32_t ) ), DirectX9::GetBufferFlags( p_eType | p_eNature ), D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pBufferObject, NULL ), "IDirect3DDevice9::CreateIndexBuffer" );
			}
			else
			{
				l_bReturn = false;
			}

			if ( l_bReturn && l_uiSize )
			{
				DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoFill( m_pBuffer->data(), m_pBuffer->GetSize() );
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
			l_bReturn = dxCheckError( m_pDevice->SetIndices( m_pBufferObject ), "IDirect3DDevice9::SetIndices" );
		}

		return l_bReturn;
	}

	void DxIndexBuffer::Unbind()
	{
	}

	uint32_t * DxIndexBuffer::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		return DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	void DxIndexBuffer::Unlock()
	{
		DxBufferObject< uint32_t, IDirect3DIndexBuffer9 >::DoUnlock();
	}
}