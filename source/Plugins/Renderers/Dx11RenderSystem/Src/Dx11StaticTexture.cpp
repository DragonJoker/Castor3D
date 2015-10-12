#include "Dx11StaticTexture.hpp"
#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxStaticTexture::DxStaticTexture( DxRenderSystem & p_renderSystem )
		: StaticTexture( p_renderSystem )
		, m_texture( p_renderSystem, true )
		, m_renderSystem( &p_renderSystem )
	{
	}

	DxStaticTexture::~DxStaticTexture()
	{
	}

	bool DxStaticTexture::Create()
	{
		return true;
	}

	void DxStaticTexture::Destroy()
	{
	}

	void DxStaticTexture::Cleanup()
	{
		StaticTexture::Cleanup();
		m_texture.Cleanup();
	}

	uint8_t * DxStaticTexture::Lock( uint32_t p_mode )
	{
		return m_texture.Lock( p_mode );
	}

	void DxStaticTexture::Unlock( bool p_modified )
	{
		m_texture.Unlock( p_modified );
	}

	bool DxStaticTexture::DoInitialise()
	{
		bool l_return = m_texture.Initialise( GetType(), m_pPixelBuffer, m_uiDepth, false, 0 );

		if ( l_return )
		{
			m_pPixelBuffer = m_texture.GetPixelBuffer();
		}

		return l_return;
	}

	bool DxStaticTexture::DoBind( uint32_t p_index )
	{
		return m_texture.Bind( p_index );
	}

	void DxStaticTexture::DoUnbind( uint32_t p_index )
	{
		m_texture.Unbind( p_index );
	}
}
