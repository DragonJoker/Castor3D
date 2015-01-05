#include "Texture.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureBase::TextureBase( eTEXTURE_TYPE p_eType, RenderSystem * p_pRenderSystem )
		:	m_bInitialised( false )
		,	m_eType( p_eType )
		,	m_eDimension( eTEXTURE_DIMENSION_2D )
		,	m_eMapMode( eTEXTURE_MAP_MODE_NONE )
		,	m_uiIndex( 0 )
		,	m_pRenderSystem( p_pRenderSystem )
	{
	}

	TextureBase::~TextureBase()
	{
	}

	void TextureBase::SetImage( Castor::PxBufferBaseSPtr p_pBuffer )
	{
		m_pPixelBuffer = p_pBuffer;
	}
}
