#include "Dx10DepthStencilRenderBuffer.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10TextureRenderer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxDepthStencilRenderBuffer::DxDepthStencilRenderBuffer( DxRenderSystem * p_pRenderSystem, ePIXEL_FORMAT p_eFormat )
		:	DepthStencilRenderBuffer( p_eFormat )
		,	m_dxRenderBuffer( p_pRenderSystem, DirectX10::Get( p_eFormat ), eBUFFER_COMPONENT_DEPTH, *this )
	{
	}

	DxDepthStencilRenderBuffer::~DxDepthStencilRenderBuffer()
	{
	}

	bool DxDepthStencilRenderBuffer::Create()
	{
		return m_dxRenderBuffer.Create();
	}

	void DxDepthStencilRenderBuffer::Destroy()
	{
		m_dxRenderBuffer.Destroy();
	}

	bool DxDepthStencilRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		return m_dxRenderBuffer.Initialise( p_size );
	}

	void DxDepthStencilRenderBuffer::Cleanup()
	{
		m_dxRenderBuffer.Cleanup();
	}

	bool DxDepthStencilRenderBuffer::Bind()
	{
		return true;
	}

	void DxDepthStencilRenderBuffer::Unbind()
	{
	}

	bool DxDepthStencilRenderBuffer::Resize( Castor::Size const & p_size )
	{
		return m_dxRenderBuffer.Resize( p_size );
	}
}
