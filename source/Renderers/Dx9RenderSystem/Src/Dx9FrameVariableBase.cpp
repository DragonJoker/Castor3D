#include "Dx9FrameVariableBase.hpp"

using namespace Castor3D;

namespace Dx9Render
{
	DxFrameVariableBase::DxFrameVariableBase( DxRenderSystem * p_pRenderSystem )
		:	m_bPresentInProgram( true )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_d3dxHandle( NULL )
	{
	}

	DxFrameVariableBase::~DxFrameVariableBase()
	{
	}

	void DxFrameVariableBase::GetVariableLocation( LPCSTR p_pVarName, ID3DXEffect * p_pEffect )
	{
		if ( p_pEffect )
		{
			m_d3dxHandle = p_pEffect->GetParameterByName( NULL, p_pVarName );
		}
		else
		{
			CASTOR_ASSERT( false );
		}
	}
}
