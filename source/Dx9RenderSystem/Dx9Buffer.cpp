#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9Buffer.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

size_t Dx9BufferObjectBase::BuffersCount = 0;

//******************************************************************************

Dx9IndexBuffer :: Dx9IndexBuffer()
	:	Dx9BufferObjectBase()
	,	m_uiIndex( BuffersCount)
	,	m_uiFilled( IndexBuffer::m_filled)
	,	m_pBuffer( IndexBuffer::m_buffer)
	,	m_pBufferObject( NULL)
{
}

Dx9IndexBuffer :: ~Dx9IndexBuffer()
{
	Cleanup();
}

void Dx9IndexBuffer :: Cleanup()
{
	Buffer3D<unsigned int>::Cleanup();
	_cleanupBufferObject();
}

void Dx9IndexBuffer :: Initialise( eBUFFER_MODE p_eMode)
{
	m_pDevice = Dx9RenderSystem::GetDevice();

	m_bAssigned = m_uiFilled > 0 && CheckDxError( m_pDevice->CreateIndexBuffer( m_uiFilled * sizeof( unsigned int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, & m_pBufferObject, NULL), "Dx9IndexBuffer :: Initialise - CreateIndexBuffer", false);

	if (m_bAssigned)
	{
		void * l_pBuffer = Lock( 0, m_uiFilled * sizeof( unsigned int), eLockWriteOnly);
		memcpy( l_pBuffer, m_pBuffer, m_uiFilled * sizeof( unsigned int));
		Unlock();
		BufferManager::AssignBuffer<unsigned int>( m_uiIndex, this);
	}
}

void Dx9IndexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		CheckDxError( m_pDevice->SetIndices( m_pBufferObject), CU_T( "Dx9IndicesBuffer :: Activate - SetIndices"), false);
	}
}

void Dx9IndexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
//		CheckDxError( m_pDevice->SetIndices( NULL), CU_T( "Dx9IndicesBuffer :: Deactivate - SetIndices"), false);
	}
}

void * Dx9IndexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pData = NULL;
	CheckDxError( m_pBufferObject->Lock( p_uiOffset, p_uiSize, & l_pData, D3dEnum::GetLockFlags( p_uiFlags)), "Dx9IndexBuffer :: Lock", false);
	return l_pData;
}

void Dx9IndexBuffer :: Unlock()
{
	CheckDxError( m_pBufferObject->Unlock(), "Dx9IndexBuffer :: Unlock", false);
}

void Dx9IndexBuffer :: _cleanupBufferObject()
{
	if (m_bAssigned)
	{
		BufferManager::UnassignBuffer<unsigned int>( m_uiIndex, this);
	}

	if (m_pBufferObject != NULL)
	{
		m_pBufferObject->Release();
		m_pBufferObject = NULL;
	}

	m_bAssigned = false;
}

//******************************************************************************

Dx9VertexBuffer :: Dx9VertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	VertexBuffer( p_pElements, p_uiNbElements)
	,	Dx9VertexBufferObject<real, IDirect3DVertexBuffer9>( VertexBuffer::m_filled, VertexBuffer::m_buffer, VertexBuffer::m_bufferDeclaration)
{
}

Dx9VertexBuffer :: ~Dx9VertexBuffer()
{
	Cleanup();
}

void Dx9VertexBuffer :: Cleanup()
{
	Buffer3D<real>::Cleanup();
	_cleanupBufferObject();
}

void Dx9VertexBuffer :: Initialise( eBUFFER_MODE p_eMode)
{
	m_pDevice = Dx9RenderSystem::GetDevice();
	std::vector<D3DVERTEXELEMENT9> l_arrayDxElements;

	for (BufferDeclaration::const_iterator l_it = m_declaration.Begin() ; l_it != m_declaration.End() ; ++l_it)
	{
		D3DVERTEXELEMENT9 l_dx9CurrentElement;
		l_dx9CurrentElement.Stream = ( * l_it).m_uiStream;
		l_dx9CurrentElement.Offset = ( * l_it).m_uiOffset;
		l_dx9CurrentElement.Method = D3DDECLMETHOD_DEFAULT;

		switch (( * l_it).m_eUsage)
		{
		case eUsagePosition:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_POSITION;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eUsageNormal:		l_dx9CurrentElement.Usage = D3DDECLUSAGE_NORMAL;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eUsageTangent:		l_dx9CurrentElement.Usage = D3DDECLUSAGE_TANGENT;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eUsageDiffuse:		l_dx9CurrentElement.Usage = D3DDECLUSAGE_COLOR;		l_dx9CurrentElement.UsageIndex = 0;	break;
		case eUsageTexCoords0:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eUsageTexCoords1:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 1;	break;
		case eUsageTexCoords2:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 2;	break;
		case eUsageTexCoords3:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 3;	break;
		}

		switch (( * l_it).m_eDataType)
		{
		case eType1Float:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT1;	break;
		case eType2Floats:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT2;	break;
		case eType3Floats:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT3;	break;
		case eType4Floats:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT4;	break;
		case eTypeColour:	l_dx9CurrentElement.Type = D3DDECLTYPE_D3DCOLOR;break;
		}

		l_arrayDxElements.push_back( l_dx9CurrentElement);
	}

	// Ajout de l'élément marquant la fin du tableau
	D3DVERTEXELEMENT9 l_dx9EndElement = D3DDECL_END();
	l_arrayDxElements.push_back( l_dx9EndElement);
	m_bAssigned = false;

	// Création de la déclaration
	if (CheckDxError( m_pDevice->CreateVertexDeclaration( & l_arrayDxElements[0], & m_pDx9Declaration), "Dx9VertexBuffer :: Dx9VertexBuffer - CreateVertexDeclaration", false))
	{
		m_bAssigned = CheckDxError( m_pDevice->CreateVertexBuffer( m_uiFilled * sizeof( real), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, & m_pBufferObject, NULL), "Dx9VertexBuffer :: Initialise - CreateVertexBuffer", false);
	}

	if (m_bAssigned)
	{
		void * l_pBuffer = Lock( 0, m_uiFilled * sizeof( real), eLockWriteOnly);
		memcpy( l_pBuffer, m_pBuffer, m_uiFilled * sizeof( real));
		Unlock();
		BufferManager::AssignBuffer<real>( m_uiIndex, this);
	}
}

void Dx9VertexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		if (CheckDxError( m_pDevice->SetStreamSource( 0, m_pBufferObject, 0, Vertex::Size * sizeof( real)), CU_T( "Dx9VertexBuffer :: Activate - SetStreamSource"), false))
		{
			CheckDxError( m_pDevice->SetVertexDeclaration( m_pDx9Declaration), "Dx9VertexBuffer :: Activate - SetVertexDeclaration", false);
		}
	}
}

void Dx9VertexBuffer :: Deactivate()
{
	if (m_bAssigned)
	{
//		CheckDxError( m_pDevice->SetStreamSource( m_uiIndex, 0, 0, sizeof( real)), CU_T( "Dx9VertexBuffer :: Deactivate - SetStreamSource"), false);
	}
}

void * Dx9VertexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	void * l_pData = NULL;
	CheckDxError( m_pBufferObject->Lock( p_uiOffset, p_uiSize, & l_pData, D3dEnum::GetLockFlags( p_uiFlags)), "Dx9IndexBuffer :: Lock", false);
	return l_pData;
}

void Dx9VertexBuffer :: Unlock()
{
	CheckDxError( m_pBufferObject->Unlock(), "Dx9IndexBuffer :: Unlock", false);
}

void Dx9VertexBuffer :: _cleanupBufferObject()
{
	if (this->m_bAssigned)
	{
		BufferManager::UnassignBuffer<real>( m_uiIndex, this);
	}

	Dx9VertexBufferObject<real, IDirect3DVertexBuffer9>::CleanupBufferObject();
	this->m_bAssigned = false;
}

//******************************************************************************
