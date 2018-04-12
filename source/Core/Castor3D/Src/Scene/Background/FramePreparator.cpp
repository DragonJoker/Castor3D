#include "Scene/Background/FramePreparator.hpp"

#include "Scene/Background/Colour.hpp"
#include "Scene/Background/Image.hpp"
#include "Scene/Background/Skybox.hpp"

#include <Command/CommandBuffer.hpp>

namespace castor3d
{
	BackgroundFramePreparator::BackgroundFramePreparator( renderer::CommandBuffer & commandBuffer
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )
		: m_commandBuffer{ commandBuffer }
	{
	}

	BackgroundFramePreparator::~BackgroundFramePreparator()
	{
		m_commandBuffer.end();
	}

	void BackgroundFramePreparator::visit( ColourBackground const & background )
	{
	}

	void BackgroundFramePreparator::visit( SkyboxBackground const & background )
	{
	}

	void BackgroundFramePreparator::visit( ImageBackground const & background )
	{
	}
}
