#include "RasteriserState.hpp"

namespace Castor3D
{
	namespace
	{
		bool g_creatingCurrent = false;
	}

	RasteriserStateWPtr RasteriserState::m_wCurrentState;

	RasteriserState::RasteriserState()
		: m_bChanged( true )
		, m_eFillMode( eFILL_MODE_SOLID )
		, m_eCulledFaces( eFACE_BACK )
		, m_bFrontCCW( true )
		, m_bAntialiasedLines( false )
		, m_fDepthBias( 0.0f )
		, m_bScissor( false )
		, m_bDepthClipping( true )
		, m_bMultisampled( false )
	{
	}

	RasteriserState::~RasteriserState()
	{
	}

	void RasteriserState::CreateCurrent()
	{
		if ( !m_wCurrentState.expired() )
		{
			m_currentState = m_wCurrentState.lock();
		}
		else if ( !g_creatingCurrent )
		{
			g_creatingCurrent = true;
			m_currentState = DoCreateCurrent();
			m_wCurrentState = m_currentState;
			g_creatingCurrent = false;
		}
	}

	void RasteriserState::DestroyCurrent()
	{
		m_currentState.reset();

		if ( m_wCurrentState.expired() )
		{
			m_wCurrentState.reset();
		}
	}
}
