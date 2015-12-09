#include "Dx11FrameVariableBase.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxFrameVariableBase::DxFrameVariableBase( DxRenderSystem * p_renderSystem )
		: m_presentInProgram( true )
		, m_renderSystem( p_renderSystem )
	{
	}

	DxFrameVariableBase::~DxFrameVariableBase()
	{
	}
}
