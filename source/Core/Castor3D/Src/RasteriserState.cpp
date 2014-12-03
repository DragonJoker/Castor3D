#include "RasteriserState.hpp"

using namespace Castor3D;

//*************************************************************************************************

RasteriserState::RasteriserState()
	:	m_bChanged( true )
	,	m_eFillMode( eFILL_MODE_SOLID )
	,	m_eCulledFaces( eFACE_BACK )
	,	m_bFrontCCW( true )
	,	m_bAntialiasedLines( false )
	,	m_fDepthBias( 0.0f )
	,	m_bScissor( false )
	,	m_bDepthClipping( true )
	,	m_bMultisampled( false )
{
}

RasteriserState::~RasteriserState()
{
}

//*************************************************************************************************