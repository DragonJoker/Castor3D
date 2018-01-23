/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/SwapChain.hpp"

namespace renderer
{
	SwapChain::SwapChain( Device const & device
		, UIVec2 const & size )
		: m_device{ device }
		, m_dimensions{ size }
	{
	}
}
