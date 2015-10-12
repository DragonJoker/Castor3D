#include "Dx11DynamicTexture.hpp"

#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxDynamicTexture::DxDynamicTexture( DxRenderSystem & p_renderSystem )
		: DynamicTexture( p_renderSystem )
		, m_texture( p_renderSystem, false )
		, m_renderSystem( &p_renderSystem )
	{
	}

	DxDynamicTexture::~DxDynamicTexture()
	{
	}

	bool DxDynamicTexture::Create()
	{
		return true;
	}

	void DxDynamicTexture::Destroy()
	{
	}

	void DxDynamicTexture::Cleanup()
	{
		DynamicTexture::Cleanup();
		m_texture.Cleanup();
	}

	uint8_t * DxDynamicTexture::Lock( uint32_t p_mode )
	{
		return m_texture.Lock( p_mode );
	}

	void DxDynamicTexture::Unlock( bool p_modified )
	{
		m_texture.Unlock( p_modified );
	}

	bool DxDynamicTexture::DoInitialise()
	{
		bool l_return = m_texture.Initialise( m_type, m_pPixelBuffer, m_uiDepth, GetRenderTarget() != nullptr, GetSamplesCount() );

		if ( l_return )
		{
			m_pPixelBuffer = m_texture.GetPixelBuffer();
		}

		return l_return;
	}

	bool DxDynamicTexture::DoBind( uint32_t p_index )
	{
		return m_texture.Bind( p_index );
	}

	void DxDynamicTexture::DoUnbind( uint32_t p_index )
	{
		m_texture.Unbind( p_index );
	}
}
