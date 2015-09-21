#include "Dx11FrameVariableBase.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxFrameVariableBase::DxFrameVariableBase( DxRenderSystem * p_pRenderSystem )
		: m_bPresentInProgram( true )
		, m_renderSystem( p_pRenderSystem )
	{
	}

	DxFrameVariableBase::~DxFrameVariableBase()
	{
	}
}
