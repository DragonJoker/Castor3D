/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "SwapChain.hpp"

namespace renderer
{
	SwapChain::SwapChain( Device const & device
		, IVec2 const & size )
		: m_device{ device }
		, m_dimensions{ size }
	{
	}
}
