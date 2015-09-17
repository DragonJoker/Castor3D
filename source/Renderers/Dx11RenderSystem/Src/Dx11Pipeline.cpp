#include "Dx11Pipeline.hpp"

#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxPipelineImpl::DxPipelineImpl( Pipeline & p_pipeline )
		: IPipelineImpl( p_pipeline )
		, m_viewport()
	{
		m_viewport.MaxDepth = 1.0f;
	}

	DxPipelineImpl::~DxPipelineImpl()
	{
	}

	void DxPipelineImpl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
	{
		m_viewport.Width = FLOAT( p_iWindowWidth );
		m_viewport.Height = FLOAT( p_iWindowHeight );
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pipeline.GetRenderSystem()->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->RSSetViewports( 1, &m_viewport );
	}
}
