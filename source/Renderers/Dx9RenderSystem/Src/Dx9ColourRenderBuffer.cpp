#include "Dx9ColourRenderBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxColourRenderBuffer::DxColourRenderBuffer( DxRenderSystem * p_pRenderSystem, ePIXEL_FORMAT p_eFormat )
		:	ColourRenderBuffer( p_eFormat )
		,	m_dxRenderBuffer( p_pRenderSystem, DirectX9::Get( p_eFormat ), eBUFFER_COMPONENT_COLOUR, *this )
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
		CASTOR_ASSERT( false );
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
