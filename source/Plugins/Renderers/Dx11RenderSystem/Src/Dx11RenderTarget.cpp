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

DxRenderTarget::DxRenderTarget( DxRenderSystem * p_renderSystem, eTARGET_TYPE p_type )
	: RenderTarget( *p_renderSystem->GetOwner(), p_type )
	, m_renderSystem( p_renderSystem )
{
}

DxRenderTarget::~DxRenderTarget()
{
}

RenderBufferAttachmentSPtr DxRenderTarget::CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	RenderBufferAttachmentSPtr l_return;

	if ( p_pRenderBuffer->GetComponent() == eBUFFER_COMPONENT_COLOUR )
	{
		l_return = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), std::static_pointer_cast< DxColourRenderBuffer >( p_pRenderBuffer ) );
	}
	else
	{
		l_return = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), std::static_pointer_cast< DxDepthStencilRenderBuffer >( p_pRenderBuffer ) );
	}

	return l_return;
}

TextureAttachmentSPtr DxRenderTarget::CreateAttachment( DynamicTextureSPtr p_texture )const
{
	return std::make_shared< DxTextureAttachment >( static_cast< DxRenderSystem * >( m_renderSystem ), p_texture );
}

FrameBufferSPtr DxRenderTarget::CreateFrameBuffer()const
{
	return std::make_shared< DxFrameBuffer >( static_cast< DxRenderSystem * >( m_renderSystem ) );
}
