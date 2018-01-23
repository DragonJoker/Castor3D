/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/RenderingResources.hpp"

#include "Buffer/StagingBuffer.hpp"
#include "Core/Device.hpp"

namespace renderer
{
	RenderingResources::RenderingResources( Device const & device )
		: m_device{ device }
	{
	}
}
