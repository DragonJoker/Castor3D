#include "Dx11ColourRenderBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxColourRenderBuffer::DxColourRenderBuffer( DxRenderSystem * p_renderSystem, ePIXEL_FORMAT p_eFormat )
		: ColourRenderBuffer( p_eFormat )
		, m_dxRenderBuffer( p_renderSystem, DirectX11::Get( p_eFormat ), eBUFFER_COMPONENT_COLOUR, *this )
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
		bool l_return = m_dxRenderBuffer.Initialise( p_size );

		if ( l_return )
		{
			m_size = p_size;
		}

		return l_return;
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
