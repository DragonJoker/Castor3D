#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9Buffer.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

size_t Dx9BufferObjectBase::BuffersCount = 0;

//*************************************************************************************************

Dx9IndexBuffer :: Dx9IndexBuffer()
	:	IndexBuffer	()
	,	Dx9BufferObject<unsigned int, IDirect3DIndexBuffer9>( IndexBuffer::m_filled, IndexBuffer::m_buffer)
{
}

Dx9IndexBuffer :: ~Dx9IndexBuffer()
{
}

void Dx9IndexBuffer :: Cleanup()
{
	Buffer<unsigned int>::clear();
	BufferCleanup( m_bAssigned, this);
	m_bAssigned = false;
}

void Dx9IndexBuffer :: Initialise( eBUFFER_MODE p_eMode)
{
	m_pDevice = Dx9RenderSystem::GetDevice();
	m_bAssigned = m_uiFilled > 0 && CheckDxError( m_pDevice->CreateIndexBuffer( m_uiFilled * sizeof( unsigned int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, & m_pBufferObject, NULL), "Dx9IndexBuffer :: Initialise - CreateIndexBuffer", false);

	if (m_bAssigned)
	{
		void * l_pBuffer = Lock( 0, m_uiFilled * sizeof( unsigned int), eLOCK_FLAG_WRITE_ONLY);
		memcpy( l_pBuffer, m_pBuffer, m_uiFilled * sizeof( unsigned int));
		Unlock();
		Root::GetSingleton()->GetBufferManager()->AssignBuffer<unsigned int>( m_uiIndex, this);
	}
}

void Dx9IndexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		CheckDxError( m_pDevice->SetIndices( m_pBufferObject), cuT( "Dx9IndicesBuffer :: Activate - SetIndices"), false);
	}
}

void Dx9IndexBuffer :: Deactivate()
{
}

void * Dx9IndexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	return BufferLock(p_uiOffset, p_uiSize, p_uiFlags);
}

void Dx9IndexBuffer :: Unlock()
{
	BufferUnlock();
}

//*************************************************************************************************

Dx9VertexBuffer :: Dx9VertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	VertexBuffer( p_pElements, p_uiNbElements)
	,	Dx9BufferObject<real, IDirect3DVertexBuffer9>( VertexBuffer::m_filled, VertexBuffer::m_buffer)
	,	m_declaration		( VertexBuffer::m_bufferDeclaration)
	,	m_pDx9Declaration	( NULL)
{
}

Dx9VertexBuffer :: ~Dx9VertexBuffer()
{
}

void Dx9VertexBuffer :: Cleanup()
{
	Buffer<real>::clear();
	BufferCleanup( m_bAssigned, this);
	m_bAssigned = false;
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
		case eELEMENT_USAGE_POSITION:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_POSITION;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_NORMAL:		l_dx9CurrentElement.Usage = D3DDECLUSAGE_NORMAL;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TANGENT:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TANGENT;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_DIFFUSE:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_COLOR;		l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TEXCOORDS0:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 0;	break;
		case eELEMENT_USAGE_TEXCOORDS1:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 1;	break;
		case eELEMENT_USAGE_TEXCOORDS2:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 2;	break;
		case eELEMENT_USAGE_TEXCOORDS3:	l_dx9CurrentElement.Usage = D3DDECLUSAGE_TEXCOORD;	l_dx9CurrentElement.UsageIndex = 3;	break;
		}

		switch (( * l_it).m_eDataType)
		{
		case eELEMENT_TYPE_1FLOAT:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT1;	break;
		case eELEMENT_TYPE_2FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT2;	break;
		case eELEMENT_TYPE_3FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT3;	break;
		case eELEMENT_TYPE_4FLOATS:	l_dx9CurrentElement.Type = D3DDECLTYPE_FLOAT4;	break;
		case eELEMENT_TYPE_COLOUR:	l_dx9CurrentElement.Type = D3DDECLTYPE_D3DCOLOR;break;
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
		m_bAssigned = CheckDxError( m_pDevice->CreateVertexBuffer( m_uiFilled * sizeof( real), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, & m_pBufferObject, nullptr), "Dx9VertexBuffer :: Initialise - CreateVertexBuffer", false);
	}

	if (m_bAssigned)
	{
		void * l_pBuffer = Lock( 0, m_uiFilled * sizeof( real), eLOCK_FLAG_WRITE_ONLY);
		memcpy( l_pBuffer, m_pBuffer, m_uiFilled * sizeof( real));
		Unlock();
		Root::GetSingleton()->GetBufferManager()->AssignBuffer<real>( m_uiIndex, this);
	}
}

void Dx9VertexBuffer :: Activate()
{
	if (m_bAssigned)
	{
		if (CheckDxError( m_pDevice->SetStreamSource( 0, m_pBufferObject, 0, Vertex::Size * sizeof( real)), cuT( "Dx9VertexBuffer :: Activate - SetStreamSource"), false))
		{
			CheckDxError( m_pDevice->SetVertexDeclaration( m_pDx9Declaration), "Dx9VertexBuffer :: Activate - SetVertexDeclaration", false);
		}
	}
}

void Dx9VertexBuffer :: Deactivate()
{
}

void * Dx9VertexBuffer :: Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags)
{
	return BufferLock(p_uiOffset, p_uiSize, p_uiFlags);
}

void Dx9VertexBuffer :: Unlock()
{
	BufferUnlock();
}

//*************************************************************************************************