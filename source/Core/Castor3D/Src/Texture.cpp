#include "Texture.hpp"

#include "RenderSystem.hpp"
#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureBase::TextureBase( eTEXTURE_BASE_TYPE p_baseType, RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_bInitialised( false )
		, m_baseType( p_baseType )
		, m_type( eTEXTURE_TYPE_2D )
		, m_eMapMode( eTEXTURE_MAP_MODE_NONE )
		, m_uiIndex( 0 )
		, m_pSampler( p_renderSystem.GetOwner()->GetDefaultSampler() )
	{
	}

	TextureBase::~TextureBase()
	{
	}

	void TextureBase::SetImage( Castor::PxBufferBaseSPtr p_pBuffer )
	{
		m_uiDepth = 1;
		m_pPixelBuffer = p_pBuffer;
	}

	bool TextureBase::Bind()
	{
		return BindAt( m_uiIndex );
	}

	void TextureBase::Unbind()
	{
		UnbindFrom( m_uiIndex );
	}
}
