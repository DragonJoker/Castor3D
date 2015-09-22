#include "Dx11RenderTarget.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameBuffer.hpp"
#include "Dx11ColourRenderBuffer.hpp"
#include "Dx11DepthStencilRenderBuffer.hpp"
#include "Dx11TextureAttachment.hpp"
#include "Dx11RenderBufferAttachment.hpp"

#include <RenderTarget.hpp>
#include <Scene.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace Dx11Render;

DxRenderTarget::DxRenderTarget( DxRenderSystem * p_pRenderSystem, eTARGET_TYPE p_type )
	: RenderTarget( p_pRenderSystem->GetEngine(), p_type )
	, m_renderSystem( p_pRenderSystem )
{
}

DxRenderTarget::~DxRenderTarget()
{
}

void DxRenderTarget::Clear()
{
	DxContextRPtr l_pContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() );
	ID3D11DeviceContext * l_pDeviceContext = l_pContext->GetDeviceContext();
	SceneSPtr l_scene = GetScene();

	// Clear the back buffer.
	if ( l_scene )
	{
		l_pDeviceContext->ClearRenderTargetView( l_pContext->GetRenderTargetView(), l_scene->GetBackgroundColour().const_ptr() );
	}
	else
	{
		float l_fColour[4] = { 0.5, 0.5, 0.5, 1.0 };
		l_pDeviceContext->ClearRenderTargetView( l_pContext->GetRenderTargetView(), l_fColour );
	}

	// Clear the depth buffer.
	l_pDeviceContext->ClearDepthStencilView( l_pContext->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f,  0 );
}

RenderBufferAttachmentSPtr DxRenderTarget::CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	RenderBufferAttachmentSPtr l_pReturn;

	if ( p_pRenderBuffer->GetComponent() == eBUFFER_COMPONENT_COLOUR )
	{
		l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), std::static_pointer_cast< DxColourRenderBuffer >( p_pRenderBuffer ) );
	}
	else
	{
		l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), std::static_pointer_cast< DxDepthStencilRenderBuffer >( p_pRenderBuffer ) );
	}

	return l_pReturn;
}

TextureAttachmentSPtr DxRenderTarget::CreateAttachment( DynamicTextureSPtr p_pTexture )const
{
	return std::make_shared< DxTextureAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), p_pTexture );
}

FrameBufferSPtr DxRenderTarget::CreateFrameBuffer()const
{
	return std::make_shared< DxFrameBuffer >( static_cast< DxRenderSystem * >( m_renderSystem ) );
}
