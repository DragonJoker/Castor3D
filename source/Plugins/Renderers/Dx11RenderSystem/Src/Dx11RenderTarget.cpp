#include "Dx11RenderTarget.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameBuffer.hpp"
#include "Dx11ColourRenderBuffer.hpp"
#include "Dx11DepthStencilRenderBuffer.hpp"

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

FrameBufferSPtr DxRenderTarget::CreateFrameBuffer()
{
	return std::make_shared< DxFrameBuffer >( static_cast< DxRenderSystem * >( m_renderSystem ) );
}
