#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/render_system/BufferElement.h"

#include "Castor3D/geometry/basic/Vertex.h"

using namespace Castor3D;

//*************************************************************************************************

void IndexBuffer :: Activate()
{
}

void IndexBuffer :: Deactivate()
{
}

//*************************************************************************************************
VertexBuffer :: VertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
	:	m_bufferDeclaration( p_pElements, p_uiNbElements)
{
}

VertexBuffer :: ~VertexBuffer()
{
}

void VertexBuffer :: Activate()
{
}

void VertexBuffer :: Deactivate()
{
}

void VertexBuffer :: AddElement( Vertex & p_pVertex, bool p_bIncrease)
{
	if (p_bIncrease)
	{
		IncreaseSize( Vertex::Size, true);
	}

	p_pVertex.LinkCoords( & m_buffer[m_filled]);
	m_filled += Vertex::Size;
}

//*************************************************************************************************

TextureBufferObject :: TextureBufferObject()
	:	m_pixelFormat( Castor::Resources::eA8R8G8B8),
		m_uiSize( 0),
		m_pBytes( NULL)
{
}

TextureBufferObject :: ~TextureBufferObject()
{
	Cleanup();
}

void TextureBufferObject :: Activate()
{
}

void TextureBufferObject :: Activate( PassPtr p_pass)
{
}

void TextureBufferObject :: Deactivate()
{
}

void TextureBufferObject :: Initialise( const ePIXEL_FORMAT & p_format, size_t p_uiSize, const unsigned char * p_pBytes)
{
	m_pixelFormat = p_format;
	m_uiSize = p_uiSize;
	m_pBytes = p_pBytes;
}

void TextureBufferObject :: Cleanup()
{
}

//*************************************************************************************************

void BufferManager :: Cleanup()
{
	BufferManager & l_pThis = GetSingleton();
	l_pThis.m_stFloatBuffers.Cleanup();
	l_pThis.m_stDoubleBuffers.Cleanup();
	l_pThis.m_stIntBuffers.Cleanup();
	l_pThis.m_stBoolBuffers.Cleanup();
	l_pThis.m_stCharBuffers.Cleanup();
	l_pThis.m_stUCharBuffers.Cleanup();
	l_pThis.m_stUIntBuffers.Cleanup();
}

void BufferManager :: Update()
{
	BufferManager & l_pThis = GetSingleton();
	l_pThis.m_stFloatBuffers.Update();
	l_pThis.m_stDoubleBuffers.Update();
	l_pThis.m_stIntBuffers.Update();
	l_pThis.m_stBoolBuffers.Update();
	l_pThis.m_stCharBuffers.Update();
	l_pThis.m_stUCharBuffers.Update();
	l_pThis.m_stUIntBuffers.Update();
}

//*************************************************************************************************