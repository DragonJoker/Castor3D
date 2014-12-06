#include "Dx10BlendState.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxBlendState::DxBlendState( DxRenderSystem * p_pRenderSystem )
		:	BlendState()
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pBlendState( NULL )
	{
	}

	DxBlendState::~DxBlendState()
	{
	}

	bool DxBlendState::Initialise()
	{
		D3D10_BLEND_DESC l_blendDesc			= { 0 };
		l_blendDesc.AlphaToCoverageEnable		= FALSE;
		l_blendDesc.SrcBlend					= DirectX10::Get( m_rtStates[0].m_eRgbSrcBlend );
		l_blendDesc.DestBlend					= DirectX10::Get( m_rtStates[0].m_eRgbDstBlend );
		l_blendDesc.BlendOp						= DirectX10::Get( m_rtStates[0].m_eRgbBlendOp );
		l_blendDesc.SrcBlendAlpha				= DirectX10::Get( m_rtStates[0].m_eAlphaSrcBlend );
		l_blendDesc.DestBlendAlpha				= DirectX10::Get( m_rtStates[0].m_eAlphaDstBlend );
		l_blendDesc.BlendOpAlpha				= DirectX10::Get( m_rtStates[0].m_eAlphaBlendOp );

		UINT8 l_writeMask = ( GetColourMaskR() == eWRITING_MASK_ALL ? D3D10_COLOR_WRITE_ENABLE_RED : 0 )
			| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3D10_COLOR_WRITE_ENABLE_GREEN : 0 )
			| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3D10_COLOR_WRITE_ENABLE_BLUE : 0 )
			| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3D10_COLOR_WRITE_ENABLE_ALPHA : 0 );

		for ( uint8_t i = 0; i < 8; i++ )
		{
			l_blendDesc.BlendEnable[i]				= m_rtStates[i].m_bEnableBlend;
			l_blendDesc.RenderTargetWriteMask[i]	= l_writeMask;
		}

		HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateBlendState( &l_blendDesc, &m_pBlendState );
		dxDebugName( m_pBlendState, BlendState );
		m_bChanged = false;
		return dxCheckError( l_hr, "CreateBlendState" );
	}

	void DxBlendState::Cleanup()
	{
		SafeRelease( m_pBlendState );
	}

	bool DxBlendState::Apply()
	{
		bool l_bReturn = true;

		if ( m_bChanged )
		{
			Cleanup();
			l_bReturn = Initialise();
		}

		if ( l_bReturn && m_pBlendState )
		{
			m_pRenderSystem->GetDevice()->OMSetBlendState( m_pBlendState, m_blendFactors.const_ptr(), m_uiSampleMask );
		}

		return l_bReturn;
	}
}
