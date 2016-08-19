#include "DepthStencilState.hpp"

using namespace Castor;

namespace Castor3D
{
	DepthStencilState::DepthStencilState( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	DepthStencilState::~DepthStencilState()
	{
	}
}
