#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Buffer.hpp"
#include "Castor3D/BufferElement.hpp"

#include "Castor3D/Vertex.hpp"

using namespace Castor3D;

//*************************************************************************************************

//*************************************************************************************************
VertexBuffer :: VertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	m_bufferDeclaration( p_pElements, p_uiNbElements)
{
}

VertexBuffer :: ~VertexBuffer()
{
}

void VertexBuffer :: AddElement( Vertex & p_pVertex, bool p_bIncrease)
{
	if (p_bIncrease)
	{
		resize( Vertex::Size, true);
	}

	p_pVertex.LinkCoords( & m_buffer[m_filled]);
	m_filled += Vertex::Size;
}

//*************************************************************************************************

TextureBufferObject :: TextureBufferObject()
	:	m_pixelFormat( Castor::Resources::ePIXEL_FORMAT_A8R8G8B8),
		m_uiSize( 0),
		m_pBytes( nullptr)
{
}

TextureBufferObject :: ~TextureBufferObject()
{
}

void TextureBufferObject :: Initialise( const ePIXEL_FORMAT & p_format, size_t p_uiSize, unsigned char const * p_pBytes)
{
	m_pixelFormat = p_format;
	m_uiSize = p_uiSize;
	m_pBytes = p_pBytes;
}

//*************************************************************************************************

BufferManager::TBuffers<bool>			BufferManager::m_stBoolBuffers;
BufferManager::TBuffers<char>			BufferManager::m_stCharBuffers;
BufferManager::TBuffers<unsigned char>	BufferManager::m_stUCharBuffers;
BufferManager::TBuffers<int>			BufferManager::m_stIntBuffers;
BufferManager::TBuffers<unsigned int>	BufferManager::m_stUIntBuffers;
BufferManager::TBuffers<float>			BufferManager::m_stFloatBuffers;
BufferManager::TBuffers<double>			BufferManager::m_stDoubleBuffers;

void BufferManager :: Clear()
{
	m_stFloatBuffers.Cleanup();
	m_stDoubleBuffers.Cleanup();
	m_stIntBuffers.Cleanup();
	m_stBoolBuffers.Cleanup();
	m_stCharBuffers.Cleanup();
	m_stUCharBuffers.Cleanup();
	m_stUIntBuffers.Cleanup();
}

void BufferManager :: Update()
{
	m_stFloatBuffers.Update();
	m_stDoubleBuffers.Update();
	m_stIntBuffers.Update();
	m_stBoolBuffers.Update();
	m_stCharBuffers.Update();
	m_stUCharBuffers.Update();
	m_stUIntBuffers.Update();
}

//*************************************************************************************************