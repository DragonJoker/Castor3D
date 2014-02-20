#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Buffer.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

IndexBuffer :: IndexBuffer( RenderSystem * p_pRenderSystem )
	:	CpuBuffer< uint32_t >	( p_pRenderSystem	)
{
}

IndexBuffer :: ~IndexBuffer()
{
}

bool IndexBuffer :: DoCreateBuffer()
{
	if( !m_pBuffer )
	{
		m_pBuffer = m_pRenderSystem->CreateIndexBuffer( GetShared() );
	}

	return m_pBuffer != nullptr;
}

//*************************************************************************************************

VertexBuffer :: VertexBuffer( RenderSystem * p_pRenderSystem, BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements )
//	:	CpuBuffer< real >	( p_pRenderSystem				)
	:	CpuBuffer< uint8_t >	( p_pRenderSystem				)
	,	m_bufferDeclaration		( p_pElements, p_uiNbElements	)
{
}

VertexBuffer :: ~VertexBuffer()
{
}

bool VertexBuffer :: DoCreateBuffer()
{
	if( !m_pBuffer )
	{
		m_pBuffer = m_pRenderSystem->CreateVertexBuffer( m_bufferDeclaration, GetShared() );
	}

	return m_pBuffer != nullptr;
}

//*************************************************************************************************

MatrixBuffer :: MatrixBuffer( RenderSystem * p_pRenderSystem )
	:	CpuBuffer< real >	( p_pRenderSystem	)
{
}

MatrixBuffer :: ~MatrixBuffer()
{
}

bool MatrixBuffer :: Bind( uint32_t p_uiCount )
{
	bool l_bReturn	= false;
	GpuBufferSPtr l_pBuffer = GetGpuBuffer();

	if( l_pBuffer )
	{
		l_bReturn = l_pBuffer->Bind( p_uiCount );
	}

	return l_bReturn;
}

bool MatrixBuffer :: DoCreateBuffer()
{
	if( !m_pBuffer )
	{
		m_pBuffer = m_pRenderSystem->CreateMatrixBuffer( GetShared() );
	}

	return m_pBuffer != nullptr;
}

//*************************************************************************************************

GeometryBuffers :: GeometryBuffers( VertexBufferSPtr p_pVertexBuffer, IndexBufferSPtr p_pIndexBuffer, MatrixBufferSPtr p_pMatrixBuffer )
	:	m_pVertexBuffer	( p_pVertexBuffer	)
	,	m_pIndexBuffer	( p_pIndexBuffer	)
	,	m_pMatrixBuffer	( p_pMatrixBuffer	)
{
}

GeometryBuffers :: ~GeometryBuffers()
{
}

bool GeometryBuffers :: Bind()
{
	bool l_bReturn = m_pVertexBuffer && m_pIndexBuffer;
	
	if( l_bReturn )
	{
		l_bReturn = m_pVertexBuffer->Bind();

		if( l_bReturn )
		{
			if( m_pMatrixBuffer )
			{
				l_bReturn = m_pMatrixBuffer->Bind( 1 );
			}

			if( l_bReturn )
			{
				l_bReturn = m_pIndexBuffer->Bind();

				if( !l_bReturn )
				{
					m_pIndexBuffer->Unbind();
				}
			}
		}
	}

	return l_bReturn;
}

void GeometryBuffers :: Unbind()
{
	if( m_pVertexBuffer && m_pIndexBuffer )
	{
		if( m_pMatrixBuffer )
		{
			m_pMatrixBuffer->Unbind();
		}

		m_pVertexBuffer->Unbind();
		m_pIndexBuffer->Unbind();
	}
}

//*************************************************************************************************

TextureBufferObject :: TextureBufferObject( RenderSystem * p_pRenderSystem )
	:	CpuBuffer< uint8_t >	( p_pRenderSystem					)
	,	m_pixelFormat			( Castor::ePIXEL_FORMAT_A8R8G8B8	)
	,	m_uiSize				( 0									)
	,	m_pBytes				( NULL								)
{
}

TextureBufferObject :: ~TextureBufferObject()
{
}

void TextureBufferObject :: Initialise( ePIXEL_FORMAT const & p_format, uint32_t p_uiSize, uint8_t const * p_pBytes, ShaderProgramBaseSPtr p_pProgram )
{
	m_pixelFormat = p_format;
	m_uiSize = p_uiSize;
	m_pBytes = p_pBytes;
	Resize( p_uiSize );
	memcpy( &m_arrayData[0], m_pBytes, m_uiSize );
	CpuBuffer< uint8_t >::Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, p_pProgram );
}

bool TextureBufferObject :: DoCreateBuffer()
{
	if( !m_pBuffer )
	{
		m_pBuffer = m_pRenderSystem->CreateTextureBuffer( GetShared() );
	}

	return m_pBuffer != nullptr;
}
//*************************************************************************************************
