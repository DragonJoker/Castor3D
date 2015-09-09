#include "BlendState.hpp"

namespace Castor3D
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	BlendStateWPtr BlendState::m_wCurrentState;

	BlendState::BlendState()
		: m_bChanged( true )
		, m_bEnableAlphaToCoverage( false )
		, m_bIndependantBlend( false )
	{
		m_eColourMask[0] = eWRITING_MASK_ALL;
		m_eColourMask[1] = eWRITING_MASK_ALL;
		m_eColourMask[2] = eWRITING_MASK_ALL;
		m_eColourMask[3] = eWRITING_MASK_ALL;
		memset( &m_rtStates[0], 0, sizeof( stRT_BLEND_STATE ) * 8 );
		m_rtStates[0].m_bEnableBlend = false;
		m_rtStates[0].m_eRgbSrcBlend = eBLEND_ONE;
		m_rtStates[0].m_eRgbDstBlend = eBLEND_ZERO;
		m_rtStates[0].m_eRgbBlendOp = eBLEND_OP_ADD;
		m_rtStates[0].m_eAlphaSrcBlend = eBLEND_ONE;
		m_rtStates[0].m_eAlphaDstBlend = eBLEND_ZERO;
		m_rtStates[0].m_eAlphaBlendOp = eBLEND_OP_ADD;
		m_rtStates[0].m_uiWriteMask = 0xFF;
	}

	BlendState::~BlendState()
	{
	}

	void BlendState::CreateCurrent()
	{
		if ( !m_wCurrentState.expired() )
		{
			m_currentState = m_wCurrentState.lock();
		}
		else
		{
			if ( !g_creatingCurrent )
			{
				g_creatingCurrent = true;
				m_currentState = DoCreateCurrent();
				m_wCurrentState = m_currentState;
			}
		}
	}

	void BlendState::DestroyCurrent()
	{
		m_currentState.reset();

		if ( m_wCurrentState.expired() )
		{
			m_wCurrentState.reset();
		}
	}
}
