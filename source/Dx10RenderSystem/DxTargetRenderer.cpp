#include "Dx10RenderSystem/PrecompiledHeader.hpp"

#include "Dx10RenderSystem/DxTargetRenderer.hpp"
#include "Dx10RenderSystem/DxRenderSystem.hpp"
#include "Dx10RenderSystem/DxFrameBuffer.hpp"

using namespace Castor;
using namespace Castor3D;
using namespace Dx10Render;

DxTargetRenderer :: DxTargetRenderer( DxRenderSystem * p_pRenderSystem )
	:	TargetRenderer		( p_pRenderSystem	)
	,	m_bInitialised		( false				)
{
}

DxTargetRenderer :: ~DxTargetRenderer()
{
}

bool DxTargetRenderer :: Initialise()
{
	m_bInitialised = true;

	return m_bInitialised;
}

void DxTargetRenderer :: Cleanup()
{
	m_bInitialised = false;
}

void DxTargetRenderer :: BeginScene()
{
	if( m_bInitialised )
	{
		DoUpdateSize();
		DxContext * l_pContext = static_cast< DxContext* >( m_pRenderSystem->GetCurrentContext() );
		DxRenderSystem * l_pRS = static_cast< DxRenderSystem* >( m_pRenderSystem );
		
		// Clear the back buffer.
		if( m_target->GetScene() )
		{
			l_pRS->GetDevice()->ClearRenderTargetView( l_pContext->GetRenderTargetView(), m_target->GetScene()->GetBackgroundColour().const_ptr() );
		}
		else
		{
			float l_fColour[4] = { 0.5, 0.5, 0.5, 1.0 };
			l_pRS->GetDevice()->ClearRenderTargetView( l_pContext->GetRenderTargetView(), l_fColour );
		}
		// Clear the depth buffer.
		l_pRS->GetDevice()->ClearDepthStencilView( l_pContext->GetDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f,  0 );
	}
}

void DxTargetRenderer :: EndScene()
{
// 	HRESULT l_hr = S_OK;
// 
// 	if( m_bInitialised )
// 	{
// 		l_hr = reinterpret_cast< Dx10RenderSystem* >( m_pRenderSystem )->GetDevice()->EndScene();
// 	}
}

void DxTargetRenderer :: DoUpdateSize()
{
//	Size l_size( ::GetSystemMetrics( SM_CXFULLSCREEN ), ::GetSystemMetrics( SM_CYFULLSCREEN ) );
//	m_target->SetSize( l_size );
}

RenderBufferAttachmentSPtr DxTargetRenderer :: CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	RenderBufferAttachmentSPtr l_pReturn;

	if( p_pRenderBuffer->GetComponent() == eBUFFER_COMPONENT_COLOUR )
	{
		l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem* >( m_pRenderSystem ), std::static_pointer_cast< DxColourRenderBuffer >( p_pRenderBuffer ) );
	}
	else
	{
		l_pReturn = std::make_shared< DxRenderBufferAttachment >( static_cast< DxRenderSystem* >( m_pRenderSystem ), std::static_pointer_cast< DxDepthStencilRenderBuffer >( p_pRenderBuffer ) );
	}

	return l_pReturn;
}

TextureAttachmentSPtr DxTargetRenderer :: CreateAttachment( DynamicTextureSPtr p_pTexture )const
{
	return std::make_shared< DxTextureAttachment >( static_cast< DxRenderSystem * >( m_pRenderSystem ), p_pTexture );
}

FrameBufferSPtr DxTargetRenderer :: CreateFrameBuffer()const
{
	return std::make_shared< DxFrameBuffer >( static_cast< DxRenderSystem * >( m_pRenderSystem ) );
}
