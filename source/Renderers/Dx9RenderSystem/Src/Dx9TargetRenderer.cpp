#include "Dx9TargetRenderer.hpp"
#include "Dx9RenderSystem.hpp"
#include "Dx9TextureAttachment.hpp"
#include "Dx9RenderBufferAttachment.hpp"
#include "Dx9ColourRenderBuffer.hpp"
#include "Dx9DepthStencilRenderBuffer.hpp"
#include "Dx9FrameBuffer.hpp"

#include <RenderTarget.hpp>
#include <Scene.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx9Render
{
	DxTargetRenderer::DxTargetRenderer( DxRenderSystem * p_pRenderSystem )
		:	TargetRenderer( p_pRenderSystem	)
		,	m_bInitialised( false	)
	{
	}

	DxTargetRenderer::~DxTargetRenderer()
	{
	}

	bool DxTargetRenderer::Initialise()
	{
		m_bInitialised = true;
		return m_bInitialised;
	}

	void DxTargetRenderer::Cleanup()
	{
		m_bInitialised = false;
	}

	void DxTargetRenderer::BeginScene()
	{
		IDirect3DDevice9 * l_pDevice = reinterpret_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();

		if ( m_bInitialised )
		{
			DoUpdateSize();

			if ( m_target->GetScene() )
			{
				l_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, m_target->GetScene()->GetBackgroundColour().to_argb(), 1.0f, 0x00 );
			}
			else
			{
				l_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x80808080, 1.0f, 0x00 );
			}

#if !DX_DEBUG
			l_pDevice->BeginScene();
#endif
		}
	}

	void DxTargetRenderer::EndScene()
	{
#if !DX_DEBUG

		if ( m_bInitialised )
		{
			reinterpret_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice()->EndScene();
		}

#endif
	}

	void DxTargetRenderer::DoUpdateSize()
	{
	}

	RenderBufferAttachmentSPtr DxTargetRenderer::CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
	{
		RenderBufferAttachmentSPtr l_pReturn;

		if ( p_pRenderBuffer->GetComponent() == eBUFFER_COMPONENT_COLOUR )
		{
			l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_pRenderSystem ), std::static_pointer_cast< DxColourRenderBuffer >( p_pRenderBuffer ) );
		}
		else
		{
			l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_pRenderSystem ), std::static_pointer_cast< DxDepthStencilRenderBuffer >( p_pRenderBuffer ) );
		}

		return l_pReturn;
	}

	TextureAttachmentSPtr DxTargetRenderer::CreateAttachment( DynamicTextureSPtr p_pTexture )const
	{
		return std::make_shared< DxTextureAttachment >( static_cast< DxRenderSystem * >( m_pRenderSystem ), p_pTexture );
	}

	FrameBufferSPtr DxTargetRenderer::CreateFrameBuffer()const
	{
		return std::make_shared< DxFrameBuffer >( static_cast< DxRenderSystem * >( m_pRenderSystem ) );
	}
}
