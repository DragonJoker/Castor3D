#include "Dx10ColourRenderBuffer.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10TextureRenderer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxColourRenderBuffer::DxColourRenderBuffer( DxRenderSystem * p_pRenderSystem, ePIXEL_FORMAT p_eFormat )
		:	ColourRenderBuffer( p_eFormat	)
		,	m_dxRenderBuffer( p_pRenderSystem, DirectX10::Get( p_eFormat ), eBUFFER_COMPONENT_COLOUR, *this	)
	{
	}

	DxColourRenderBuffer::~DxColourRenderBuffer()
	{
	}

	bool DxColourRenderBuffer::Create()
	{
		return m_dxRenderBuffer.Create();
	}

	void DxColourRenderBuffer::Destroy()
	{
		m_dxRenderBuffer.Destroy();
	}

	bool DxColourRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		return m_dxRenderBuffer.Initialise( p_size );
	}

	void DxColourRenderBuffer::Cleanup()
	{
		m_dxRenderBuffer.Cleanup();
	}

	bool DxColourRenderBuffer::Bind()
	{
		return true;
	}

	void DxColourRenderBuffer::Unbind()
	{
	}

	bool DxColourRenderBuffer::Resize( Castor::Size const & p_size )
	{
		return m_dxRenderBuffer.Resize( p_size );
	}
}
