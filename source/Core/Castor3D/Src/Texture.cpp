#include "Texture.hpp"

#include "RenderSystem.hpp"
#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	Texture::Texture( eTEXTURE_BASE_TYPE p_baseType, RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_initialised( false )
		, m_baseType( p_baseType )
		, m_type( eTEXTURE_TYPE_2D )
		, m_eMapMode( eTEXTURE_MAP_MODE_NONE )
		, m_index( 0 )
		, m_pSampler( p_renderSystem.GetEngine()->GetDefaultSampler() )
		, m_cpuAccess( p_cpuAccess )
		, m_gpuAccess( p_gpuAccess )
	{
	}

	Texture::~Texture()
	{
	}

	void Texture::SetImage( Castor::PxBufferBaseSPtr p_buffer )
	{
		m_uiDepth = 1;
		m_pPixelBuffer = p_buffer;
	}

	bool Texture::Bind()
	{
		return BindAt( m_index );
	}

	void Texture::Unbind()
	{
		UnbindFrom( m_index );
	}
}
