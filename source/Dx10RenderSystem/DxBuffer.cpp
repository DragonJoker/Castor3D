#include "Dx10RenderSystem/PrecompiledHeader.hpp"

#include "Dx10RenderSystem/DxBuffer.hpp"
#include "Dx10RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Dx10Render;

//*************************************************************************************************

uint32_t DxBufferObjectBase::BuffersCount = 0;

//*************************************************************************************************

DxIndexBuffer :: DxIndexBuffer( HardwareBufferPtr p_pBuffer)
	:	DxBufferObject< uint32_t, ID3D10Buffer >( p_pBuffer)
{
}

DxIndexBuffer :: ~DxIndexBuffer()
{
}

bool DxIndexBuffer :: Create()
{
	return true;
}

void DxIndexBuffer :: Destroy()
{
	DxBufferObject< uint32_t, ID3D10Buffer >::DoDestroy();
}

void DxIndexBuffer :: Cleanup()
{
	DxBufferObject< uint32_t, ID3D10Buffer >::DoCleanup();
}

bool DxIndexBuffer :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = false;

	if( m_pBuffer )
	{
		m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
		UINT l_uiSize = UINT( m_pBuffer->GetSize() );

		if( l_uiSize > 0 )
		{
			HRESULT l_hr;
			D3D10_BUFFER_DESC l_desc	= { 0 };
			l_desc.ByteWidth			= l_uiSize * UINT( sizeof( uint32_t ) );
			l_desc.Usage				= DirectX10::Get( p_eType );
			l_desc.BindFlags			= D3D10_BIND_INDEX_BUFFER;
			l_desc.CPUAccessFlags		= DirectX10::GetCpuAccessFlags( p_eType | p_eNature );
			l_desc.MiscFlags			= 0;

			if( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
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

			l_bReturn = dxCheckError( l_hr, "ID3D10Device :: CreateIndexBuffer" );
		}
		else
		{
			l_bReturn = false;
		}

		if( m_pBufferObject )
		{
			if( p_eType != eBUFFER_ACCESS_TYPE_STATIC )
			{
				UINT l_uiSize = UINT( m_pBuffer->GetSize() );

				if( l_uiSize )
				{
					uint32_t * l_pBuffer = Lock( 0, l_uiSize, eLOCK_FLAG_WRITE_ONLY );

					if( l_pBuffer )
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

bool DxIndexBuffer :: Bind()
{
	bool l_bReturn = true;

	if( m_pBuffer && m_pBuffer->IsAssigned() )
	{
		ID3D10Device * l_pDevice = reinterpret_cast< DxRenderSystem* >( m_pBuffer->GetRenderSystem() )->GetDevice();
		l_pDevice->IASetIndexBuffer( m_pBufferObject, DXGI_FORMAT_R32_UINT, 0 );
	}

	return l_bReturn;
}

void DxIndexBuffer :: Unbind()
{
}

uint32_t * DxIndexBuffer :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags)
{
	return DxBufferObject< uint32_t, ID3D10Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
}

void DxIndexBuffer :: Unlock()
{
	DxBufferObject< uint32_t, ID3D10Buffer >::DoUnlock();
}

//*************************************************************************************************

DxVertexBuffer :: DxVertexBuffer( BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer)
	:	DxBufferObject< uint8_t, ID3D10Buffer >( p_pBuffer)
	,	m_pDxDeclaration	( NULL)
	,	m_declaration		( p_declaration)
{
}

DxVertexBuffer :: ~DxVertexBuffer()
{
}

bool DxVertexBuffer :: Create()
{
	return true;
}

void DxVertexBuffer :: Destroy()
{
	DxBufferObject< uint8_t, ID3D10Buffer >::DoDestroy();
}

void DxVertexBuffer :: Cleanup()
{
	SafeRelease( m_pDxDeclaration );
	DxBufferObject< uint8_t, ID3D10Buffer >::DoCleanup();
}

bool DxVertexBuffer :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
{
	bool l_bReturn = (m_pBuffer != NULL);
	m_pProgram = std::static_pointer_cast< DxShaderProgram >( p_pProgram );

	if( l_bReturn )
	{
		m_pDevice = static_cast< DxRenderSystem* >( m_pBuffer->GetRenderSystem() )->GetDevice();
		m_pBuffer->Unassign();
		// Création de la déclaration
		l_bReturn = DoCreateLayout();

		// Création du vertex buffer
		if( l_bReturn )
		{
			l_bReturn = DoCreateBuffer( p_eType, p_eNature );
		}
		
		// Remplissage du buffer si besoin est
		if( l_bReturn )
		{
			if( p_eType != eBUFFER_ACCESS_TYPE_STATIC )
			{
				UINT l_uiSize = UINT( m_pBuffer->GetSize() );

				if( l_uiSize )
				{
					uint8_t * l_pBuffer = Lock( 0, l_uiSize, eLOCK_FLAG_WRITE_ONLY );

					if( l_pBuffer )
					{
						memcpy( l_pBuffer, m_pBuffer->data(), l_uiSize * sizeof( uint8_t ) );
						Unlock();
					}
				}
			}

			m_pBuffer->Assign();
		}
	}

	return l_bReturn;
}

bool DxVertexBuffer :: Bind()
{
	bool l_bReturn = m_pBuffer && m_pBuffer->IsAssigned();

	if( l_bReturn )
	{
		ID3D10Device * l_pDevice = reinterpret_cast< DxRenderSystem* >( m_pBuffer->GetRenderSystem() )->GetDevice();
		UINT l_uiStrides[1] = { m_declaration.GetStride() };
		UINT l_uiOffsets[1] = { 0 };
		ID3D10Buffer * l_pBuffers[1] = { m_pBufferObject };
		l_pDevice->IASetVertexBuffers( 0, 1, l_pBuffers, l_uiStrides, l_uiOffsets );
		l_pDevice->IASetInputLayout( m_pDxDeclaration );
	}

	return l_bReturn;
}

void DxVertexBuffer :: Unbind()
{
}

uint8_t * DxVertexBuffer :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	return DxBufferObject< uint8_t, ID3D10Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
}

void DxVertexBuffer :: Unlock()
{
	DxBufferObject< uint8_t, ID3D10Buffer >::DoUnlock();
}

bool DxVertexBuffer :: DoCreateLayout()
{
	DxShaderProgramSPtr l_pProgram = m_pProgram.lock();
	bool l_bReturn = false;

	if( l_pProgram )
	{
		std::vector< D3D10_INPUT_ELEMENT_DESC >	l_arrayDxElements;
		UINT l_uiPosition = 0;
		UINT l_uiNormal = 0;
		UINT l_uiTangent = 0;
		UINT l_uiBinormal = 0;
		UINT l_uiColour = 0;
		UINT l_uiOffset = 0;

		for( BufferDeclaration::BufferElementDeclarationArrayConstIt l_it = m_declaration.Begin(); l_it != m_declaration.End(); ++l_it )
		{
			bool l_bAdd = true;
			D3D10_INPUT_ELEMENT_DESC l_d3dCurrentElement = { 0 };
			l_d3dCurrentElement.Format;
			l_d3dCurrentElement.InputSlot				= l_it->m_uiStream;
			l_d3dCurrentElement.AlignedByteOffset		= l_uiOffset;//l_it->m_uiOffset;
			l_d3dCurrentElement.InputSlotClass			= D3D10_INPUT_PER_VERTEX_DATA;
			l_d3dCurrentElement.InstanceDataStepRate	= 0;

			l_uiOffset = D3D10_APPEND_ALIGNED_ELEMENT;

			switch( l_it->m_eUsage )
			{
			case eELEMENT_USAGE_POSITION:		l_d3dCurrentElement.SemanticName	= "POSITION";		l_d3dCurrentElement.SemanticIndex	= l_uiPosition++;	break;
			case eELEMENT_USAGE_NORMAL:			l_d3dCurrentElement.SemanticName	= "NORMAL";			l_d3dCurrentElement.SemanticIndex	= l_uiNormal++;		break;
			case eELEMENT_USAGE_TANGENT:		l_d3dCurrentElement.SemanticName	= "TANGENT";		l_d3dCurrentElement.SemanticIndex	= l_uiTangent++;	break;
			case eELEMENT_USAGE_BITANGENT:		l_d3dCurrentElement.SemanticName	= "BINORMAL";		l_d3dCurrentElement.SemanticIndex	= l_uiBinormal++;	break;
			case eELEMENT_USAGE_DIFFUSE:		l_d3dCurrentElement.SemanticName	= "COLOR";			l_d3dCurrentElement.SemanticIndex	= l_uiColour++;		break;
			case eELEMENT_USAGE_TEXCOORDS0:		l_d3dCurrentElement.SemanticName	= "TEXCOORD";		l_d3dCurrentElement.SemanticIndex	= 0;				break;
			case eELEMENT_USAGE_TEXCOORDS1:		l_d3dCurrentElement.SemanticName	= "TEXCOORD";		l_d3dCurrentElement.SemanticIndex	= 1;				break;
			case eELEMENT_USAGE_TEXCOORDS2:		l_d3dCurrentElement.SemanticName	= "TEXCOORD";		l_d3dCurrentElement.SemanticIndex	= 2;				break;
			case eELEMENT_USAGE_TEXCOORDS3:		l_d3dCurrentElement.SemanticName	= "TEXCOORD";		l_d3dCurrentElement.SemanticIndex	= 3;				break;
			default: l_bAdd	= false;
// 			case eELEMENT_USAGE_BONE_IDS:		l_d3dCurrentElement.SemanticName	= "BLENDINDICES";	l_d3dCurrentElement.SemanticIndex	= 0;				break;
// 			case eELEMENT_USAGE_BONE_WEIGHTS:	l_d3dCurrentElement.SemanticName	= "BLENDWEIGHT";	l_d3dCurrentElement.SemanticIndex	= 0;				break;
			}

			switch( l_it->m_eDataType )
			{
			case eELEMENT_TYPE_1FLOAT:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32_FLOAT;				break;
			case eELEMENT_TYPE_2FLOATS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32_FLOAT;			break;
			case eELEMENT_TYPE_3FLOATS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;		break;
			case eELEMENT_TYPE_4FLOATS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	break;
			case eELEMENT_TYPE_COLOUR:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32_UINT;				break;
			case eELEMENT_TYPE_1INT:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32_UINT;				break;
			case eELEMENT_TYPE_2INTS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32_UINT;			break;
			case eELEMENT_TYPE_3INTS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32_UINT;		break;
			case eELEMENT_TYPE_4INTS:	l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32A32_UINT;		break;
			}

			if( l_bAdd )
			{
				l_arrayDxElements.push_back( l_d3dCurrentElement );
			}
		}

		ID3DBlob * l_pBlob = l_pProgram->GetCompiled( eSHADER_TYPE_VERTEX );

		if( l_pBlob )
		{
			HRESULT l_hr = m_pDevice->CreateInputLayout( &l_arrayDxElements[0], UINT( l_arrayDxElements.size() ), reinterpret_cast< DWORD const * >( l_pBlob->GetBufferPointer() ), l_pBlob->GetBufferSize(), &m_pDxDeclaration );
			dxDebugName( m_pDxDeclaration, VtxInputLayout );
			l_bReturn = dxCheckError( l_hr, "ID3D10Device :: CreateInputLayout" );
		}
	}

	return l_bReturn;
}

bool DxVertexBuffer :: DoCreateBuffer( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature )
{
	HRESULT l_hr;
	UINT l_uiSize				= UINT( m_pBuffer->GetSize() );
	D3D10_BUFFER_DESC l_desc	= { 0 };
	l_desc.ByteWidth			= l_uiSize * sizeof( uint8_t );
	l_desc.Usage				= DirectX10::Get( p_eType );
	l_desc.BindFlags			= D3D10_BIND_VERTEX_BUFFER;
	l_desc.CPUAccessFlags		= DirectX10::GetCpuAccessFlags( p_eType | p_eNature );
	l_desc.MiscFlags			= 0;

	if( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
	{
		D3D10_SUBRESOURCE_DATA l_data = { 0 };
		l_data.pSysMem = m_pBuffer->data();
		l_hr = m_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
		dxDebugName( m_pBufferObject, VertexBuffer );
	}
	else
	{
		l_hr = m_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
		dxDebugName( m_pBufferObject, VertexBuffer );
	}

	return dxCheckError( l_hr, "ID3D10Device :: CreateBuffer" );
}

//*************************************************************************************************

DxMatrixBuffer :: DxMatrixBuffer( HardwareBufferPtr p_pBuffer)
	:	DxBufferObject< real, ID3D10Buffer >	( p_pBuffer	)
	,	m_pDxDeclaration						( NULL		)
{
}

DxMatrixBuffer :: ~DxMatrixBuffer()
{
}

bool DxMatrixBuffer :: Create()
{
	return true;
}

void DxMatrixBuffer :: Destroy()
{
	DxBufferObject< real, ID3D10Buffer >::DoDestroy();
}

void DxMatrixBuffer :: Cleanup()
{
	SafeRelease( m_pDxDeclaration );
	DxBufferObject< real, ID3D10Buffer >::DoCleanup();
}

bool DxMatrixBuffer :: Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
{
	return true;
	bool l_bReturn = false;
	m_pProgram = std::static_pointer_cast< DxShaderProgram >( p_pProgram );

	if( m_pBuffer )
	{
		m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
		DxShaderProgramSPtr l_pProgram = m_pProgram.lock();
		bool l_bReturn = false;

		if( l_pProgram )
		{
			std::vector< D3D10_INPUT_ELEMENT_DESC >	l_arrayDxElements;
			UINT l_uiMtx = 0;

			for( int i = 0; i < 4; ++i )
			{
				D3D10_INPUT_ELEMENT_DESC l_d3dCurrentElement = { 0 };
				l_d3dCurrentElement.Format;
				l_d3dCurrentElement.InputSlot				= 0;
				l_d3dCurrentElement.AlignedByteOffset		= 0;
				l_d3dCurrentElement.InputSlotClass			= D3D10_INPUT_PER_INSTANCE_DATA;
				l_d3dCurrentElement.InstanceDataStepRate	= 1;
				l_d3dCurrentElement.SemanticName			= "MATRIX";
				l_d3dCurrentElement.SemanticIndex			= l_uiMtx++;
				l_d3dCurrentElement.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
				l_arrayDxElements.push_back( l_d3dCurrentElement );
			}

			ID3DBlob * l_pBlob = l_pProgram->GetCompiled( eSHADER_TYPE_VERTEX );

			if( l_pBlob )
			{
				HRESULT l_hr = m_pDevice->CreateInputLayout( &l_arrayDxElements[0], UINT( l_arrayDxElements.size() ), reinterpret_cast< DWORD const * >( l_pBlob->GetBufferPointer() ), l_pBlob->GetBufferSize(), &m_pDxDeclaration );
				dxDebugName( m_pDxDeclaration, MtxInputLayout );
				l_bReturn = dxCheckError( l_hr, "ID3D10Device :: CreateInputLayout" );
			}
		}

		UINT l_uiSize = UINT( m_pBuffer->GetSize() );

		if( l_uiSize > 0 )
		{
			HRESULT l_hr;
			D3D10_BUFFER_DESC l_desc	= { 0 };
			l_desc.ByteWidth			= l_uiSize * UINT( sizeof( uint32_t ) );
			l_desc.Usage				= DirectX10::Get( p_eType );
			l_desc.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
			l_desc.CPUAccessFlags		= DirectX10::GetCpuAccessFlags( p_eType | p_eNature );
			l_desc.MiscFlags			= 0;

			if( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
			{
				D3D10_SUBRESOURCE_DATA l_data = { 0 };
				l_data.pSysMem = &m_pBuffer->data()[0];
				l_hr = m_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
				dxDebugName( m_pBufferObject, MatrixBuffer );
			}
			else
			{
				l_hr = m_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
				dxDebugName( m_pBufferObject, MatrixBuffer );
			}

			l_bReturn = dxCheckError( l_hr, "ID3D10Device :: CreateIndexBuffer" );
		}
		else
		{
			l_bReturn = false;
		}

		if( m_pBufferObject )
		{
			if( p_eType != eBUFFER_ACCESS_TYPE_STATIC )
			{
				UINT l_uiSize = UINT( m_pBuffer->GetSize() );

				if( l_uiSize )
				{
					real * l_pBuffer = Lock( 0, l_uiSize, eLOCK_FLAG_WRITE_ONLY );

					if( l_pBuffer )
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

bool DxMatrixBuffer :: Bind( uint32_t p_uiCount )
{
	return true;
	bool l_bReturn = true;

	if( m_pBuffer && m_pBuffer->IsAssigned() )
	{
	}

	return l_bReturn;
}

void DxMatrixBuffer :: Unbind()
{
}

real * DxMatrixBuffer :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags)
{
	return NULL;
	return DxBufferObject< real, ID3D10Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
}

void DxMatrixBuffer :: Unlock()
{
	DxBufferObject< real, ID3D10Buffer >::DoUnlock();
}

//******************************************************************************

DxGeometryBuffers :: DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, VertexBufferSPtr p_pVertexBuffer, IndexBufferSPtr p_pIndexBuffer, MatrixBufferSPtr p_pMatrixBuffer )
	:	GeometryBuffers	( p_pVertexBuffer,	p_pIndexBuffer, p_pMatrixBuffer	)
	,	m_pRenderSystem	( p_pRenderSystem									)
{
}

DxGeometryBuffers :: ~DxGeometryBuffers()
{
}

bool DxGeometryBuffers :: Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
{
	m_pRenderSystem->GetDevice()->IASetPrimitiveTopology( DirectX10::Get( p_ePrimitiveType ) );

	bool l_bReturn = m_pVertexBuffer->Bind();

	if( l_bReturn )
	{
		if( m_pIndexBuffer )
		{
			l_bReturn = m_pIndexBuffer->Bind();

			if( l_bReturn )
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

bool DxGeometryBuffers :: DrawInstanced( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
{
	m_pRenderSystem->GetDevice()->IASetPrimitiveTopology( DirectX10::Get( p_ePrimitiveType ) );

	bool l_bReturn = m_pVertexBuffer->Bind();

	if( l_bReturn )
	{
		if( m_pMatrixBuffer )
		{
			l_bReturn = m_pMatrixBuffer->Bind( p_uiCount );
		}

		if( l_bReturn )
		{
			if( m_pIndexBuffer )
			{
				l_bReturn = m_pIndexBuffer->Bind();

				if( l_bReturn )
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

bool DxGeometryBuffers :: Initialise()
{
	return true;
}

void DxGeometryBuffers :: Cleanup()
{
}

//*************************************************************************************************