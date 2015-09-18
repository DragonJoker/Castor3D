#include "Texture.hpp"
#include "RenderSystem.hpp"
#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureBase::TextureBase( eTEXTURE_TYPE p_type, RenderSystem * p_pRenderSystem )
		: m_bInitialised( false )
		, m_type( p_type )
		, m_eDimension( eTEXTURE_DIMENSION_2D )
		, m_eMapMode( eTEXTURE_MAP_MODE_NONE )
		, m_uiIndex( 0 )
		, m_renderSystem( p_pRenderSystem )
		, m_pSampler( p_pRenderSystem->GetEngine()->GetDefaultSampler() )
	{
	}

	TextureBase::~TextureBase()
	{
	}

	void TextureBase::SetImage( Castor::PxBufferBaseSPtr p_pBuffer )
	{
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
