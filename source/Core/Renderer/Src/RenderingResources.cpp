/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderingResources.hpp"

#include "Device.hpp"
#include "StagingBuffer.hpp"

namespace renderer
{
	RenderingResources::RenderingResources( Device const & device )
		: m_device{ device }
	{
	}
}
