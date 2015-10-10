#include "TextureBuffer.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureBuffer::TextureBuffer( Engine & p_engine )
		: CpuBuffer< uint8_t >( p_engine )
		, m_pixelFormat( Castor::ePIXEL_FORMAT_A8R8G8B8 )
		, m_uiSize( 0 )
		, m_pBytes( NULL )
	{
	}

	TextureBuffer::~TextureBuffer()
	{
	}

	void TextureBuffer::Initialise( ePIXEL_FORMAT const & p_format, uint32_t p_uiSize, uint8_t const * p_pBytes, ShaderProgramBaseSPtr p_pProgram )
	{
		m_pixelFormat = p_format;
		m_uiSize = p_uiSize;
		m_pBytes = p_pBytes;
		Resize( p_uiSize );
		memcpy( &m_arrayData[0], m_pBytes, m_uiSize );
		CpuBuffer< uint8_t >::Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, p_pProgram );
	}

	bool TextureBuffer::DoCreateBuffer()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetOwner()->GetRenderSystem()->CreateTextureBuffer( this );
		}

		return m_pBuffer != nullptr;
	}
}
