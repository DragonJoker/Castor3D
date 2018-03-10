/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/SwapChain.hpp"

namespace renderer
{
	SwapChain::SwapChain( Device const & device
		, Extent2D const & size )
		: m_device{ device }
		, m_dimensions{ size }
	{
	}
}
