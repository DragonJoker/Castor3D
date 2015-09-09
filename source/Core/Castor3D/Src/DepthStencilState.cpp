#include "DepthStencilState.hpp"

namespace Castor3D
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	DepthStencilStateWPtr DepthStencilState::m_wCurrentState;

	DepthStencilState::DepthStencilState()
		: m_bChanged( true )
		, m_eDepthFunc( eDEPTH_FUNC_LESS )
		, m_bDepthTest( true )
		, m_dDepthNear( 0 )
		, m_dDepthFar( 1 )
		, m_eDepthMask( eWRITING_MASK_ALL )
		, m_bStencilTest( true )
		, m_uiStencilReadMask( 0xFFFFFFFF )
		, m_uiStencilWriteMask( 0xFFFFFFFF )
	{
		m_stStencilFront.m_eFunc = eSTENCIL_FUNC_ALWAYS;
		m_stStencilFront.m_eFailOp = eSTENCIL_OP_KEEP;
		m_stStencilFront.m_eDepthFailOp = eSTENCIL_OP_INCR;
		m_stStencilFront.m_ePassOp = eSTENCIL_OP_KEEP;
		m_stStencilFront.m_ref = 0;
		m_stStencilBack.m_eFunc = eSTENCIL_FUNC_ALWAYS;
		m_stStencilBack.m_eFailOp = eSTENCIL_OP_KEEP;
		m_stStencilBack.m_eDepthFailOp = eSTENCIL_OP_INCR;
		m_stStencilBack.m_ePassOp = eSTENCIL_OP_KEEP;
		m_stStencilBack.m_ref = 0;
	}

	DepthStencilState::~DepthStencilState()
	{
	}

	void DepthStencilState::CreateCurrent()
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

	void DepthStencilState::DestroyCurrent()
	{
		m_currentState.reset();

		if ( m_wCurrentState.expired() )
		{
			m_wCurrentState.reset();
		}
	}
}
