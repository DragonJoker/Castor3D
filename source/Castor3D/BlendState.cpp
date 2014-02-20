#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/BlendState.hpp"

using namespace Castor3D;

//*************************************************************************************************

BlendState :: BlendState()
	:	m_bChanged					( true	)
	,	m_bEnableAlphaToCoverage	( false	)
	,	m_bIndependantBlend			( false	)
{
	memset( &m_rtStates[0], 0, sizeof( stRT_BLEND_STATE ) * 8 );

	m_rtStates[0].m_bEnableBlend	= false;
	m_rtStates[0].m_eRgbSrcBlend	= eBLEND_ONE;
	m_rtStates[0].m_eRgbDstBlend	= eBLEND_ZERO;
	m_rtStates[0].m_eRgbBlendOp		= eBLEND_OP_ADD;
	m_rtStates[0].m_eAlphaSrcBlend	= eBLEND_ONE;
	m_rtStates[0].m_eAlphaDstBlend	= eBLEND_ZERO;
	m_rtStates[0].m_eAlphaBlendOp	= eBLEND_OP_ADD;
	m_rtStates[0].m_uiWriteMask		= 0xFF;
}

BlendState :: ~BlendState()
{
}

//*************************************************************************************************