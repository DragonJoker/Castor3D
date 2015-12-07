#include "Texture.hpp"

#include "RenderSystem.hpp"
#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureBase::TextureBase( eTEXTURE_BASE_TYPE p_baseType, RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_initialised( false )
		, m_baseType( p_baseType )
		, m_type( eTEXTURE_TYPE_2D )
		, m_eMapMode( eTEXTURE_MAP_MODE_NONE )
		, m_index( 0 )
		, m_pSampler( p_renderSystem.GetOwner()->GetDefaultSampler() )
		, m_cpuAccess( p_cpuAccess )
		, m_gpuAccess( p_gpuAccess )
	{
	}

	TextureBase::~TextureBase()
	{
	}

	void TextureBase::SetImage( Castor::PxBufferBaseSPtr p_buffer )
	{
		m_uiDepth = 1;
		m_pPixelBuffer = p_buffer;
	}

	bool TextureBase::Bind()
	{
		return BindAt( m_index );
	}

	void TextureBase::Unbind()
	{
		UnbindFrom( m_index );
	}
}
