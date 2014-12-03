#include "Dx10FrameVariableBase.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Dx10Render
{
	DxFrameVariableBase::DxFrameVariableBase( DxRenderSystem * p_pRenderSystem )
		:	m_bPresentInProgram( true )
		,	m_pRenderSystem( p_pRenderSystem )
#if C3DDX10_USE_DXEFFECTS
		,	m_pVariable( NULL )
#endif
	{
	}

	DxFrameVariableBase::~DxFrameVariableBase()
	{
#if C3DDX10_USE_DXEFFECTS
		SafeRelease( m_pVariable );
#else
#endif
	}
#if C3DDX10_USE_DXEFFECTS
	void DxFrameVariableBase::GetVariableLocation( LPCSTR p_pVarName, ID3D10Effect * p_pEffect )
	{
		if ( p_pEffect )
		{
			m_pVariable = p_pEffect->GetVariableByName( p_pVarName );
		}
	}
#else
#endif
}